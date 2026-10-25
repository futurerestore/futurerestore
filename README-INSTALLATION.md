# Local macOS source installation

This records the source installation and cleanup performed on October 25, 2026
(UTC). The correct project name is **futurerestore**, not `futurestore`. It was
not installed using pip or npm; macOS `install` is a file-copy utility, not a
package manager.

## Installed result

- Upstream: <https://github.com/futurerestore/futurerestore>
- Branch: `dev`
- Commit: `a15bfde42a20d6d2cd0f5b9de4e73f929cd0cc5f`
- Reported version: `v2.0.0`, build 348
- Platform: Apple Silicon macOS, native `arm64`
- Source checkout: `/Users/Apple/futurerestore`
- Installed executable: `/Users/Apple/futurerestore/install/bin/futurerestore`
- PATH symlink: `/opt/homebrew/bin/futurerestore`

The executable incorporates the privately built libraries statically, but still
depends on Homebrew and macOS dynamic libraries. Keep the installed Homebrew
runtime dependencies and `install/`. No restore or device modification was
performed; verification used `--help` only.

## 1. Prerequisites and source

Xcode and Homebrew were already installed. Most device libraries and Autotools
were also already available. The additional packages installed were:

```sh
HOMEBREW_NO_AUTO_UPDATE=1 brew install cmake libpng openssl@3
```

For another machine, the complete build prerequisites also include `autoconf`,
`automake`, `libtool`, `pkgconf`, `libzip`, `libplist`, `libimobiledevice`,
`libimobiledevice-glue`, `libusbmuxd`, and `libirecovery`. macOS supplies curl,
zlib, compression, CoreFoundation, and IOKit. These instructions assume
Homebrew is under `/opt/homebrew`.

For a new checkout:

```sh
git clone --recursive https://github.com/futurerestore/futurerestore.git \
  /Users/Apple/futurerestore
git -C /Users/Apple/futurerestore switch dev
git -C /Users/Apple/futurerestore checkout a15bfde42a20d6d2cd0f5b9de4e73f929cd0cc5f
git -C /Users/Apple/futurerestore submodule update --init --recursive
```

The installed checkout remained on `dev` at that commit. Do not clone over the
existing directory. For a rebuild after cleanup, reuse it and recreate the
dependency directories below.

Set these build variables in the same shell used for the subsequent commands:

```sh
export ROOT=/Users/Apple/futurerestore
export PATH="/opt/homebrew/bin:/opt/homebrew/sbin:$PATH"
export SDKROOT="$(xcrun --sdk macosx --show-sdk-path)"
export CC="$(xcrun --find clang)"
export CXX="$(xcrun --find clang++)"
export CFLAGS="-isysroot $SDKROOT"
export CXXFLAGS="-isysroot $SDKROOT"
export LDFLAGS="-isysroot $SDKROOT"
export PKG_CONFIG_PATH="$ROOT/dep_root/lib/pkgconfig:/opt/homebrew/opt/openssl@3/lib/pkgconfig:/opt/homebrew/lib/pkgconfig:$SDKROOT/usr/lib/pkgconfig"
mkdir -p "$ROOT/build-deps"
```

Explicit SDK selection fixed `ld: library 'System' not found`. Selecting
Homebrew's pkg-config and OpenSSL 3 avoids mixing MacPorts libraries or the
already-installed OpenSSL 4 into this build.

## 2. Build private dependencies

The ARM64 dependency bundle used by upstream CI could not be downloaded with
TLS verification because its CDN certificate had expired. Certificate
verification was not disabled; the missing libraries were built from source.

```sh
for repo in tihmstar/libgeneral tihmstar/libfragmentzip tihmstar/libinsn \
  tihmstar/img3tool tihmstar/img4tool Cryptiiiic/libpatchfinder \
  Cryptiiiic/libipatcher futurerestore/libtatsu; do
  git clone --recursive "https://github.com/$repo.git" \
    "$ROOT/build-deps/${repo##*/}" || break
done
git -C "$ROOT/build-deps/libpatchfinder" switch dev
git -C "$ROOT/build-deps/libipatcher" switch dev
git -C "$ROOT/build-deps/libipatcher" submodule update --init --recursive
```

The `dev` branches of the Cryptiiiic forks provide the newer libpatchfinder
integration. Dependency branch tips can change; these commands describe the
successful route, not a fully pinned dependency lockfile.

Build the first five libraries in dependency order. Stop and resolve any failed
command before proceeding:

