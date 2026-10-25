#!/opt/homebrew/bin/python3
"""Poll upstream PR #189 and submit macOS notifications for changes."""

import argparse
import copy
import datetime
import fcntl
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parent
STATE = ROOT / "state.json"
GH = "/opt/homebrew/bin/gh"
REPOSITORY = "futurerestore/futurerestore"
NUMBER = 189
URL = f"https://github.com/{REPOSITORY}/pull/{NUMBER}"
FIELDS = (
    "state,isDraft,mergeable,mergeStateStatus,reviewDecision,headRefOid,"
    "comments,reviews,statusCheckRollup"
)
APPLESCRIPT = """
on run argv
    display notification (item 2 of argv) with title (item 1 of argv)
end run
"""


def log(message):
    now = datetime.datetime.now(datetime.timezone.utc).isoformat()
    print(f"{now} {message}", flush=True)


def gh_json(arguments):
    environment = os.environ.copy()
    environment.update({"GH_PAGER": "", "GH_PROMPT_DISABLED": "1"})
    result = subprocess.run(
        [GH, *arguments],
        text=True,
        capture_output=True,
        timeout=45,
        env=environment,
        check=True,
    )
    return json.loads(result.stdout)


def comment_fingerprints(comments):
    result = {}
    for comment in comments:
        identifier = str(comment["id"])
        # Keep only hashes locally, not comment bodies or authentication data.
        content = {
            key: comment.get(key)
            for key in ("body", "state", "updated_at", "updatedAt", "submittedAt")
        }
        result[identifier] = hashlib.sha256(
            json.dumps(content, sort_keys=True).encode()
        ).hexdigest()
    return result


def fetch_snapshot():
    pr = gh_json(
        ["pr", "view", str(NUMBER), "--repo", REPOSITORY, "--json", FIELDS]
    )
    pages = gh_json(
        [
            "api", "--paginate", "--slurp",
            f"repos/{REPOSITORY}/pulls/{NUMBER}/comments?per_page=100",
        ]
    )
    inline = [comment for page in pages for comment in page]
    # Ignore timestamps, ordering, and URLs that do not represent a CI change.
    checks = [
        {
            key: check.get(key)
            for key in (
                "__typename", "name", "workflowName", "context",
                "state", "status", "conclusion",
            )
        }
        for check in pr.get("statusCheckRollup") or []
    ]
    return {
        "status": {
            key: pr.get(key)
            for key in ("state", "isDraft", "reviewDecision", "headRefOid")
        },
        "merge": {
            key: pr.get(key) for key in ("mergeable", "mergeStateStatus")
        },
        "comments": comment_fingerprints(pr.get("comments") or []),
        "reviews": comment_fingerprints(pr.get("reviews") or []),
        "inline_comments": comment_fingerprints(inline),
        "checks": sorted(checks, key=lambda item: json.dumps(item, sort_keys=True)),
    }


def changes(previous, current):
    messages = []
    if previous["status"] != current["status"]:
        messages.append(f"PR status updated ({current['status']['state']})")
    # GitHub may briefly report UNKNOWN while recalculating mergeability.
    merge_known = (
        current["merge"].get("mergeable") not in (None, "UNKNOWN")
        and current["merge"].get("mergeStateStatus") not in (None, "UNKNOWN")
    )
    if merge_known and previous["merge"] != current["merge"]:
        messages.append("Merge status changed")
    for key, label in (
        ("comments", "PR comments"),
        ("reviews", "Reviews"),
        ("inline_comments", "Inline comments"),
    ):
        if previous[key] != current[key]:
            messages.append(f"{label} changed ({len(current[key])} total)")
    if previous["checks"] != current["checks"]:
        messages.append("CI checks changed")
    return messages


def notify(message):
    # Arguments are passed separately; PR text cannot inject AppleScript.
    subprocess.run(
        [
            "/usr/bin/osascript", "-e", APPLESCRIPT,
            f"futurerestore PR #{NUMBER}", f"{message}\n{URL}",
        ],
        text=True,
        capture_output=True,
        timeout=20,
        check=True,
    )


def write_state(snapshot):
    payload = {
        "schema": 1,
        "url": URL,
        "last_checked_at": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "snapshot": snapshot,
    }
    temporary = STATE.with_suffix(".tmp")
    temporary.write_text(json.dumps(payload, indent=2) + "\n")
    os.chmod(temporary, 0o600)
    temporary.replace(STATE)


def poll():
    with (ROOT / "watch.lock").open("a") as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            return
        current = fetch_snapshot()
        if not STATE.exists():
            write_state(current)
            log("Baseline saved; watching " + URL)
            return
        previous = json.loads(STATE.read_text())["snapshot"]
        # Preserve the last known merge status through transient UNKNOWN values.
        if (
            current["merge"].get("mergeable") in (None, "UNKNOWN")
            or current["merge"].get("mergeStateStatus") in (None, "UNKNOWN")
        ):
            current["merge"] = previous["merge"]
        messages = changes(previous, current)
        if messages:
            message = "; ".join(messages)
            notify(message)
            log(message)
        # Failed API/notification calls never advance the baseline, so launchd
        # can retry during its next scheduled invocation.
        write_state(current)


def self_test():
    baseline = {
        "status": {
            "state": "OPEN", "isDraft": False,
            "reviewDecision": "", "headRefOid": "abc",
        },
        "merge": {"mergeable": "MERGEABLE", "mergeStateStatus": "UNSTABLE"},
        "comments": {}, "reviews": {}, "inline_comments": {}, "checks": [],
    }
    assert changes(baseline, copy.deepcopy(baseline)) == []
    for key in ("comments", "reviews", "inline_comments"):
        modified = copy.deepcopy(baseline)
        modified[key] = {"1": "new hash"}
        assert len(changes(baseline, modified)) == 1
    modified = copy.deepcopy(baseline)
    modified["status"]["state"] = "MERGED"
    assert "MERGED" in changes(baseline, modified)[0]
    modified = copy.deepcopy(baseline)
    modified["checks"] = [{"name": "build", "conclusion": "FAILURE"}]
    assert changes(baseline, modified) == ["CI checks changed"]
    modified = copy.deepcopy(baseline)
    modified["merge"] = {"mergeable": "UNKNOWN", "mergeStateStatus": "UNKNOWN"}
    assert changes(baseline, modified) == []
    assert comment_fingerprints([{"id": 1, "body": "old"}]) != (
        comment_fingerprints([{"id": 1, "body": "edited"}])
    )
    assert "private text" not in json.dumps(
        comment_fingerprints([{"id": 1, "body": "private text"}])
    )
    log("Self-tests passed: deduplication, feedback edits, status, CI, and hashes.")


def main():
    os.umask(0o077)
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--test-notification", action="store_true")
    args = parser.parse_args()
    try:
        if args.self_test:
            self_test()
        elif args.test_notification:
            notify("Alerts enabled. Checking every minute while logged in.")
            log("Setup notification submitted to macOS.")
        else:
            poll()
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        # Avoid logging API response bodies or GitHub credentials.
        log(f"Check failed ({type(error).__name__}); retrying next scheduled run.")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