```sh
for dep in libgeneral libfragmentzip libinsn img3tool img4tool; do
  (
    cd "$ROOT/build-deps/$dep" &&
    ./autogen.sh --prefix="$ROOT/dep_root" --enable-static --disable-shared &&
    make -j4 &&
    make install
  ) || break
done
```

Build only the static patchfinder target. This avoids upstream helper targets
that assume GNU `sed` and GNU `ln`, which differ from macOS utilities:

```sh
ln -s "$ROOT/dep_root" "$ROOT/build-deps/libpatchfinder/dep_root"
cmake -S "$ROOT/build-deps/libpatchfinder" \
  -B "$ROOT/build-deps/libpatchfinder/build" \
  -DCMAKE_BUILD_TYPE=Release -DARCH=arm64 -DNO_PKGCFG=1 -DNO_SHARED=ON \
  -DCMAKE_INSTALL_PREFIX="$ROOT/dep_root" \
  -DCMAKE_CXX_FLAGS="-I/opt/homebrew/opt/openssl@3/include -I/opt/homebrew/include"
cmake --build "$ROOT/build-deps/libpatchfinder/build" --target patchfinder -j4
cp "$ROOT/build-deps/libpatchfinder/build/libpatchfinder.a" "$ROOT/dep_root/lib/"
cp -R "$ROOT/build-deps/libpatchfinder/include/libpatchfinder" "$ROOT/dep_root/include/"
```

Create local pkg-config metadata using the dependency's actual Git revision
count rather than the failed upstream metadata-generation helper:

```sh
python3 - <<'PY'
import os
import subprocess
from pathlib import Path

root = Path(os.environ["ROOT"])
version = subprocess.check_output(
    ["git", "-C", str(root / "build-deps/libpatchfinder"),
     "rev-list", "--count", "HEAD"], text=True
).strip()
metadata = (
    f"prefix={root}/dep_root\n"
    "exec_prefix=${prefix}\nlibdir=${prefix}/lib\nincludedir=${prefix}/include\n\n"
    "Name: libpatchfinder\nDescription: Source-built ARM patchfinder library\n"
    f"Version: {version}\n"
    "Requires: libgeneral libinsn libimg3tool libimg4tool libplist-2.0 openssl\n"
    "Libs: -L${libdir} -lpatchfinder -lcompression\nCflags: -I${includedir}\n"
)
(root / "dep_root/lib/pkgconfig/libpatchfinder.pc").write_text(metadata)
PY

for dep in libipatcher libtatsu; do
  (
    cd "$ROOT/build-deps/$dep" &&
    ./autogen.sh --prefix="$ROOT/dep_root" --enable-static --disable-shared &&
    make -j4 &&
    make install
  ) || break
done
```

The **futurerestore/libtatsu** fork is required: the standard Homebrew library
lacked `savage_num` and `yonkers_num`, causing the final link to fail. The fork
was installed only into `dep_root`, without replacing the system/Homebrew copy.
The installed executable also reported img4tool 0.220 and libipatcher 0.103.

## 3. Configure, build, and install futurerestore

Provide private aliases for the package names expected by upstream CMake:

```sh
ln -s libtatsu-1.0.pc "$ROOT/dep_root/lib/pkgconfig/libtatsu.pc"
ln -s /opt/homebrew/lib/pkgconfig/libimobiledevice.pc \
  "$ROOT/dep_root/lib/pkgconfig/libimobiledevice-1.0.pc"

cmake -S "$ROOT" -B "$ROOT/cmake-build-release-arm64" \
  -DARCH=arm64 -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$ROOT/install" \
  -DPKG_CONFIG_EXECUTABLE=/opt/homebrew/bin/pkg-config \
  -DCMAKE_EXE_LINKER_FLAGS="-L$ROOT/dep_root/lib" \
  -DADDITIONAL_C_FLAGS="-I/opt/homebrew/include -I/opt/homebrew/opt/openssl@3/include" \
  -DADDITIONAL_CXX_FLAGS="-I/opt/homebrew/include -I/opt/homebrew/opt/openssl@3/include"
cmake --build "$ROOT/cmake-build-release-arm64" -j4
cmake --install "$ROOT/cmake-build-release-arm64"
ln -s "$ROOT/install/bin/futurerestore" /opt/homebrew/bin/futurerestore
```

The final symlink already exists on this machine; do not recreate it unless
needed. The plain `ln -s` intentionally refuses to overwrite an existing command.
No `sudo` or changes to shell startup files were needed.

## 4. Verify before cleanup

```sh
command -v futurerestore
futurerestore --help
file "$ROOT/install/bin/futurerestore"
otool -L "$ROOT/install/bin/futurerestore"
```

Observed results:

- Command resolved to `/opt/homebrew/bin/futurerestore`.
- Help reported `v2.0.0(a15bfde42a20d6d2cd0f5b9de4e73f929cd0cc5f-348)`.
- Binary format was Mach-O 64-bit `arm64`.
- Help reported both 32-bit and 64-bit PWNDFU support enabled.
- Dynamic dependencies were Homebrew and macOS libraries, not `build-deps`,
  `dep_root`, or the CMake build directory.

This verifies command startup and linkage, not device restore compatibility.

## 5. Cleanup performed

After checking runtime linkage, the following build-only contents were removed:

- `build-deps/`: dependency checkouts, object files, and intermediate builds.
- `cmake-build-release-arm64/`: CMake cache and compiled build products.
- `dep_root/bin/`: intermediate img3tool/img4tool commands.
- Generated files in `dep_root/include/` and `dep_root/lib/`, preserving `.keep`.

The cleanup used explicitly scoped paths and did not follow symlinks:

```sh
python3 - <<'PY'
from pathlib import Path
import shutil

root = Path("/Users/Apple/futurerestore")
for name in ("build-deps", "cmake-build-release-arm64", "dep_root/bin"):
    target = root / name
    if target.is_symlink():
        target.unlink()
    elif target.exists():
        shutil.rmtree(target)

for name in ("dep_root/include", "dep_root/lib"):
    for target in (root / name).iterdir():
        if target.name == ".keep":
            continue
        if target.is_symlink() or target.is_file():
            target.unlink()
        elif target.is_dir():
            shutil.rmtree(target)
PY

futurerestore --help
du -sh /Users/Apple/futurerestore
git -C /Users/Apple/futurerestore --no-pager status --short
```

Help worked again after cleanup. The checkout shrank from approximately 129 MB
to 60 MB, reclaiming about 69 MB. The source and submodules, tracked `.keep`
files, `install/`, Homebrew libraries, and PATH symlink were retained.
**Do not delete `install/` or the source directory while using this symlink.**

Rebuilding now requires recreating the cleaned dependencies and build tree.
See also the upstream [compilation guide](COMPILING.md).

## 6. Persistent PR notification watcher

The local macOS watcher for upstream PR #189 is saved under `tools/pr-watch/`:

- `watch.py`: polls GitHub through the authenticated `gh` CLI and sends macOS
  notifications for PR status, merge status, comment, review, and CI changes.
- `dev.local.futurerestore-pr-189.plist`: per-user LaunchAgent configured to run
  once at load and every 60 seconds in a logged-in graphical session.
- `.gitignore`: excludes runtime state, locks, logs, and Python bytecode.

These files are a snapshot of this machine's installed configuration, not a
portable installer. They target `futurerestore/futurerestore#189`, use
`/opt/homebrew/bin/gh` and `/opt/homebrew/bin/python3`, and reference
`/Users/Apple`. Adapt those settings before installing elsewhere.

The live script is installed at
`/Users/Apple/Library/Application Support/FuturerestorePRWatch/watch.py`; the
active plist is in `/Users/Apple/Library/LaunchAgents/`. Repository copies do
not replace the live files automatically. Do not load both copies.

The watcher stores only comment fingerprints, not comment bodies or credentials.
It saves a baseline on its first successful check, suppresses duplicate alerts,
and retries failed API/notification calls at the next interval. Notifications
depend on macOS permissions and Focus settings; polling requires network access
and an awake Mac. Existing GitHub CLI authentication must remain available.
State files, logs, lock files, and the installed futurerestore binary are not
included in the commit.

Validate the saved configuration without querying GitHub or sending alerts:

```sh
python3 /Users/Apple/futurerestore/tools/pr-watch/watch.py --self-test
plutil -lint /Users/Apple/futurerestore/tools/pr-watch/dev.local.futurerestore-pr-189.plist
```

Inspect or stop the live job:

```sh
launchctl print "gui/$(id -u)/dev.local.futurerestore-pr-189"
launchctl bootout "gui/$(id -u)/dev.local.futurerestore-pr-189"
```

`bootout` stops the job for the current session; the installed LaunchAgent file
will load again at the next login unless it is removed or disabled. None of
these watcher operations posts comments or modifies the PR.
