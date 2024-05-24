//
//  futurerestore.cpp
//  futurerestore
//
//  Created by tihmstar on 14.09.16.
//  Copyright © 2016 tihmstar. All rights reserved.
//

#include <libgeneral/macros.h>
#include <libgen.h>
#include <zlib.h>
#include <utility>
#include <fstream>
#include "futurerestore.hpp"

#ifdef HAVE_LIBIPATCHER
#include <libipatcher/libipatcher.hpp>
#endif

#include <img4tool/img4tool.hpp>

extern "C" {
#include "common.h"
#include "normal.h"
#include "recovery.h"
#include "dfu.h"
#include "ipsw.h"
#include "locking.h"
#include "restore.h"
#include "endianness.h"
#include "tsschecker.h"
#include "download.h"
#include <zip.h>
}

#ifdef safe_mkdir
#undef safe_mkdir
#endif
#ifdef WIN32
#include <windows.h>
#ifdef mkdir
#undef mkdir
#endif
#define safe_mkdir(path, mode) mkdir(path)
#else
void safe_mkdir(const char *path, int mode) {
    int newID = 1000;
#ifdef __APPLE__
    newID = 501;
#else
    std::ifstream osReleaseStream(std::string("/etc/os-release"), std::ios::in | std::ios::binary);
    std::string osRelease;
    if(osReleaseStream.good()) {
        osRelease.reserve(futurerestore::getFileSize(std::string("/etc/os-release")));
        osRelease.assign((std::istreambuf_iterator<char>(osReleaseStream)), std::istreambuf_iterator<char>());
        if ((osReleaseStream.good())) {
            int pos = osRelease.find(std::string("\nID="));
            if(pos != std::string::npos) {
                osRelease.erase(0, pos + 4);
                pos = osRelease.find('\n');
                osRelease.erase(pos, osRelease.length());
                if(std::equal(osRelease.begin(), osRelease.end(), std::string("ubuntu").end())) {
                    if (getuid() == 999) {
                        newID = 999;
                    }
                }
            }
        }
    }
#endif
    int id = (int)getuid();
    int id1 = (int)getgid();
    int id2 = (int)geteuid();
    int id3 = (int)getegid();
    if(newID > -1) {
        setuid(newID);
        setgid(newID);
        seteuid(newID);
        setegid(newID);
    }
    __mkdir(path, mode);
    if(newID > -1) {
        setuid(id);
        setgid(id1);
        seteuid(id2);
        setegid(id3);
    }
}
#endif

#define USEC_PER_SEC 1000000

#ifdef WIN32
std::string futurerestoreTempPath("download");
#else
std::string tempPath("/tmp");
std::string futurerestoreTempPath(tempPath + "/futurerestore");
#endif

std::string roseTempPath = futurerestoreTempPath + "/rose.bin";
std::string seTempPath = futurerestoreTempPath + "/se.sefw";
std::string veridianDGMTempPath = futurerestoreTempPath + "/veridianDGM.der";
std::string veridianFWMTempPath = futurerestoreTempPath + "/veridianFWM.plist";
std::string timerTempPath = futurerestoreTempPath + "/timer.bin";
std::string baobabTempPath = futurerestoreTempPath + "/baobab.bin";
std::string cryptex1SysOSTempPath = futurerestoreTempPath + "/cryptex1SysOS.dmg";
std::string cryptex1SysVOLTempPath = futurerestoreTempPath + "/cryptex1SysVOL.dmg.root_hash";
std::string cryptex1SysTCTempPath = futurerestoreTempPath + "/cryptex1SysTC.dmg.trustcache";
std::string cryptex1AppOSTempPath = futurerestoreTempPath + "/cryptex1AppOS.dmg";
std::string cryptex1AppVOLTempPath = futurerestoreTempPath + "/cryptex1AppVOL.dmg.root_hash";
std::string cryptex1AppTCTempPath = futurerestoreTempPath + "/cryptex1AppTC.dmg.trustcache";
std::string basebandTempPath = futurerestoreTempPath + "/baseband.bbfw";
std::string basebandManifestTempPath = futurerestoreTempPath + "/basebandManifest.plist";
std::string sepTempPath = futurerestoreTempPath + "/sep.im4p";
std::string sepManifestTempPath = futurerestoreTempPath + "/sepManifest.plist";

#ifdef __APPLE__
#include <sys/sysctl.h>
#   include <CommonCrypto/CommonDigest.h>

#   define SHA1(d, n, md) CC_SHA1(d, n, md)
#   define SHA_CTX CC_SHA1_CTX
#   define SHA1_Init(c) CC_SHA1_Init(c)
#   define SHA1_Final(md, c) CC_SHA1_Final(md, c)
#   define SHA1_Update(c, d, l) CC_SHA1_Update(c, d, l)
#   define SHA256(d, n, md) CC_SHA256(d, n, md)
#   define SHA256_CTX CC_SHA256_CTX
#   define SHA256_Init(c) CC_SHA256_Init(c)
#   define SHA256_Final(md, c) CC_SHA256_Final(md, c)
#   define SHA256_Update(c, d, l) CC_SHA256_Update(c, d, l)
#   define SHA384(d, n, md) CC_SHA384(d, n, md)
#   define SHA384_Init(c) CC_SHA384_Init(c)
#   define SHA384_Final(md, c) CC_SHA384_Final(md, c)
#   define SHA384_Update(c, d, l) CC_SHA384_Update(c, d, l)
#   define SHA512(d, n, md) CC_SHA512(d, n, md)
#   define SHA512_CTX CC_SHA512_CTX
#   define SHA512_Init(c) CC_SHA512_Init(c)
#   define SHA512_Final(md, c) CC_SHA512_Final(md, c)
#   define SHA512_Update(c, d, l) CC_SHA512_Update(c, d, l)
#else
#   include <openssl/sha.h>
extern "C" {
    unsigned char *SHA1(const unsigned char *d, size_t n, unsigned char *md);
    unsigned char *SHA256(const unsigned char *d, size_t n, unsigned char *md);
}
#endif // __APPLE__

#ifndef HAVE_LIBIPATCHER
#define _enterPwnRecoveryRequested false
#endif

using namespace tihmstar;

#pragma mark helpers
extern "C" {
void irecv_event_cb(const irecv_device_event_t *event, void *userdata);
void idevice_event_cb(const idevice_event_t *event, void *userdata);
}

#pragma mark futurerestore

futurerestore::futurerestore(bool isUpdateInstall, bool isPwnDfu, bool noIBSS, bool setNonce, bool serial,
                             bool noRestore, bool noRSEP) : _isUpdateInstall(isUpdateInstall), _isPwnDfu(isPwnDfu), _noIBSS(noIBSS),
                                               _setNonce(setNonce), _serial(serial), _noRestore(noRestore), _noRSEP(noRSEP) {
    _client = idevicerestore_client_new();
    retassure(_client != nullptr, "Could not create idevicerestore client\n");
#ifdef WIN32
    struct _stat64 st{0};
#else
    struct stat st{0};
#endif
    char *tmpdir = std::getenv("TMPDIR");
    if(tmpdir != nullptr ) {
        std::string TMPDIR(tmpdir);
#ifdef WIN32
        if(!TMPDIR.empty() && _stat64(tmpdir, &st) > -1) {
#else
        if(!TMPDIR.empty() && stat(tmpdir, &st) > -1) {
#endif
            futurerestoreTempPath = TMPDIR +"/futurerestore";
            roseTempPath = futurerestoreTempPath + "/rose.bin";
            seTempPath = futurerestoreTempPath + "/se.sefw";
            veridianDGMTempPath = futurerestoreTempPath + "/veridianDGM.der";
            veridianFWMTempPath = futurerestoreTempPath + "/veridianFWM.plist";
            timerTempPath = futurerestoreTempPath + "/timer.bin";
            baobabTempPath = futurerestoreTempPath + "/baobab.bin";
            cryptex1SysOSTempPath = futurerestoreTempPath + "/cryptex1SysOS.dmg";
            cryptex1SysVOLTempPath = futurerestoreTempPath + "/cryptex1SysVOL.dmg.root_hash";
            cryptex1SysTCTempPath = futurerestoreTempPath + "/cryptex1SysTC.dmg.trustcache";
            cryptex1AppOSTempPath = futurerestoreTempPath + "/cryptex1AppOS.dmg";
            cryptex1AppVOLTempPath = futurerestoreTempPath + "/cryptex1AppVOL.dmg.root_hash";
            cryptex1AppTCTempPath = futurerestoreTempPath + "/cryptex1AppTC.dmg.trustcache";
            basebandTempPath = futurerestoreTempPath + "/baseband.bbfw";
            basebandManifestTempPath = futurerestoreTempPath + "/basebandManifest.plist";
            sepTempPath = futurerestoreTempPath + "/sep.im4p";
            sepManifestTempPath = futurerestoreTempPath + "/sepManifest.plist";
        }
    }
#ifdef WIN32
    if (_stat64(futurerestoreTempPath.c_str(), &st) == -1) safe_mkdir(futurerestoreTempPath.c_str(), 0755);
#else
    if (stat(futurerestoreTempPath.c_str(), &st) == -1) safe_mkdir(futurerestoreTempPath.c_str(), 0755);
#endif

// TODO: implement windows CI and enable update check
#ifndef WIN32
    this->checkForUpdates();
#endif

    nocache = 1; //tsschecker nocache
    _foundnonce = -1;
    _useCustomLatest = false;
    _useCustomLatestBuildID = false;
    _useCustomLatestBeta = false;
    _customLatest = std::string("");
    _customLatestBuildID = std::string("");
}

bool futurerestore::init() {
    if (_didInit) return _didInit;
//    If device is in an invalid state, don't check if it supports img4
    if ((_didInit = check_mode(_client) != _MODE_UNKNOWN)) {
        if (!(_client->image4supported = is_image4_supported(_client))) {
            info("[INFO] 32-bit device detected\n");
        } else {
            info("[INFO] 64-bit device detected\n");
        }
    }
    if(_client) {
        dfu_client_free(_client);
    }
    if(_client) {
        recovery_client_free(_client);
    }
    getDeviceMode(true);
#ifdef __APPLE__
    daemonManager(false);
#endif
    return _didInit;
}

uint64_t futurerestore::getDeviceEcid() const {
    retassure(_didInit, "did not init\n");
    return _client->ecid;
}

int futurerestore::getDeviceMode(bool reRequest) const {
    retassure(_didInit, "did not init\n");
    if (!reRequest && _client->mode && _client->mode->index != _MODE_UNKNOWN) {
        return _client->mode->index;
    } else {
        dfu_client_free(_client);
        recovery_client_free(_client);
        return check_mode(_client);
    }
}

void futurerestore::putDeviceIntoRecovery() {
    retassure(_didInit, "did not init\n");

#ifdef HAVE_LIBIPATCHER
    _enterPwnRecoveryRequested = _isPwnDfu;
#endif

    getDeviceMode(false);
    info("Found device in %s mode\n", _client->mode->string);
    if (_client->mode == MODE_NORMAL) {
        irecv_device_event_subscribe(&_client->irecv_e_ctx, irecv_event_cb, _client);
        idevice_event_subscribe(idevice_event_cb, _client);
        _client->idevice_e_ctx = (void *) idevice_event_cb;
#ifdef HAVE_LIBIPATCHER
        retassure(!_isPwnDfu, "isPwnDfu enabled, but device was found in normal mode\n");
#endif
        info("Entering recovery mode...\n");
        retassure(!normal_enter_recovery(_client), "Unable to place device into recovery mode from %s mode\n",
                  _client->mode->string);
    } else if (_client->mode == MODE_RECOVERY) {
        info("Device already in recovery mode\n");
    } else if (_client->mode == MODE_DFU && _isPwnDfu &&
               #ifdef HAVE_LIBIPATCHER
               true
#else
        false
#endif
            ) {
        info("requesting to get into pwnRecovery later\n");
    } else if (!_client->image4supported) {
        info("32-bit device in DFU mode found, assuming user wants to use iOS 9.x re-restore bug. Not failing here\n");
    } else {
        reterror("unsupported device mode, please put device in recovery or normal mode\n");
    }
    safeFree(_client->udid); //only needs to be freed manually when function didn't throw exception

    //these get also freed by destructor
    dfu_client_free(_client);
    recovery_client_free(_client);
}

void futurerestore::setAutoboot(bool val) const {
    retassure(_didInit, "did not init\n");

    retassure(getDeviceMode(false) == _MODE_RECOVERY, "can't set auto-boot, when device isn't in recovery mode\n");
    if (!_client->recovery) {
        retassure(!recovery_client_new(_client), "Could not connect to device in recovery mode.\n");
    }
    retassure(!recovery_set_autoboot(_client, val), "Setting auto-boot failed?!\n");
}

void futurerestore::exitRecovery() const {
    setAutoboot(true);
    recovery_send_reset(_client);
    recovery_client_free(_client);
}

plist_t futurerestore::nonceMatchesApTickets() {
    retassure(_didInit, "did not init\n");

    if (getDeviceMode(true) != _MODE_RECOVERY) {
        if (getDeviceMode(false) != _MODE_DFU || *_client->version != '9')
            reterror("Device is not in recovery mode, can't check ApNonce\n");
        else
            _rerestoreiOS9 = (info("Detected iOS 9.x 32-bit re-restore, proceeding in DFU mode\n"), true);
    }

    unsigned char *realnonce;
    int realNonceSize = 0;
    if (_rerestoreiOS9) {
        info("Skipping ApNonce check\n");
    } else {
        recovery_get_ap_nonce(_client, &realnonce, &realNonceSize);

        info("Got ApNonce from device: ");
        int i = 0;
        for (i = 0; i < realNonceSize; i++) {
            info("%02x ", ((unsigned char *) realnonce)[i]);
        }
        info("\n");
    }

    std::vector<const char *> nonces;

    if (_client->image4supported) {
        for (int i = 0; i < _im4ms.size(); i++) {
            auto nonce = img4tool::getValFromIM4M({_im4ms[i].first, _im4ms[i].second}, 'BNCH');
            if (nonce.payloadSize() == realNonceSize && memcmp(realnonce, nonce.payload(), realNonceSize) == 0)
                return _aptickets[i];
        }
    } else {
        for (int i = 0; i < _im4ms.size(); i++) {
            size_t ticketNonceSize = 0;
            const char *nonce = nullptr;
            try {
                //nonce might not exist, which we use in re-restoring iOS 9.x for 32-bit
                auto n = getNonceFromSCAB(_im4ms[i].first, _im4ms[i].second);
                ticketNonceSize = n.second;
                nonce = n.first;
            } catch (...) {}
            if (memcmp(realnonce, nonce, ticketNonceSize) == 0 &&
                ((ticketNonceSize == realNonceSize && realNonceSize + ticketNonceSize > 0) ||
                 (!ticketNonceSize && *_client->version == '9' &&
                  (getDeviceMode(false) == _MODE_DFU ||
                   (getDeviceMode(false) == _MODE_RECOVERY &&
                    !strncmp(getiBootBuild(), "iBoot-2817", strlen("iBoot-2817")))
                  )
                 )
                )
                    )
                //either nonce needs to match or using re-restore bug in iOS 9.x
                return _aptickets[i];
        }
    }

    return nullptr;
}

std::pair<const char *, size_t> futurerestore::nonceMatchesIM4Ms() {
    retassure(_didInit, "did not init\n");

    retassure(getDeviceMode(true) == _MODE_RECOVERY, "Device is not in recovery mode, can't check ApNonce\n");

    unsigned char *realnonce;
    int realNonceSize = 0;
    recovery_get_ap_nonce(_client, &realnonce, &realNonceSize);

    std::vector<const char *> nonces;

    if (_client->image4supported) {
        for (auto &_im4m: _im4ms) {
            auto nonce = img4tool::getValFromIM4M({_im4m.first, _im4m.second}, 'BNCH');
            if (nonce.payloadSize() == realNonceSize && memcmp(realnonce, nonce.payload(), realNonceSize) == 0)
                return _im4m;
        }
    } else {
        for (auto &_im4m: _im4ms) {
            size_t ticketNonceSize = 0;
            const char *nonce = nullptr;
            try {
                //nonce might not exist, which we use in re-restoring iOS 9.x for 32-bit
                auto n = getNonceFromSCAB(_im4m.first, _im4m.second);
                ticketNonceSize = n.second;
                nonce = n.first;
            } catch (...) {
                //
            }
            if (memcmp(realnonce, nonce, ticketNonceSize) == 0) return _im4m;
        }
    }

    return {NULL, 0};
}

void futurerestore::waitForNonce(std::vector<const char *> nonces, size_t nonceSize) {
    retassure(_didInit, "did not init\n");
    setAutoboot(false);

    unsigned char *realnonce;
    int realNonceSize = 0;

    for (auto nonce: nonces) {
        info("waiting for ApNonce: ");
        int i = 0;
        for (i = 0; i < nonceSize; i++) {
            info("%02x ", ((unsigned char *) nonce)[i]);
        }
        info("\n");
    }

    do {
        if (realNonceSize) {
            recovery_send_reset(_client);
            recovery_client_free(_client);
            usleep(1 * USEC_PER_SEC);
        }
        while (getDeviceMode(true) != _MODE_RECOVERY) usleep(USEC_PER_SEC * 0.5);
        retassure(!recovery_client_new(_client), "Could not connect to device in recovery mode\n");

        recovery_get_ap_nonce(_client, &realnonce, &realNonceSize);
        info("Got ApNonce from device: ");
        for (int i = 0; i < realNonceSize; i++) {
            info("%02x ", realnonce[i]);
        }
        info("\n");
        for (int i = 0; i < nonces.size(); i++) {
            if (memcmp(realnonce, (unsigned const char *) nonces[i], realNonceSize) == 0) _foundnonce = i;
        }
    } while (_foundnonce == -1);
    info("Device has requested ApNonce now\n");

    setAutoboot(true);
}

void futurerestore::waitForNonce() {
    retassure(!_im4ms.empty(), "No IM4M loaded\n");

    size_t nonceSize = 0;
    std::vector<const char *> nonces;

    retassure(_client->image4supported, "Error: ApNonce collision function is not supported on 32-bit devices\n");

    for (auto im4m: _im4ms) {
        auto nonce = img4tool::getValFromIM4M({im4m.first, im4m.second}, 'BNCH');
        if (!nonceSize) {
            nonceSize = nonce.payloadSize();
        }
        retassure(nonceSize == nonce.payloadSize(), "Nonces have different lengths!");
        nonces.push_back((const char *) nonce.payload());
    }

    waitForNonce(nonces, nonceSize);
}

void futurerestore::loadAPTickets(const std::vector<const char *> &apticketPaths) {
    for (auto apticketPath: apticketPaths) {
        plist_t apticket = nullptr;
        char *im4m = nullptr;
#ifdef WIN32
        struct _stat64 fst{};
#else
        struct stat fst{};
#endif
#ifdef WIN32
        retassure(!_stat64(apticketPath, &fst), "failed to load APTicket at %s\n", apticketPath);
#else
        retassure(!stat(apticketPath, &fst), "failed to load APTicket at %s\n", apticketPath);
#endif

        gzFile zf = gzopen(apticketPath, "rb");
        if (zf) {
            int blen = 0;
            int readsize = 16384; //0x4000
            int bufsize = readsize;
            std::allocator<uint8_t> alloc;
            char *bin = (char *)alloc.allocate(bufsize);
            char *p = bin;
            do {
                int bytes_read = gzread(zf, p, readsize);
                retassure(bytes_read > 0, "Error reading gz compressed data\n");
                blen += bytes_read;
                if (bytes_read < readsize) {
                    if (gzeof(zf)) {
                        bufsize += bytes_read;
                        break;
                    }
                }
                bufsize += readsize;
                bin = (char *) realloc(bin, bufsize);
                p = bin + blen;
            } while (!gzeof(zf));
            gzclose(zf);
            if (blen > 0) {
                if (memcmp(bin, "bplist00", 8) == 0)
                    plist_from_bin(bin, blen, &apticket);
                else
                    plist_from_xml(bin, blen, &apticket);
            }
            free(bin);
        }

        if (_isUpdateInstall) {
            if (plist_t update = plist_dict_get_item(apticket, "updateInstall")) {
                plist_t cpy = plist_copy(update);
                plist_free(update);
                plist_t gen_cpy = nullptr;
                if (plist_t gen = plist_dict_get_item(apticket, "generator")) {
                    gen_cpy = plist_copy(gen);
                    plist_free(gen);
                    plist_dict_set_item(cpy, "generator", gen_cpy);
                }
                plist_free(apticket);
                apticket = cpy;
            }
        }

        plist_t ticket = plist_dict_get_item(apticket, (_client->image4supported) ? "ApImg4Ticket" : "APTicket");
        uint64_t im4msize = 0;
        plist_get_data_val(ticket, &im4m, &im4msize);

        retassure(im4msize, "Error: failed to load signing ticket file %s\n", apticketPath);

        _im4ms.emplace_back(im4m, im4msize);
        _aptickets.push_back(apticket);
        printf("reading signing ticket %s is done\n", apticketPath);
    }
}

uint64_t futurerestore::getBasebandGoldCertIDFromDevice() const {
    if (!_client->preflight_info) {
        if (normal_get_preflight_info(_client, &_client->preflight_info) == -1) {
            printf("[WARNING] failed to read BasebandGoldCertID from device! Is it already in recovery?\n");
            return 0;
        }
    }
    plist_t node;
    node = plist_dict_get_item(_client->preflight_info, "CertID");
    if (!node || plist_get_node_type(node) != PLIST_UINT) {
        debug("Unable to find required BbGoldCertId in parameters\n");
        return 0;
    }
    uint64_t val = 0;
    plist_get_uint_val(node, &val);
    return val;
}

char *futurerestore::getiBootBuild() {
    if (!_ibootBuild) {
        if (_client->recovery == nullptr) {
            retassure(!recovery_client_new(_client), "Error: can't create new recovery client");
        }
        irecv_getenv(_client->recovery->client, "build-version", &_ibootBuild);
        retassure(_ibootBuild, "Error: can't get a build-version");
    }
    return _ibootBuild;
}

std::pair<ptr_smart<char *>, size_t>
getIPSWComponent(struct idevicerestore_client_t *client, plist_t build_identity, const std::string &component) {
    ptr_smart<char *> path;
    unsigned char *component_data = nullptr;
    unsigned int component_size = 0;

    if (!(char *) path) {
        retassure(!build_identity_get_component_path(build_identity, component.c_str(), &path),
                  "ERROR: Unable to get path for component '%s'\n", component.c_str());
    }

    retassure(!extract_component(client->ipsw, (char *) path, &component_data, &component_size),
              "ERROR: Unable to extract component: %s\n", component.c_str());

    return {(char *) component_data, component_size};
}

void futurerestore::enterPwnRecovery(plist_t build_identity, std::string bootargs) {
#ifndef HAVE_LIBIPATCHER
    reterror("compiled without libipatcher");
#else
    idevicerestore_mode_t *mode = nullptr;
    libipatcher::fw_key iBSSKeys{};
    libipatcher::fw_key iBECKeys{};
    std::pair<ptr_smart<char *>, size_t> iBSS;
    std::pair<ptr_smart<char *>, size_t> iBEC;
    FILE *ibss = nullptr;
    FILE *ibec = nullptr;
    int rv;
    bool cache1 = false;
    bool cache2 = false;
    std::string img3_end(".patched.img3");
    std::string img4_end(".patched.img4");
    std::string ibss_name(futurerestoreTempPath + "/ibss.");
    std::string ibec_name(futurerestoreTempPath + "/ibec.");

    /* Assure device is in dfu */
    irecv_device_event_subscribe(&_client->irecv_e_ctx, irecv_event_cb, _client);
    idevice_event_subscribe(idevice_event_cb, _client);
    _client->idevice_e_ctx = (void *) idevice_event_cb;
    getDeviceMode(true);
    mutex_lock(&_client->device_event_mutex);
    cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 1000);
    retassure(((_client->mode == MODE_DFU) || (mutex_unlock(&_client->device_event_mutex), 0)),
              "Device isn't in DFU mode!");
    retassure(((dfu_client_new(_client) == IRECV_E_SUCCESS) || (mutex_unlock(&_client->device_event_mutex), 0)),
              "Failed to connect to device in DFU Mode!");
    mutex_unlock(&_client->device_event_mutex);
    info("Device found in DFU Mode.\n");

    ibss_name.append(getDeviceBoardNoCopy());
    ibec_name.append(getDeviceBoardNoCopy());
    ibss_name.append(".");
    ibec_name.append(".");
    ibss_name.append(_client->build);
    ibec_name.append(_client->build);
    if (_client->image4supported) {
        ibss_name.append(img4_end);
        ibec_name.append(img4_end);
    } else {
        ibss_name.append(img3_end);
        ibec_name.append(img3_end);
    }
    std::allocator<uint8_t> alloc;
    if (!_noCache) {
        ibss = fopen(ibss_name.c_str(), "rb");
        if (ibss) {
            fseek(ibss, 0, SEEK_END);
            iBSS.second = ftell(ibss);
            fseek(ibss, 0, SEEK_SET);
            retassure(iBSS.first = (char *)alloc.allocate(iBSS.second), "failed to allocate memory for Rose\n");
            size_t freadRet = 0;
            retassure((freadRet = fread((char *) iBSS.first, 1, iBSS.second, ibss)) == iBSS.second,
                      "failed to load iBSS. size=%zu but fread returned %zu\n", iBSS.second, freadRet);
            fclose(ibss);
            cache1 = true;
        }
        ibec = fopen(ibec_name.c_str(), "rb");
        if (ibec) {
            fseek(ibec, 0, SEEK_END);
            iBEC.second = ftell(ibec);
            fseek(ibec, 0, SEEK_SET);
            retassure(iBEC.first = (char *)alloc.allocate(iBEC.second), "failed to allocate memory for Rose\n");
            size_t freadRet = 0;
            retassure((freadRet = fread((char *) iBEC.first, 1, iBEC.second, ibec)) == iBEC.second,
                      "failed to load iBEC. size=%zu but fread returned %zu\n", iBEC.second, freadRet);
            fclose(ibec);
            cache2 = true;
        }
    }

    /* Patch bootloaders */
    if (!cache1 && !cache2) {
        try {
            std::string board = getDeviceBoardNoCopy();
            info("Getting firmware keys for: %s\n", board.c_str());
            if (board == "n71ap" || board == "n71map" || board == "n69ap" || board == "n69uap" || board == "n66ap" ||
                board == "n66map") {
                if (!_noIBSS && !cache1) {
                    iBSSKeys = libipatcher::getFirmwareKey(_client->device->product_type, _client->build, "iBSS",
                                                           board);
                }
                if (!cache2) {
                    iBECKeys = libipatcher::getFirmwareKey(_client->device->product_type, _client->build, "iBEC",
                                                           board);
                }
            } else {
                if (!_noIBSS && !cache1) {
                    iBSSKeys = libipatcher::getFirmwareKey(_client->device->product_type, _client->build, "iBSS");
                }
                if (!cache2) {
                    iBECKeys = libipatcher::getFirmwareKey(_client->device->product_type, _client->build, "iBEC");
                }
            }
        } catch (tihmstar::exception &e) {
            reterror("getting keys failed with error: %d (%s). Are keys publicly available?", e.code(), e.what());
        }
    }

    if (!iBSS.first && !_noIBSS) {
        info("Patching iBSS\n");
        iBSS = getIPSWComponent(_client, build_identity, "iBSS");
        iBSS = std::move(libipatcher::patchiBSS((char *) iBSS.first, iBSS.second, iBSSKeys));
    }
    if (!iBEC.first) {
        info("Patching iBEC\n");
        iBEC = getIPSWComponent(_client, build_identity, "iBEC");
        iBEC = std::move(libipatcher::patchiBEC((char *) iBEC.first, iBEC.second, iBECKeys, std::move(bootargs)));
    }

    if (_client->image4supported) {
        /* if this is 64-bit, we need to back IM4P to IMG4
           also due to the nature of iBoot64Patchers sigpatches we need to stich a valid signed im4m to it (but nonce is ignored) */
        if (!cache1 && !_noIBSS) {
            info("Repacking patched iBSS as IMG4\n");
            iBSS = std::move(libipatcher::packIM4PToIMG4(iBSS.first, iBSS.second, _im4ms[0].first, _im4ms[0].second));
        }
        if (!cache2) {
            info("Repacking patched iBEC as IMG4\n");
            iBEC = std::move(libipatcher::packIM4PToIMG4(iBEC.first, iBEC.second, _im4ms[0].first, _im4ms[0].second));
        }
    }

    if (!_noIBSS) {
        retassure(ibss = fopen(ibss_name.c_str(), "wb"), "can't save patched ibss at %s\n", ibss_name.c_str());
        retassure(rv = fwrite(iBSS.first, iBSS.second, 1, ibss), "can't save patched ibss at %s\n", ibss_name.c_str());
        fflush(ibss);
        fclose(ibss);
    }
    retassure(ibec = fopen(ibec_name.c_str(), "wb"), "can't save patched ibec at %s\n", ibec_name.c_str());
    retassure(rv = fwrite(iBEC.first, iBEC.second, 1, ibec), "can't save patched ibec at %s\n", ibec_name.c_str());
    fflush(ibec);
    fclose(ibec);

    /* Send and boot bootloaders */
    irecv_error_t err = IRECV_E_UNKNOWN_ERROR;
    if (!_noIBSS) {
        /* send iBSS */
        info("Sending %s (%lu bytes)...\n", "iBSS", iBSS.second);
        mutex_lock(&_client->device_event_mutex);
        err = irecv_send_buffer(_client->dfu->client, (unsigned char *) (char *) iBSS.first,
                                (unsigned long) iBSS.second, 1);
        retassure(err == IRECV_E_SUCCESS, "ERROR: Unable to send %s component: %s\n", "iBSS", irecv_strerror(err));

        info("Booting iBSS, waiting for device to disconnect...\n");
        cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);

        retassure(((_client->mode == MODE_UNKNOWN) || (mutex_unlock(&_client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBSS. Reset device and try again");
        info("Booting iBSS, waiting for device to reconnect...\n");
    }
    bool dfu = false;
    if ((_client->device->chip_id >= 0x7000 && _client->device->chip_id <= 0x8004) ||
        (_client->device->chip_id >= 0x8900 && _client->device->chip_id <= 0x8965)) {
        cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);
        retassure(((_client->mode == MODE_DFU) || (mutex_unlock(&_client->device_event_mutex), 0)),
                  "Device did not reconnect. Possibly invalid iBSS. Reset device and try again");
        if (_client->build_major > 8) {
            mutex_unlock(&_client->device_event_mutex);
            getDeviceMode(true);
            retassure(((dfu_client_new(_client) == IRECV_E_SUCCESS) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Failed to connect to device in DFU Mode!");
            retassure(irecv_usb_set_configuration(_client->dfu->client, 1) >= 0, "ERROR: set configuration failed\n");
            /* send iBEC */
            info("Sending %s (%lu bytes)...\n", "iBEC", iBEC.second);
            mutex_lock(&_client->device_event_mutex);
            err = irecv_send_buffer(_client->dfu->client, (unsigned char *) (char *) iBEC.first,
                                    (unsigned long) iBEC.second, 1);
            retassure(err == IRECV_E_SUCCESS, "ERROR: Unable to send %s component: %s\n", "iBEC", irecv_strerror(err));

            info("Booting iBEC, waiting for device to disconnect...\n");
            cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);

#if __aarch64__
            retassure(((_client->mode == MODE_UNKNOWN) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not disconnect. Possibly invalid iBEC. If you're using a USB-C to Lightning cable, switch to USB-A to Lightning (see issue #67)");
#else
            retassure(((_client->mode == MODE_UNKNOWN) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not disconnect. Possibly invalid iBEC. Reset device and try again");
#endif
            info("Booting iBEC, waiting for device to reconnect...\n");
            cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);
#if __aarch64__
            retassure(((_client->mode == MODE_RECOVERY) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not reconnect. Possibly invalid iBEC. If you're using a USB-C to Lightning cable, switch to USB-A to Lightning (see issue #67)");
#else
            retassure(((_client->mode == MODE_RECOVERY) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not reconnect. Possibly invalid iBEC. Reset device and try again");
#endif
            mutex_unlock(&_client->device_event_mutex);
            getDeviceMode(true);
            retassure(((recovery_client_new(_client) == IRECV_E_SUCCESS) ||
                       (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Failed to connect to device in Recovery Mode!");
        }
    } else if ((_client->device->chip_id >= 0x8006 && _client->device->chip_id <= 0x8030) ||
               (_client->device->chip_id >= 0x8101 && _client->device->chip_id <= 0x8301)) {
        dfu = true;
        cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);
#if __aarch64__
        retassure(((_client->mode == MODE_RECOVERY) || (mutex_unlock(&_client->device_event_mutex), 0)),
                  "Device did not reconnect. Possibly invalid iBSS. If you're using a USB-C to Lightning cable, switch to USB-A to Lightning (see issue #67)");
#else
        retassure(((_client->mode == MODE_RECOVERY) || (mutex_unlock(&_client->device_event_mutex), 0)),
                  "Device did not reconnect. Possibly invalid iBSS. Reset device and try again");
#endif
    } else {
        mutex_unlock(&_client->device_event_mutex);
        reterror("Device not supported!\n");
    }

    /* Verify correct nonce/set nonce */
    if (_client->image4supported) {
        char *deviceGen = nullptr;
        cleanup([&] {
            safeFree(deviceGen);
        });
        mutex_unlock(&_client->device_event_mutex);
        if (_client->device->chip_id < 0x8015) {
            if (dfu) {
                assure(!irecv_send_command(_client->dfu->client, "bgcolor 255 0 0"));
            } else {
                assure(!irecv_send_command(_client->recovery->client, "bgcolor 255 0 0"));
            }
            sleep(2);
        }
        auto nonceelem = img4tool::getValFromIM4M({_im4ms[0].first, _im4ms[0].second}, 'BNCH');

        info("ApNonce pre-hax:\n");
        getDeviceMode(true);
        retassure(((recovery_client_new(_client) == IRECV_E_SUCCESS) ||
                   (mutex_unlock(&_client->device_event_mutex), 0)),
                  "Failed to connect to device in Recovery Mode!");
        if (get_ap_nonce(_client, &_client->nonce, &_client->nonce_size) < 0) {
            reterror("Failed to get apnonce from device!");
        }

        std::string generator = (_setNonce && _custom_nonce != nullptr) ? _custom_nonce : getGeneratorFromSHSH2(
                _client->tss);

        if ((_setNonce && _custom_nonce != nullptr) ||
            memcmp(_client->nonce, nonceelem.payload(), _client->nonce_size) != 0) {
            if (!_setNonce)
                info("ApNonce from device doesn't match IM4M nonce, applying hax...\n");

            assure(_client->tss);
            info("Writing generator=%s to nvram!\n", generator.c_str());

            retassure(!irecv_setenv(_client->recovery->client, "com.apple.System.boot-nonce", generator.c_str()),
                      "Failed to write generator to nvram!");
            retassure(!irecv_saveenv(_client->recovery->client), "Failed to save nvram!");

            getDeviceMode(true);
            retassure(((dfu_client_new(_client) == IRECV_E_SUCCESS) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Failed to connect to device in Recovery Mode!");
            retassure(irecv_usb_set_configuration(_client->dfu->client, 1) >= 0, "ERROR: set configuration failed\n");

            /* send iBEC */
            info("Sending %s (%lu bytes)...\n", "iBEC", iBEC.second);
            mutex_lock(&_client->device_event_mutex);
            err = irecv_send_buffer(_client->dfu->client, (unsigned char *) (char *) iBEC.first,
                                    (unsigned long) iBEC.second, 1);
            retassure(err == IRECV_E_SUCCESS, "ERROR: Unable to send %s component: %s\n", "iBEC", irecv_strerror(err));
            retassure(((irecv_send_command_breq(_client->dfu->client, "go", 1) == IRECV_E_SUCCESS) ||
                       (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not disconnect/reconnect. Possibly invalid iBEC. Reset device and try again\n");
            irecv_usb_control_transfer(_client->dfu->client, 0x21, 1, 0, 0, nullptr, 0, 5000);

            info("Booting iBEC, waiting for device to disconnect...\n");
            cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);
            retassure(((_client->mode == MODE_UNKNOWN) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not disconnect. Possibly invalid iBEC. Reset device and try again");
            info("Booting iBEC, waiting for device to reconnect...\n");
            cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);
            retassure(((_client->mode == MODE_RECOVERY) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not reconnect. Possibly invalid iBEC. Reset device and try again");
            mutex_unlock(&_client->device_event_mutex);
            getDeviceMode(true);
            retassure(((recovery_client_new(_client) == IRECV_E_SUCCESS) ||
                       (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Failed to connect to device in Recovery Mode after ApNonce hax!");
            printf("APnonce post-hax:\n");
            if (get_ap_nonce(_client, &_client->nonce, &_client->nonce_size) < 0) {
                reterror("Failed to get apnonce from device!");
            }
            assure(!irecv_send_command(_client->recovery->client, "bgcolor 255 255 0"));
            retassure(_setNonce || memcmp(_client->nonce, nonceelem.payload(), _client->nonce_size) == 0,
                      "ApNonce from device doesn't match IM4M nonce after applying ApNonce hax. Aborting!");
        } else {
            getDeviceMode(true);
            retassure(((dfu_client_new(_client) == IRECV_E_SUCCESS) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Failed to connect to device in Recovery Mode!");
            retassure(irecv_usb_set_configuration(_client->dfu->client, 1) >= 0, "ERROR: set configuration failed\n");
            /* send iBEC */
            info("Sending %s (%lu bytes)...\n", "iBEC", iBEC.second);
            mutex_lock(&_client->device_event_mutex);
            err = irecv_send_buffer(_client->dfu->client, (unsigned char *) (char *) iBEC.first,
                                    (unsigned long) iBEC.second, 1);
            retassure(err == IRECV_E_SUCCESS, "ERROR: Unable to send %s component: %s\n", "iBEC", irecv_strerror(err));
            retassure(((irecv_send_command_breq(_client->dfu->client, "go", 1) == IRECV_E_SUCCESS) ||
                       (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not disconnect/reconnect. Possibly invalid iBEC. Reset device and try again\n");
            irecv_usb_control_transfer(_client->dfu->client, 0x21, 1, 0, 0, nullptr, 0, 5000);

            info("Booting iBEC, waiting for device to disconnect...\n");
            cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);
            retassure(((MODE_UNKNOWN) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not disconnect. Possibly invalid iBEC. Reset device and try again");
            info("Booting iBEC, waiting for device to reconnect...\n");
            cond_wait_timeout(&_client->device_event_cond, &_client->device_event_mutex, 10000);
            retassure(((MODE_RECOVERY) || (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Device did not reconnect. Possibly invalid iBEC. Reset device and try again");
            mutex_unlock(&_client->device_event_mutex);
            getDeviceMode(true);
            retassure(((recovery_client_new(_client) == IRECV_E_SUCCESS) ||
                       (mutex_unlock(&_client->device_event_mutex), 0)),
                      "Failed to connect to device in Recovery Mode after ApNonce hax!");
            assure(!irecv_send_command(_client->recovery->client, "bgcolor 255 255 0"));
            info("APNonce from device already matches IM4M nonce, no need for extra hax...\n");
        }
        retassure(!irecv_setenv(_client->recovery->client, "com.apple.System.boot-nonce", generator.c_str()),
                  "failed to write generator to nvram");
        retassure(!irecv_saveenv(_client->recovery->client), "failed to save nvram");
        uint64_t gen = std::stoull(generator, nullptr, 16);
        auto *nonce = (uint8_t *)alloc.allocate(_client->nonce_size == 32 ? 48 : 20);
        if (_client->nonce_size == 20) {
            SHA1((unsigned char *) &gen, 8, nonce);
        } else if (_client->nonce_size == 32) {
            SHA384((unsigned char *) &gen, 8, nonce);
        } else {
            reterror("Failed to set nonce generator: %s! Unknown nonce size: %d\n", generator.c_str(),
                     _client->nonce_size);
        }
        for (int i = 0; i < _client->nonce_size; i++) {
            if (*(uint8_t *) (nonce + i) != *(uint8_t *) (_client->nonce + i)) {
                reterror("Failed to set nonce generator: %s!\n", generator.c_str());
            }
        }
        info("Successfully set nonce generator: %s\n", generator.c_str());

        if (_setNonce) {
            info("Done setting nonce!\n");
            info("Use futurerestore --exit-recovery to go back to normal mode if you aren't restoring.\n");
            setAutoboot(false);
            recovery_send_reset(_client);
            recovery_client_free(_client);
            exit(0);
        }

        sleep(2);
    }
#endif //HAVE_LIBIPATCHER
}

void get_custom_component(struct idevicerestore_client_t *client, plist_t build_identity, const char *component,
                          unsigned char **data, unsigned int *size) {
#ifndef HAVE_LIBIPATCHER
    reterror("compiled without libipatcher");
#else
    try {
        auto comp = getIPSWComponent(client, build_identity, component);
        comp = std::move(libipatcher::decryptFile3((char *) comp.first, comp.second,
                                              libipatcher::getFirmwareKey(client->device->product_type, client->build,
                                                                          component)));
        *data = (unsigned char *) (char *) comp.first;
        *size = comp.second;
        comp.first = NULL; //don't free on destruction
    } catch (tihmstar::exception &e) {
        reterror("ERROR: libipatcher failed with reason %d (%s)\n", e.code(), e.what());
    }

#endif
}

void futurerestore::doRestore(const char *ipsw) {
    plist_t buildmanifest = nullptr;
    int delete_fs = 0;
    char *filesystem = nullptr;
    cleanup([&] {
        info("Cleaning up...\n");
        safeFreeCustom(buildmanifest, plist_free);
        if (delete_fs && filesystem) unlink(filesystem);
    });
    struct idevicerestore_client_t *client = _client;
    plist_t build_identity = nullptr;

    client->ipsw = strdup(ipsw);
    if (_noRestore) client->flags |= FLAG_NO_RESTORE;
    if (_noRSEP) client->flags |= FLAG_NO_RSEP;
    if (!_isUpdateInstall) client->flags |= FLAG_ERASE;

    if(!client->idevice_e_ctx) {
        irecv_device_event_subscribe(&client->irecv_e_ctx, irecv_event_cb, client);
        idevice_event_subscribe(idevice_event_cb, client);
        client->idevice_e_ctx = (void *) idevice_event_cb;
    }

    mutex_lock(&client->device_event_mutex);
    client->ignore_device_add_events = 0;
    unsigned int timeout = 10000;
    // 1337 epic speed hax
    if(client->mode != MODE_UNKNOWN) {
        timeout = 1;
    }
    cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, timeout);

    retassure(client->mode != MODE_UNKNOWN, "Unable to discover device mode. Please make sure a device is attached.\n");
    if (client->mode != MODE_RECOVERY) {
        retassure(client->mode == MODE_DFU, "Device is in unexpected mode detected!");
        retassure(_enterPwnRecoveryRequested, "Device is in DFU mode detected, but we were expecting recovery mode!");
    } else {
        retassure(!_enterPwnRecoveryRequested, "--use-pwndfu was specified, but device found in recovery mode!");
    }
    mutex_unlock(&client->device_event_mutex);
    info("Found device in %s mode\n", client->mode->string);

    info("Identified device as %s, %s\n", getDeviceBoardNoCopy(), getDeviceModelNoCopy());

    retassure(!access(client->ipsw, F_OK), "ERROR: Firmware file %s does not exist.\n",
              client->ipsw); // verify if ipsw file exists

    info("Extracting BuildManifest from iPSW\n");
    {
        int unused;
        retassure(!ipsw_extract_build_manifest(client->ipsw, &buildmanifest, &unused),
                  "ERROR: Unable to extract BuildManifest from %s. Firmware file might be corrupt.\n", client->ipsw);
    }
    client->build_manifest = plist_copy(buildmanifest);

    /* check if device type is supported by the given build manifest */
    retassure(!build_manifest_check_compatibility(buildmanifest, client->device->product_type),
              "ERROR: Could not make sure this firmware is suitable for the current device. Refusing to continue.\n");

    /* print iOS information from the manifest */
    build_manifest_get_version_information(buildmanifest, client);
    info("Product version: %s\n", client->version);
    info("Product build: %s Major: %d\n", client->build, client->build_major);
    client->image4supported = is_image4_supported(client);
    info("Device supports Image4: %s\n", (client->image4supported) ? "true" : "false");

    if (_enterPwnRecoveryRequested) //we are in pwnDFU, so we don't need to check nonces
        client->tss = _aptickets.at(0);
    else if (!(client->tss = nonceMatchesApTickets()) && !this->_isPwnDfu)
        reterror("Device ApNonce does not match APTicket nonce\n");

    plist_dict_remove_item(client->tss, "BBTicket");
    plist_dict_remove_item(client->tss, "BasebandFirmware");


    if (_enterPwnRecoveryRequested && _client->image4supported) {
        retassure(plist_dict_get_item(_client->tss, "generator"),
                  "signing ticket file does not contain generator. But a generator is required for 64-bit pwnDFU restore");
    }

    retassure(build_identity = getBuildidentityWithBoardconfig(buildmanifest, client->device->hardware_model,
                                                               _isUpdateInstall),
              "ERROR: Unable to find any build identities for iPSW\n");

    if (_client->image4supported && !_setNonce) {
        if (!(client->sepBuildIdentity = getBuildidentityWithBoardconfig(_sepbuildmanifest,
                                                                         client->device->hardware_model,
                                                                         _isUpdateInstall))) {
            retassure(_isPwnDfu, "ERROR: Unable to find any build identities for SEP\n");
            warning("can't find buildidentity for SEP with InstallType=%s. However pwnDFU was requested, so trying fallback to %s",
                    (_isUpdateInstall ? "UPDATE" : "ERASE"), (!_isUpdateInstall ? "UPDATE" : "ERASE"));
            retassure((client->sepBuildIdentity = getBuildidentityWithBoardconfig(_sepbuildmanifest,
                                                                                  client->device->hardware_model,
                                                                                  !_isUpdateInstall)),
                      "ERROR: Unable to find any build identities for SEP\n");
        }
    }

    plist_t manifest = plist_dict_get_item(build_identity, "Manifest"); //this is the buildidentity used for restore

    printf("checking if the APTicket is valid for this restore...\n"); //if we are in pwnDFU, just use first APTicket. We don't need to check nonces.
    auto im4m = (_enterPwnRecoveryRequested || _rerestoreiOS9) ? _im4ms.at(0) : nonceMatchesIM4Ms();

    uint64_t deviceEcid = getDeviceEcid();
    uint64_t im4mEcid = 0;
    if (_client->image4supported) {
        auto ecid = img4tool::getValFromIM4M({im4m.first, im4m.second}, 'ECID');
        im4mEcid = ecid.getIntegerValue();
    } else {
        im4mEcid = getEcidFromSCAB(im4m.first, im4m.second);
    }

    retassure(im4mEcid, "Failed to read ECID from APTicket\n");

    if (im4mEcid != deviceEcid) {
        error("ECID inside of the APTicket does not match the device's ECID\n");
        printf("APTicket is valid for %16llu (dec) but device is %16llu (dec)\n", im4mEcid, deviceEcid);
        if (_skipBlob) {
            info("[WARNING] NOT VALIDATING SHSH BLOBS ECID!\n");
        } else {
            reterror("APTicket can't be used for restoring this device\n");
        }
    } else
        printf("Verified ECID in APTicket matches the device's ECID\n");

    if (_client->image4supported) {
        printf("checking if the APTicket is valid for this restore...\n");

        if (im4mEcid != deviceEcid) {
            error("ECID inside of the APTicket does not match the device's ECID\n");
            printf("APTicket is valid for %16llu (dec) but device is %16llu (dec)\n", im4mEcid, deviceEcid);
            if (_skipBlob) {
                info("[WARNING] NOT VALIDATING SHSH BLOBS ECID!\n");
            } else {
                reterror("APTicket can't be used for restoring this device\n");
            }
        } else
            printf("Verified ECID in APTicket matches the device's ECID\n");

        plist_t ticketIdentity = nullptr;

        try {
            ticketIdentity = img4tool::getBuildIdentityForIm4m({im4m.first, im4m.second}, buildmanifest);
        } catch (tihmstar::exception &e) {
            if (_skipBlob) {
                info("[WARNING] NOT VALIDATING SHSH BLOBS IM4M!\n");
            }
            //
        }

        if (!_skipBlob && !ticketIdentity) {
            printf("Failed to get exact match for build identity, using fallback to ignore certain values\n");
            ticketIdentity = img4tool::getBuildIdentityForIm4m({im4m.first, im4m.second}, buildmanifest,
                                                               {"RestoreRamDisk", "RestoreTrustCache"});
        }

        /* TODO: make this nicer!
           for now a simple pointercompare should be fine, because both plist_t should point into the same buildidentity inside the buildmanifest */
        if (ticketIdentity != build_identity) {
            error("BuildIdentity selected for restore does not match APTicket\n\n");
            info("BuildIdentity selected for restore:\n");
            img4tool::printGeneralBuildIdentityInformation(build_identity);
            info("\nBuildIdentity is valid for the APTicket:\n");

            if (ticketIdentity) img4tool::printGeneralBuildIdentityInformation(ticketIdentity), putchar('\n');
            else {
                info("IM4M is not valid for any restore within the Buildmanifest\n");
                info("This APTicket can't be used for restoring this firmware\n");
            }
            if (_skipBlob) {
                info("[WARNING] NOT VALIDATING SHSH BLOBS!\n");
            } else {
                reterror("APTicket can't be used for this restore\n");
            }
        } else {
            if (!img4tool::isIM4MSignatureValid({im4m.first, im4m.second})) {
                info("IM4M signature is not valid!\n");
            }
            info("Verified APTicket to be valid for this restore\n");
        }
    } else if (_enterPwnRecoveryRequested) {
        info("[WARNING] skipping ramdisk hash check, since device is in pwnDFU according to user\n");

    } else {
        info("[WARNING] full buildidentity check is not implemented, only comparing ramdisk hash.\n");

        auto ticket = getRamdiskHashFromSCAB(im4m.first, im4m.second);
        const char *tickethash = ticket.first;
        size_t tickethashSize = ticket.second;

        uint64_t manifestDigestSize = 0;
        char *manifestDigest = nullptr;

        plist_t restoreRamdisk = plist_dict_get_item(manifest, "RestoreRamDisk");
        plist_t digest = plist_dict_get_item(restoreRamdisk, "Digest");

        plist_get_data_val(digest, &manifestDigest, &manifestDigestSize);

        if (tickethashSize == manifestDigestSize && memcmp(tickethash, manifestDigest, tickethashSize) == 0) {
            info("Verified APTicket to be valid for this restore\n");
            free(manifestDigest);
        } else {
            free(manifestDigest);
            printf("APTicket ramdisk hash does not match the ramdisk we are trying to boot. Are you using correct install type (Update/Erase)?\n");
            reterror("APTicket can't be used for this restore\n");
        }
    }

    if (_basebandbuildmanifest) {
        if (!(client->basebandBuildIdentity = getBuildidentityWithBoardconfig(_basebandbuildmanifest,
                                                                              client->device->hardware_model,
                                                                              _isUpdateInstall))) {
            retassure(client->basebandBuildIdentity = getBuildidentityWithBoardconfig(_basebandbuildmanifest,
                                                                                      client->device->hardware_model,
                                                                                      !_isUpdateInstall),
                      "ERROR: Unable to find any build identities for Baseband\n");
            info("[WARNING] Unable to find Baseband buildidentities for restore type %s, using fallback %s\n",
                 (_isUpdateInstall) ? "Update" : "Erase", (!_isUpdateInstall) ? "Update" : "Erase");
        }

        client->bbfwtmp = (char *)this->_basebandPath.c_str();

        plist_t bb_manifest = plist_dict_get_item(client->basebandBuildIdentity, "Manifest");
        plist_t bb_baseband = plist_copy(plist_dict_get_item(bb_manifest, "BasebandFirmware"));
        plist_dict_set_item(manifest, "BasebandFirmware", bb_baseband);

        retassure(_client->basebandBuildIdentity, "BasebandBuildIdentity not loaded, refusing to continue");
    } else {
        warning("WARNING: we don't have a BasebandBuildManifest, won't flash baseband!\n");
    }

    if (_client->image4supported && !_setNonce) {
        //check SEP
        plist_t sep_manifest = plist_dict_get_item(client->sepBuildIdentity, "Manifest");
        plist_t sep_sep = plist_copy(plist_dict_get_item(sep_manifest, "SEP"));
        plist_dict_set_item(manifest, "SEP", sep_sep);
        unsigned char genHash[48]; //SHA384 digest length
        ptr_smart<unsigned char *> sephash = NULL;
        uint64_t sephashlen = 20;
        plist_t digest = plist_dict_get_item(sep_sep, "Digest");

        retassure(digest && plist_get_node_type(digest) == PLIST_DATA, "ERROR: can't find SEP digest\n");

        plist_get_data_val(digest, reinterpret_cast<char **>(&sephash), &sephashlen);
        if (sephashlen == 20) {
            SHA1((unsigned char *) _client->sepfwdata, _client->sepfwdatasize, (unsigned char *)&genHash);
        }
        else {
            SHA384((const unsigned char *) _client->sepfwdata, (uint64_t) _client->sepfwdatasize, genHash);
        }
        retassure(!memcmp(genHash, sephash, sephashlen), "ERROR: SEP does not match sepmanifest\n");
    }

    build_identity_print_information(build_identity); // print information about current build identity

    //check for enterpwnrecovery, because we Could be in DFU mode
    if (_enterPwnRecoveryRequested) {
        retassure((getDeviceMode(true) == _MODE_DFU) || (getDeviceMode(false) == _MODE_RECOVERY && _noIBSS),
                  "unexpected device mode\n");
        if(client->irecv_e_ctx) {
            irecv_device_event_unsubscribe(client->irecv_e_ctx);
        }
        if(client->idevice_e_ctx != nullptr) {
            client->idevice_e_ctx = nullptr;
        }
        std::string bootargs;
        if (_boot_args != nullptr) {
            bootargs = _boot_args;
        } else {
            if (_serial) {
                bootargs.append("serial=0x3 ");
            }
            bootargs.append("rd=md0 ");
            if (!_isUpdateInstall) {
                bootargs.append("nand-enable-reformat=0x1 ");
            }
            bootargs.append(
                    "-v -restore debug=0x2014e keepsyms=0x1 amfi=0xff amfi_allow_any_signature=0x1 amfi_get_out_of_my_way=0x1 cs_enforcement_disable=0x1");
        }
        enterPwnRecovery(build_identity, bootargs);
        if(_client->irecv_e_ctx) {
            irecv_device_event_unsubscribe(_client->irecv_e_ctx);
        }
        if(_client->idevice_e_ctx != nullptr) {
            _client->idevice_e_ctx = nullptr;
        }
        irecv_device_event_subscribe(&client->irecv_e_ctx, irecv_event_cb, _client);
        idevice_event_subscribe(idevice_event_cb, client);
        client->idevice_e_ctx = (void *) idevice_event_cb;
    }

    // Get filesystem name from build identity
    char *fsname = nullptr;
    retassure(!build_identity_get_component_path(build_identity, "OS", &fsname),
              "ERROR: Unable to get path for filesystem component\n");

    // check if we already have an extracted filesystem
#ifdef WIN32
    struct _stat64 st{};
    memset(&st, '\0', sizeof(struct _stat64));
#else
    struct stat st{};
    memset(&st, '\0', sizeof(struct stat));
#endif
    char tmpf[1024];
    if (client->cache_dir) {
#ifdef WIN32
        if (_stat64(client->cache_dir, &st) < 0) {
#else
        if (stat(client->cache_dir, &st) < 0) {
#endif
            mkdir_with_parents(client->cache_dir, 0755);
        }
        strcpy(tmpf, client->cache_dir);
        strcat(tmpf, "/");
        char *ipswtmp = strdup(client->ipsw);
        strcat(tmpf, basename(ipswtmp));
        free(ipswtmp);
    } else {
        strcpy(tmpf, client->ipsw);
    }
    char *p = strrchr(tmpf, '.');
    if (p) {
        *p = '\0';
    }
#ifdef WIN32
    if (_stat64(tmpf, &st) < 0) {
#else
    if (stat(tmpf, &st) < 0) {
#endif
        safe_mkdir(tmpf, 0755);
    }
    strcat(tmpf, "/");
    strcat(tmpf, fsname);

#ifdef WIN32
    memset(&st, '\0', sizeof(struct _stat64));
    if (_stat64(tmpf, &st) == 0) {
#else
    memset(&st, '\0', sizeof(struct stat));
    if (stat(tmpf, &st) == 0) {
#endif
        off_t fssize = 0;
        ipsw_get_file_size(client->ipsw, fsname, (uint64_t *) &fssize);
        if ((fssize > 0) && (st.st_size == fssize)) {
            info("Using cached filesystem from '%s'\n", tmpf);
            filesystem = strdup(tmpf);
        }
    }

    if (!filesystem) {
        char extfn[1024];
        strcpy(extfn, tmpf);
        strcat(extfn, ".extract");
        char lockfn[1024];
        strcpy(lockfn, tmpf);
        strcat(lockfn, ".lock");
        lock_info_t li;

        lock_file(lockfn, &li);
        FILE *extf = nullptr;
        if (access(extfn, F_OK) != 0) {
            extf = fopen(extfn, "w");
        }
        unlock_file(&li);
        if (!extf) {
            // use temp filename
            filesystem = reinterpret_cast<char *>(mkstemp(const_cast<char *>("ipsw_")));
            if (!filesystem) {
                error("WARNING: Could not get temporary filename, using '%s' in current directory\n", fsname);
                filesystem = strdup(fsname);
            }
            delete_fs = 1;
        } else {
            // use <fsname>.extract as filename
            filesystem = strdup(extfn);
            fclose(extf);
        }
        remove(lockfn);

        info("Extracting filesystem from iPSW\n");
        retassure(!ipsw_extract_to_file_with_progress(client->ipsw, fsname, filesystem, 1),
                  "ERROR: Unable to extract filesystem from iPSW\n");

        // rename <fsname>.extract to <fsname>
        if (strstr(filesystem, ".extract")) {
            remove(tmpf);
            rename(filesystem, tmpf);
            free(filesystem);
            filesystem = strdup(tmpf);
        }
    }

    if (_rerestoreiOS9) {
        mutex_lock(&_client->device_event_mutex);
        if (dfu_send_component(client, build_identity, "iBSS") < 0) {
            irecv_close(client->dfu->client);
            client->dfu->client = nullptr;
            reterror("ERROR: Unable to send iBSS to device\n");
        }

        /* reconnect */
        dfu_client_free(client);

        info("Booting iBSS, Waiting for device to disconnect...\n");
        cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, 10000);
        retassure((client->mode == MODE_UNKNOWN || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBSS. Reset device and try again");
        mutex_unlock(&client->device_event_mutex);

        info("Booting iBSS, Waiting for device to reconnect...\n");
        mutex_lock(&_client->device_event_mutex);
        cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, 10000);
        retassure((client->mode == MODE_DFU || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBSS. Reset device and try again");
        mutex_unlock(&client->device_event_mutex);

        dfu_client_new(client);

        /* send iBEC */
        if (dfu_send_component(client, build_identity, "iBEC") < 0) {
            irecv_close(client->dfu->client);
            client->dfu->client = nullptr;
            reterror("ERROR: Unable to send iBEC to device\n");
        }

        dfu_client_free(client);

        info("Booting iBEC, Waiting for device to disconnect...\n");
        mutex_lock(&_client->device_event_mutex);
        cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, 10000);
        retassure((client->mode == MODE_UNKNOWN || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBEC. Reset device and try again");
        mutex_unlock(&client->device_event_mutex);

        info("Booting iBEC, Waiting for device to reconnect...\n");
        mutex_lock(&_client->device_event_mutex);
        cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, 10000);
        retassure((client->mode == MODE_RECOVERY || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not reconnect. Possibly invalid iBEC. Reset device and try again");
        mutex_unlock(&client->device_event_mutex);

    } else {
        if ((client->build_major > 8)) {
            if (!client->image4supported) {
                /* send APTicket */
                if (recovery_send_ticket(client) < 0) {
                    error("WARNING: Unable to send APTicket\n");
                }
            }
        }
    }

    if (_enterPwnRecoveryRequested) {
        if (!_client->image4supported) {
            if (strncmp(client->version, "10.", 3) ==
                0)//if pwnrecovery send all components decrypted, unless we're dealing with iOS 10
                client->recovery_custom_component_function = get_custom_component;
        }
    } else if (!_rerestoreiOS9) {

        /* now we load the iBEC */
        retassure(!recovery_send_ibec(client, build_identity), "ERROR: Unable to send iBEC\n");

        debug("waiting for device to reconnect...\n");
        recovery_client_free(client);

        debug("Waiting for device to disconnect...\n");
        mutex_unlock(&client->device_event_mutex);
        cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, 10000);
#if __aarch64__
        retassure((client->mode == MODE_UNKNOWN || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBEC. If you're using a USB-C to Lightning cable, switch to USB-A to Lightning (see issue #67)");
#else
        retassure((client->mode == MODE_UNKNOWN || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBEC. Reset device and try again");  
#endif  
        mutex_unlock(&client->device_event_mutex);

        debug("Waiting for device to reconnect...\n");
        mutex_unlock(&client->device_event_mutex);
        cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, 10000);
#if __aarch64__
        retassure((client->mode == MODE_RECOVERY || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBEC. If you're using a USB-C to Lightning cable, switch to USB-A to Lightning (see issue #67)");
#else
        retassure((client->mode == MODE_RECOVERY || (mutex_unlock(&client->device_event_mutex), 0)),
                  "Device did not disconnect. Possibly invalid iBEC. Reset device and try again");    
#endif    
        mutex_unlock(&client->device_event_mutex);
    }

    retassure(client->mode == MODE_RECOVERY, "failed to reconnect to device in recovery (iBEC) mode\n");

    //do magic
    if (_client->image4supported) get_sep_nonce(client, &client->sepnonce, &client->sepnonce_size);
    get_ap_nonce(client, &client->nonce, &client->nonce_size);

    if (client->mode == MODE_RECOVERY) {
        retassure(client->srnm, "ERROR: Could not retrieve device serial number. Can't continue.\n");

        if (client->device->chip_id < 0x8015) {
            retassure(!irecv_send_command(client->recovery->client, "bgcolor 0 255 0"),
                      "ERROR: Unable to set bgcolor\n");
            info("[WARNING] Setting bgcolor to green! If you don't see a green screen, then your device didn't boot iBEC correctly\n");
            sleep(2); //show the user a green screen!
        }

        retassure(!recovery_enter_restore(client, build_identity), "ERROR: Unable to place device into restore mode\n");

        recovery_client_free(client);
    }

    if (_client->image4supported && !_setNonce) {
        info("getting SEP ticket\n");
        retassure(!get_tss_response(client, client->sepBuildIdentity, &client->septss),
                  "ERROR: Unable to get signing tickets for SEP\n");
        retassure(_client->sepfwdatasize && _client->sepfwdata, "SEP is not loaded, refusing to continue");
    }

    mutex_lock(&client->device_event_mutex);
    debug("Waiting for device to enter restore mode...\n");
    cond_wait_timeout(&client->device_event_cond, &client->device_event_mutex, 180000);
    retassure((client->mode == MODE_RESTORE || (mutex_unlock(&client->device_event_mutex), 0)),
              "Unable to place device into restore mode");
    mutex_unlock(&client->device_event_mutex);

    info("About to restore device... \n");
    int result = restore_device(client, build_identity, filesystem);
    if (result == 2) return;
    else retassure(!(result), "ERROR: Unable to restore device\n");
}

#ifdef __APPLE__
// Borrowed from apple killall.c
int futurerestore::findProc(const char *procName, bool load) {
    struct kinfo_proc *procs = nullptr, *procs2 = nullptr;
    int mib[4];
    size_t mibLen, size = 0;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ALL;
    mib[3] = 0;
    mibLen = 3;
    int ctlRet = 0;
    do {
        ctlRet = sysctl(mib, mibLen, nullptr, &size, nullptr, 0);
        if (ctlRet < 0) {
            debug("daemonManager: findProc: failed sysctl(KERN_PROC)!\n");
            return -1;
        }
        if (!size) {
            debug("daemonManager: findProc: failed sysctl(KERN_PROC) size!\n");
            return -1;
        }
        size += size / 10;
        procs2 = static_cast<kinfo_proc *>(realloc(procs, size));
        if (!procs2) {
            debug("daemonManager: findProc: realloc failed!\n");
            safeFree(procs);
            safeFree(procs2);
            return -1;
        }
        procs = procs2;
        ctlRet = sysctl(mib, mibLen, procs, &size, nullptr, 0);
    } while(ctlRet < 0 && errno == ENOMEM);
    size_t nprocs = size / sizeof(struct kinfo_proc);
    int pid = 0;
    char *cmd;
    for(int i = 0; i < nprocs; i++) {
        if (procs[i].kp_proc.p_stat == SZOMB) {
            continue;
        }
        pid = procs[i].kp_proc.p_pid;
        char *procArgs = nullptr, *foundProc = nullptr;
        int mib2[3], argMax;
        size_t sysSize;
        mib2[0] = CTL_KERN;
        mib2[1] = KERN_ARGMAX;
        sysSize = sizeof(argMax);
        if (sysctl(mib2, 2, &argMax, &sysSize, nullptr, 0) == -1) {
            continue;
        }
        procArgs = static_cast<char *>(malloc(argMax));
        if (procArgs == nullptr) {
            continue;
        }
        mib2[0] = CTL_KERN;
        mib2[1] = KERN_PROCARGS2;
        mib2[2] = pid;
        sysSize = (size_t)argMax;
        if (sysctl(mib2, 3, procArgs, &sysSize, nullptr, 0) == -1) {
            safeFree(procArgs);
            continue;
        }
        for (foundProc = procArgs; foundProc < &procArgs[sysSize]; foundProc++) {
            if (*foundProc == '\0') {
                break;
            }
        }

        if (foundProc == &procArgs[sysSize]) {
            free(procArgs);
            continue;
        }

        for (; foundProc < &procArgs[sysSize]; foundProc++) {
            if (*foundProc != '\0') {
                break;
            }
        }

        if (foundProc == &procArgs[sysSize]) {
            free(procArgs);
            continue;
        }

        /* Strip off any path that was specified */
        for(cmd = foundProc; (foundProc < &procArgs[sysSize]) && (*foundProc != '\0'); foundProc++) {
            if (*foundProc == '/') {
                cmd = foundProc + 1;
            }
        }
        if (strcmp(cmd, procName) == 0) {
            if(!load) {
                debug("daemonManager: findProc: found %s!\n", procName);
            }
            return pid;
        }
    }
    return -1;
}

void futurerestore::daemonManager(bool load) {
    if(!load) {
        debug("daemonManager: suspending invasive macOS daemons...\n");
    }
    int pid = 0;
    const char *procList[] = { "Spotify", "MobileDeviceUpdater", "AMPDevicesAgent", "AMPDeviceDiscoveryAgent", nullptr};
    for(int i = 0; i < 3; i++) {
        pid = findProc(procList[i], load);
        if (pid > 1) {
            if (load) {
                int ret = kill(pid, SIGCONT);
            } else {
                debug("daemonManager: killing %s.\n", procList[i]);
                int ret = kill(pid, SIGSTOP);
            }
        }
    }

    if(!load) {
        debug("daemonManager: done!\n");
    }
}
#endif

futurerestore::~futurerestore() {
    recovery_client_free(_client);
    idevicerestore_client_free(_client);
    for (auto im4m: _im4ms) {
        safeFree(im4m.first);
    }
    safeFree(_ibootBuild);
    safeFree(_firmwareJson);
    safeFree(_betaFirmwareJson);
    safeFree(_otaFirmwareJson);
    safeFree(_firmwareTokens);
    safeFree(_betaFirmwareTokens);
    safeFree(_otaFirmwareTokens);
    safeFree(_latestManifest);
    safeFree(_latestFirmwareUrl);
    for (auto plist: _aptickets) {
        safeFreeCustom(plist, plist_free);
    }
    safeFreeCustom(_sepbuildmanifest, plist_free);
    safeFreeCustom(_basebandbuildmanifest, plist_free);
#ifdef __APPLE__
    daemonManager(true);
#endif
}

void futurerestore::loadFirmwareTokens() {
    if (!_firmwareTokens) {
        if (!_firmwareJson) _firmwareJson = getFirmwareJson();
        retassure(_firmwareJson, "[TSSC] Could not get firmware.json\n");
        long cnt = parseTokens(_firmwareJson, &_firmwareTokens);
        retassure(cnt > 0, "[TSSC] parsing %s.json failed\n", (0) ? "ota" : "firmware");
    }
    if(!_betaFirmwareTokens && _useCustomLatestBeta) {
        if (!_betaFirmwareJson) _betaFirmwareJson = getBetaFirmwareJson(getDeviceModelNoCopy());
        if(!_betaFirmwareJson || strcmp(_betaFirmwareJson, "[]") == 0) {
            info("[TSSC] Could not get betas json, falling back to appledb\n");
            _useAppleDB = true;
            std::string type("iOS");
            if(std::string(getDeviceModelNoCopy()).find("iPad") != std::string::npos) {
                type = std::string("iPadOS");
            }
            _betaFirmwareJson = getBetaFirmwareJson2(type.c_str(), _customLatestBuildID.c_str());
            retassure(_betaFirmwareJson, "[TSSC] Could not get betas json\n");
        }
        long cnt = parseTokens(_betaFirmwareJson, &_betaFirmwareTokens);
        retassure(cnt > 0, "[TSSC] parsing %s.json failed\n", (0) ? "beta ota" : "beta firmware");
    }
    if(!_otaFirmwareTokens && _useCustomLatestOTA) {
        if (!_otaFirmwareJson) _otaFirmwareJson = getOtaJson();
        retassure(_otaFirmwareJson, "[TSSC] Could not get otas json\n");
        long cnt = parseTokens(_otaFirmwareJson, &_otaFirmwareTokens);
        retassure(cnt > 0, "[TSSC] parsing %s.json failed\n", (0) ? "beta ota" : "beta firmware");
    }
}

const char *futurerestore::getDeviceModelNoCopy() {
    if(_model) {
        return _model;
    }
    if (!_client->device || !_client->device->product_type) {

        int mode = getDeviceMode(true);
        retassure(mode == _MODE_NORMAL || mode == _MODE_RECOVERY || mode == _MODE_DFU, "unexpected device mode=%d\n",
                  mode);

        switch (mode) {
            case _MODE_RESTORE:
                _client->device = restore_get_irecv_device(_client);
                break;
            case _MODE_NORMAL:
                _client->device = normal_get_irecv_device(_client);
                break;
            case _MODE_DFU:
            case _MODE_RECOVERY:
                _client->device = dfu_get_irecv_device(_client);
                break;
            default:
                break;
        }
    }

    return _model = _client->device->product_type;
}

const char *futurerestore::getDeviceBoardNoCopy() {
    if(_board) {
        return _board;
    }
    if (!_client->device || !_client->device->product_type) {

        int mode = getDeviceMode(true);
        retassure(mode == _MODE_NORMAL || mode == _MODE_RECOVERY || mode == _MODE_DFU, "unexpected device mode=%d\n",
                  mode);

        switch (mode) {
            case _MODE_RESTORE:
                _client->device = restore_get_irecv_device(_client);
                break;
            case _MODE_NORMAL:
                _client->device = normal_get_irecv_device(_client);
                break;
            case _MODE_DFU:
            case _MODE_RECOVERY:
                _client->device = dfu_get_irecv_device(_client);
                break;
            default:
                break;
        }
    }
    return _board = _client->device->hardware_model;
}

char *futurerestore::getLatestManifest() {
    if (!_latestManifest) {
        loadFirmwareTokens();

        const char *device = getDeviceModelNoCopy();
        t_iosVersion versVals;
        memset(&versVals, 0, sizeof(versVals));

        int versionCnt = 0;
        int i = 0;
        char **versions = nullptr;
        if(_useCustomLatestBuildID) {
            if(_useCustomLatestOTA) {
                versions = getListOfiOSForDevice2(_otaFirmwareTokens, device, 1, &versionCnt, 1,
                                                  _useCustomLatestBeta);
            } else {
                versions = getListOfiOSForDevice2(_firmwareTokens, device, 0, &versionCnt, 1,
                                                  _useCustomLatestBeta);
            }
        } else {
            versions = getListOfiOSForDevice(_firmwareTokens, device, 0, &versionCnt, _useCustomLatestBeta);
        }
        retassure(versionCnt, "[TSSC] failed finding latest firmware version\n");
        char *bpos = nullptr;
        while ((bpos = strstr((char *) (versVals.version = strdup(versions[i++])), "[B]")) != nullptr) {
            free((char *) versVals.version);
            if (--versionCnt == 0)
                reterror("[TSSC] automatic selection of firmware Couldn't find for non-beta versions\n");
        }
        if(_useCustomLatest) {
            i = 0;
            while(i < versionCnt) {
                versVals.version = strdup(versions[i++]);
                std::string version(versVals.version);
                if(!std::equal(_customLatest.begin(), _customLatest.end(), version.begin())) {
                    free((char *) versVals.version);
                } else {
                    i = -1;
                    break;
                }
            }
            if(i != -1) {
                reterror("[TSSC] failed to find custom version for device!\n");
            }
        } else if(!_useCustomLatestBeta && _useCustomLatestBuildID) {
            i = 0;
            while (i < versionCnt) {
                versVals.buildID = strdup(versions[i++]);
                std::string version(versVals.buildID);
                if (!std::equal(_customLatestBuildID.begin(), _customLatestBuildID.end(), version.begin())) {
                    free((char *) versVals.buildID);
                } else {
                    i = -1;
                    break;
                }
            }
            if(i != -1) {
                reterror("[TSSC] failed to find custom buildid for device!\n");
            }
        }
        if(!_useCustomLatestBeta) {
            if(_useCustomLatestBuildID) {
                debug("[TSSC] selecting latest firmware version: %s\n", versVals.buildID);
            } else {
                debug("[TSSC] selecting latest firmware version: %s\n", versVals.version);
            }
        }
        if (bpos) *bpos = '\0';
        if (versions) free(versions[versionCnt - 1]), free(versions);

        ptr_smart<const char *> autofree(
                versVals.version); //make sure it gets freed after function finishes execution by either reaching end or throwing exception
        ptr_smart<const char *> autofree2(
                versVals.buildID); //make sure it gets freed after function finishes execution by either reaching end or throwing exception

        if(_useCustomLatestBeta) {
            debug("[TSSC] selecting latest firmware version: %s\n", _customLatestBuildID.c_str());
            if(_useCustomLatestOTA) {
                t_versionURL *urls = getFirmwareUrls(device, &versVals, _otaFirmwareTokens, _useCustomLatestBeta, _useCustomLatestOTA);
                while(urls && urls++->url) {
                    char *manifeststr = getBuildManifest(urls->url, device, nullptr, _customLatestBuildID.c_str(), _useCustomLatestOTA);
                    if(!manifeststr) {
                        continue;
                    }
                    plist_t manifest = nullptr;
                    plist_from_xml(manifeststr, (uint32_t) strlen(manifeststr), &manifest);
                    plist_t ident = getBuildidentityWithBoardconfig(manifest, getDeviceBoardNoCopy(), _useCustomLatestOTA);
                    if(ident) {
                        _latestFirmwareUrl = urls->url;
                        safeFree(urls->buildID);
                        safeFree(urls->version);
                        break;
                    }
                    safeFree(urls->buildID);
                    safeFree(urls->version);
                }
            } else {
                if(_useAppleDB) {
                    _latestFirmwareUrl = getBetaURLForDevice2(_betaFirmwareTokens, getDeviceModelNoCopy());
                } else {
                    _latestFirmwareUrl = getBetaURLForDevice(_betaFirmwareTokens, _customLatestBuildID.c_str());
                }
            }
            _latestManifest = getBuildManifest(_latestFirmwareUrl, device, nullptr, _customLatestBuildID.c_str(), _useCustomLatestOTA);
        } else {
            if(_useCustomLatestBuildID) {
                if(_useCustomLatestOTA) {
                    t_versionURL *urls = getFirmwareUrls(device, &versVals, _otaFirmwareTokens, _useCustomLatestBeta, _useCustomLatestOTA);
                    while(urls && urls++->url) {
                        char *manifeststr = getBuildManifest(urls->url, device, nullptr, _customLatestBuildID.c_str(), _useCustomLatestOTA);
                        if(!manifeststr) {
                            continue;
                        }
                        plist_t manifest = nullptr;
                        plist_from_xml(manifeststr, (uint32_t) strlen(manifeststr), &manifest);
                        plist_t ident = getBuildidentityWithBoardconfig(manifest, getDeviceBoardNoCopy(), _useCustomLatestOTA);
                        if(ident) {
                            _latestFirmwareUrl = urls->url;
                            safeFree(urls->buildID);
                            safeFree(urls->version);
                            break;
                        }
                        safeFree(urls->buildID);
                        safeFree(urls->version);
                    }
                } else {
                    _latestFirmwareUrl = getFirmwareUrl(device, &versVals, _firmwareTokens, _useCustomLatestBeta,
                                                        _useCustomLatestOTA);
                }
            } else {
                _latestFirmwareUrl = getFirmwareUrl(device, &versVals, _firmwareTokens, _useCustomLatestBeta, _useCustomLatestOTA);
            }
            _latestManifest = getBuildManifest(_latestFirmwareUrl, device, nullptr, versVals.buildID,
                                               _useCustomLatestOTA);
        }
        retassure(_latestFirmwareUrl, "Could not find url of latest firmware version\n");
        retassure(_latestManifest, "Could not get buildmanifest of latest firmware version\n");
    }

    return _latestManifest;
}

char *futurerestore::getLatestFirmwareUrl() {
    return getLatestManifest(), _latestFirmwareUrl;
}

void inline futurerestore::test() const {
    info("DEBUG: 1337: device: %p\n", _client->device);
    assert(_client->device);
}

void futurerestore::downloadLatestRose() {
    char *manifeststr = getLatestManifest();
    char *roseStr = (elemExists("Rap,RTKitOS", manifeststr, getDeviceBoardNoCopy(), 0) ? getPathOfElementInManifest(
            "Rap,RTKitOS", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    char otaString[1024]{};
    if (roseStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", roseStr);
            roseStr = reinterpret_cast<char *>(&otaString);
        }
        auto *digestString = getDigestOfElementInManifest("Rap,RTKitOS", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(roseTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));
        if(hash && digestString && !memcmp(digestString, hash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
            info("Using cached Rose\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Rose firmware\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), roseStr, roseTempPath.c_str()),
                      "Could not download Rose\n");
        }
        loadRose(roseTempPath);
    }
}

void futurerestore::downloadLatestSE() {
    char *manifeststr = getLatestManifest();
    char *seStr = (elemExists("SE,UpdatePayload", manifeststr, getDeviceBoardNoCopy(), 0) ? getPathOfElementInManifest(
            "SE,UpdatePayload", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    char otaString[1024]{};
    if (seStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", seStr);
            seStr = reinterpret_cast<char *>(&otaString);
        }
        // TODO: SE caching how does ProductionUpdatePayloadHash work?
        info("Downloading SE firmware\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), seStr, seTempPath.c_str()),
                  "Could not download SE\n");
        loadSE(seTempPath);
    }
}

void futurerestore::downloadLatestSavage() {
    char *manifeststr = getLatestManifest();
    char *savageB0ProdStr = (elemExists("Savage,B0-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                             ? getPathOfElementInManifest("Savage,B0-Prod-Patch", manifeststr, getDeviceBoardNoCopy(),
                                                          0) : nullptr);
    char *savageB0DevStr = (elemExists("Savage,B0-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0)
                            ? getPathOfElementInManifest("Savage,B0-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                            : nullptr);
    char *savageB2ProdStr = (elemExists("Savage,B2-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0)
                             ? getPathOfElementInManifest("Savage,B2-Prod-Patch", manifeststr, getDeviceBoardNoCopy(),
                                                          _useCustomLatestOTA) : nullptr);
    char *savageB2DevStr = (elemExists("Savage,B2-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0)
                            ? getPathOfElementInManifest("Savage,B2-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                            : nullptr);
    char *savageBAProdStr = (elemExists("Savage,BA-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0)
                             ? getPathOfElementInManifest("Savage,BA-Prod-Patch", manifeststr, getDeviceBoardNoCopy(),
                                                          _useCustomLatestOTA) : nullptr);
    char *savageBADevStr = (elemExists("Savage,BA-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0)
                            ? getPathOfElementInManifest("Savage,BA-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                            : nullptr);
    std::array<std::string, 6> savagePaths{};


    char otaString[1024]{};
    if (savageB0ProdStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", savageB0ProdStr);
            savageB0ProdStr = reinterpret_cast<char *>(&otaString);
        }
        savagePaths[0] = futurerestoreTempPath + "/savageB0PP.fw";
        auto *digestString = getDigestOfElementInManifest("Savage,B0-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(savagePaths[0], 1);
        if(hash && digestString && !memcmp(digestString, hash, 32)) {
            info("Using cached Savage,B0-Prod-Patch.\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Savage,B0-Prod-Patch\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), savageB0ProdStr, savagePaths[0].c_str()),
                      "Could not download Savage,B0-Prod-Patch\n");
        }
    }
    memset(otaString, 0, 1024);
    if (savageB0DevStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", savageB0DevStr);
            savageB0DevStr = reinterpret_cast<char *>(&otaString);
        }
        savagePaths[1] = futurerestoreTempPath + "//savageB0DP.fw";
        auto *digestString = getDigestOfElementInManifest("Savage,B0-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(savagePaths[1], 1);
        if(hash && digestString && !memcmp(digestString, hash, 32)) {
            info("Using cached Savage,B0-Dev-Patch.\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Savage,B0-Dev-Patch\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), savageB0DevStr, savagePaths[1].c_str()),
                      "Could not download Savage,B0-Dev-Patch\n");
        }
    }
    memset(otaString, 0, 1024);
    if (savageB2ProdStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", savageB2ProdStr);
            savageB2ProdStr = reinterpret_cast<char *>(&otaString);
        }
        savagePaths[2] = futurerestoreTempPath + "//savageB2PP.fw";
        auto *digestString = getDigestOfElementInManifest("Savage,B2-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(savagePaths[2], 1);
        if(hash && digestString && !memcmp(digestString, hash, 32)) {
            info("Using cached Savage,B2-Prod-Patch.\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Savage,B2-Prod-Patch\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), savageB2ProdStr, savagePaths[2].c_str()),
                      "Could not download Savage,B2-Prod-Patch\n");
        }
    }
    memset(otaString, 0, 1024);
    if (savageB2DevStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", savageB2DevStr);
            savageB2DevStr = reinterpret_cast<char *>(&otaString);
        }
        savagePaths[3] = futurerestoreTempPath + "//savageB2DP.fw";
        auto *digestString = getDigestOfElementInManifest("Savage,B2-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(savagePaths[3], 1);
        if(hash && digestString && !memcmp(digestString, hash, 32)) {
            info("Using cached Savage,B2-Dev-Patch.\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Savage,B2-Dev-Patch\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), savageB2DevStr, savagePaths[3].c_str()),
                      "Could not download Savage,B2-Dev-Patch\n");
        }
    }
    memset(otaString, 0, 1024);
    if (savageBAProdStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", savageBAProdStr);
            savageBAProdStr = reinterpret_cast<char *>(&otaString);
        }
        savagePaths[4] = futurerestoreTempPath + "//savageBAPP.fw";
        auto *digestString = getDigestOfElementInManifest("Savage,BA-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(savagePaths[4], 1);
        if(hash && digestString && !memcmp(digestString, hash, 32)) {
            info("Using cached Savage,BA-Prod-Patch.\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Savage,BA-Prod-Patch\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), savageBAProdStr, savagePaths[4].c_str()),
                      "Could not download Savage,BA-Prod-Patch\n");
        }
    }
    memset(otaString, 0, 1024);
    if (savageBADevStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", savageBADevStr);
            savageBADevStr = reinterpret_cast<char *>(&otaString);
        }
        savagePaths[5] = futurerestoreTempPath + "//savageBADP.fw";
        auto *digestString = getDigestOfElementInManifest("Savage,BA-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(savagePaths[5], 1);
        if(hash && digestString && !memcmp(digestString, hash, 32)) {
            info("Using cached Savage,BA-Dev-Patch.\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Savage,BA-Dev-Patch\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), savageBADevStr, savagePaths[5].c_str()),
                      "Could not download Savage,BA-Dev-Patch\n");
        }
    }
    if (savageB0ProdStr &&
        savageB0DevStr &&
        savageB2ProdStr &&
        savageB2DevStr &&
        savageBAProdStr &&
        savageBADevStr) {
        loadSavage(savagePaths);
    }
}

void futurerestore::downloadLatestVeridian() {
    char *manifeststr = getLatestManifest();
    char *veridianDGMStr = (elemExists("BMU,DigestMap", manifeststr, getDeviceBoardNoCopy(), 0)
                            ? getPathOfElementInManifest("BMU,DigestMap", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                            : nullptr);
    char *veridianFWMStr = (elemExists("BMU,FirmwareMap", manifeststr, getDeviceBoardNoCopy(), 0)
                            ? getPathOfElementInManifest("BMU,FirmwareMap", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                            : nullptr);
    char otaString[1024]{};
    if (veridianDGMStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", veridianDGMStr);
            veridianDGMStr = reinterpret_cast<char *>(&otaString);
        }
        auto *digestString = getDigestOfElementInManifest("BMU,DigestMap", manifeststr, getDeviceBoardNoCopy(), 0);
        unsigned char *hash = getSHA(veridianDGMTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));
        if(hash && digestString && !memcmp(digestString, hash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
            info("Using cached BMU,DigestMap(Veridian).\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Veridian DigestMap\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), veridianDGMStr, veridianDGMTempPath.c_str()),
                      "Could not download Veridian DigestMap\n");
        }
    }
    memset(otaString, 0, 1024);
    if (veridianFWMStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", veridianFWMStr);
            veridianFWMStr = reinterpret_cast<char *>(&otaString);
        }
        auto digestString = getDigestOfElementInManifest("BMU,FirmwareMap", manifeststr, getDeviceBoardNoCopy(), 0);
        auto hash = getSHA(veridianFWMTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));
        if(hash && digestString && !memcmp(digestString, hash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
            info("Using cached BMU,FirmwareMap(Veridian).\n");
            safeFree(digestString);
            safeFree(hash);
        } else {
            info("Downloading Veridian FirmwareMap\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), veridianFWMStr, veridianFWMTempPath.c_str()),
                      "Could not download Veridian FirmwareMap\n");
        }
    }
    if (veridianDGMStr && veridianFWMStr)
        loadVeridian(veridianDGMTempPath, veridianFWMTempPath);
}

void futurerestore::downloadLatestTimer() {
    char *manifeststr = getLatestManifest();
    char *timerStr = (elemExists("Timer,RestoreRTKitOS", manifeststr, getDeviceBoardNoCopy(), 0) ? getPathOfElementInManifest(
            "Timer,RestoreRTKitOS", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    char otaString[1024]{};
    if (timerStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", timerStr);
            timerStr = reinterpret_cast<char *>(&otaString);
        }
        info("Downloading Timer firmware\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), timerStr, timerTempPath.c_str()),
                  "Could not download Timer\n");
        loadTimer(timerTempPath);
    }
}

void futurerestore::downloadLatestBaobab() {
    char *manifeststr = getLatestManifest();
    char *baobabStr = (elemExists("Baobab,TCON", manifeststr, getDeviceBoardNoCopy(), 0) ? getPathOfElementInManifest(
            "Baobab,TCON", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    char otaString[1024]{};
    if (baobabStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", baobabStr);
            baobabStr = reinterpret_cast<char *>(&otaString);
        }
        info("Downloading Baobab firmware\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), baobabStr, baobabTempPath.c_str()),
                  "Could not download Baobab\n");
        loadBaobab(baobabTempPath);
    }
}

void futurerestore::downloadLatestYonkers() {
    char *manifeststr = getLatestManifest();
    char *yonkers0Str = (elemExists("Yonkers,SysTopPatch0", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                             ? getPathOfElementInManifest("Yonkers,SysTopPatch0", manifeststr, getDeviceBoardNoCopy(),
                                                          0) : nullptr);
    char *yonkers1Str = (elemExists("Yonkers,SysTopPatch1", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch1", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers2Str = (elemExists("Yonkers,SysTopPatch2", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch2", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers3Str = (elemExists("Yonkers,SysTopPatch3", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch3", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers4Str = (elemExists("Yonkers,SysTopPatch4", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch4", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers5Str = (elemExists("Yonkers,SysTopPatch5", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch5", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers6Str = (elemExists("Yonkers,SysTopPatch6", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch6", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers7Str = (elemExists("Yonkers,SysTopPatch7", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch7", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers8Str = (elemExists("Yonkers,SysTopPatch8", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch8", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkers9Str = (elemExists("Yonkers,SysTopPatch9", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatch9", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkersAStr = (elemExists("Yonkers,SysTopPatchA", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatchA", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkersBStr = (elemExists("Yonkers,SysTopPatchB", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatchB", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkersCStr = (elemExists("Yonkers,SysTopPatchC", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatchC", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkersDStr = (elemExists("Yonkers,SysTopPatchD", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatchD", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkersEStr = (elemExists("Yonkers,SysTopPatchE", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatchE", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    char *yonkersFStr = (elemExists("Yonkers,SysTopPatchF", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA)
                         ? getPathOfElementInManifest("Yonkers,SysTopPatchF", manifeststr, getDeviceBoardNoCopy(),
                                                      0) : nullptr);
    std::array<std::string, 16> yonkersPaths{};


    char otaString[1024]{};
    if (yonkers0Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers0Str);
            yonkers0Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[0] = futurerestoreTempPath + "/yonkers0.fw";
            info("Downloading Yonkers,SysTopPatch0\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers0Str, yonkersPaths[0].c_str()),
                      "Could not download Yonkers,SysTopPatch0\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers1Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers1Str);
            yonkers1Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[1] = futurerestoreTempPath + "/yonkers1.fw";
        info("Downloading Yonkers,SysTopPatch1\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers1Str, yonkersPaths[1].c_str()),
                  "Could not download Yonkers,SysTopPatch1\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers2Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers2Str);
            yonkers2Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[2] = futurerestoreTempPath + "/yonkers2.fw";
        info("Downloading Yonkers,SysTopPatch2\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers2Str, yonkersPaths[2].c_str()),
                  "Could not download Yonkers,SysTopPatch2\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers3Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers3Str);
            yonkers3Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[3] = futurerestoreTempPath + "/yonkers3.fw";
        info("Downloading Yonkers,SysTopPatch3\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers3Str, yonkersPaths[3].c_str()),
                  "Could not download Yonkers,SysTopPatch3\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers4Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers4Str);
            yonkers4Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[4] = futurerestoreTempPath + "/yonkers4.fw";
        info("Downloading Yonkers,SysTopPatch4\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers4Str, yonkersPaths[4].c_str()),
                  "Could not download Yonkers,SysTopPatch4\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers5Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers5Str);
            yonkers5Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[5] = futurerestoreTempPath + "/yonkers5.fw";
        info("Downloading Yonkers,SysTopPatch5\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers5Str, yonkersPaths[5].c_str()),
                  "Could not download Yonkers,SysTopPatch5\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers6Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers6Str);
            yonkers6Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[6] = futurerestoreTempPath + "/yonkers6.fw";
        info("Downloading Yonkers,SysTopPatch6\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers6Str, yonkersPaths[6].c_str()),
                  "Could not download Yonkers,SysTopPatch6\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers7Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers7Str);
            yonkers7Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[7] = futurerestoreTempPath + "/yonkers7.fw";
        info("Downloading Yonkers,SysTopPatch7\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers7Str, yonkersPaths[7].c_str()),
                  "Could not download Yonkers,SysTopPatch7\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers8Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers8Str);
            yonkers8Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[8] = futurerestoreTempPath + "/yonkers8.fw";
        info("Downloading Yonkers,SysTopPatch8\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers8Str, yonkersPaths[8].c_str()),
                  "Could not download Yonkers,SysTopPatch8\n");
    }
    memset(otaString, 0, 1024);
    if (yonkers9Str) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkers9Str);
            yonkers9Str = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[9] = futurerestoreTempPath + "/yonkers9.fw";
        info("Downloading Yonkers,SysTopPatch9\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkers9Str, yonkersPaths[9].c_str()),
                  "Could not download Yonkers,SysTopPatch9\n");
    }
    memset(otaString, 0, 1024);
    if (yonkersAStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkersAStr);
            yonkersAStr = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[10] = futurerestoreTempPath + "/yonkersA.fw";
        info("Downloading Yonkers,SysTopPatchA\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkersAStr, yonkersPaths[10].c_str()),
                  "Could not download Yonkers,SysTopPatchA\n");
    }
    memset(otaString, 0, 1024);
    if (yonkersBStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkersBStr);
            yonkersBStr = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[11] = futurerestoreTempPath + "/yonkersB.fw";
        info("Downloading Yonkers,SysTopPatchB\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkersBStr, yonkersPaths[11].c_str()),
                  "Could not download Yonkers,SysTopPatchB\n");
    }
    memset(otaString, 0, 1024);
    if (yonkersCStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkersCStr);
            yonkersCStr = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[12] = futurerestoreTempPath + "/yonkersC.fw";
        info("Downloading Yonkers,SysTopPatchC\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkersCStr, yonkersPaths[12].c_str()),
                  "Could not download Yonkers,SysTopPatchC\n");
    }
    memset(otaString, 0, 1024);
    if (yonkersDStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkersDStr);
            yonkersDStr = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[13] = futurerestoreTempPath + "/yonkersD.fw";
        info("Downloading Yonkers,SysTopPatchD\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkersDStr, yonkersPaths[13].c_str()),
                  "Could not download Yonkers,SysTopPatchD\n");
    }
    memset(otaString, 0, 1024);
    if (yonkersEStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkersEStr);
            yonkersEStr = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[14] = futurerestoreTempPath + "/yonkersE.fw";
        info("Downloading Yonkers,SysTopPatchE\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkersEStr, yonkersPaths[14].c_str()),
                  "Could not download Yonkers,SysTopPatchE\n");
    }
    memset(otaString, 0, 1024);
    if (yonkersFStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", yonkersFStr);
            yonkersFStr = reinterpret_cast<char *>(&otaString);
        }
        yonkersPaths[15] = futurerestoreTempPath + "/yonkersF.fw";
        info("Downloading Yonkers,SysTopPatchF\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), yonkersFStr, yonkersPaths[15].c_str()),
                  "Could not download Yonkers,SysTopPatchF\n");
    }
    if (yonkers0Str &&
            yonkers0Str &&
            yonkers1Str &&
            yonkers2Str &&
            yonkers3Str &&
            yonkers4Str &&
            yonkers5Str &&
            yonkers6Str &&
            yonkers7Str &&
            yonkers8Str &&
            yonkers9Str &&
            yonkersAStr &&
            yonkersBStr &&
            yonkersCStr &&
            yonkersDStr &&
            yonkersEStr &&
            yonkersFStr) {
        loadYonkers(yonkersPaths);
    }
}

void futurerestore::downloadLatestCryptex1() {
    char *manifeststr = getLatestManifest();
    char *cryptex1SysOSStr = (elemExists("Cryptex1,SystemOS", manifeststr, getDeviceBoardNoCopy(), 0)
                              ? getPathOfElementInManifest(
                    "Cryptex1,SystemOS", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    auto *cryptex1SysOSDGSTStr = getDigestOfElementInManifest("Cryptex1,SystemOS", manifeststr, getDeviceBoardNoCopy(),
                                                              _useCustomLatestOTA);
    info("Checking for cached Cryptex1...\n");
    auto *cryptex1SysOSHash = getSHA(cryptex1SysOSTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));

    char *cryptex1SysVOLStr = (elemExists("Cryptex1,SystemVolume", manifeststr, getDeviceBoardNoCopy(), 0)
                               ? getPathOfElementInManifest(
                    "Cryptex1,SystemVolume", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    auto *cryptex1SysVOLDGSTStr = getDigestOfElementInManifest("Cryptex1,SystemVolume", manifeststr,
                                                               getDeviceBoardNoCopy(), _useCustomLatestOTA);
    auto *cryptex1SysVOLHash = getSHA(cryptex1SysVOLTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));

    char *cryptex1SysTCStr = (elemExists("Cryptex1,SystemTrustCache", manifeststr, getDeviceBoardNoCopy(), 0)
                              ? getPathOfElementInManifest(
                    "Cryptex1,SystemTrustCache", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    auto *cryptex1SysTCDGSTStr = getDigestOfElementInManifest("Cryptex1,SystemTrustCache", manifeststr,
                                                              getDeviceBoardNoCopy(), _useCustomLatestOTA);
    auto *cryptex1SysTCHash = getSHA(cryptex1SysTCTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));

    char *cryptex1AppOSStr = (elemExists("Cryptex1,AppOS", manifeststr, getDeviceBoardNoCopy(), 0)
                              ? getPathOfElementInManifest(
                    "Cryptex1,AppOS", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    auto *cryptex1AppOSDGSTStr = getDigestOfElementInManifest("Cryptex1,AppOS", manifeststr, getDeviceBoardNoCopy(),
                                                              _useCustomLatestOTA);
    auto *cryptex1AppOSHash = getSHA(cryptex1AppOSTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));

    char *cryptex1AppVOLStr = (elemExists("Cryptex1,AppVolume", manifeststr, getDeviceBoardNoCopy(), 0)
                               ? getPathOfElementInManifest(
                    "Cryptex1,AppVolume", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    auto *cryptex1AppVOLDGSTStr = getDigestOfElementInManifest("Cryptex1,AppVolume", manifeststr,
                                                               getDeviceBoardNoCopy(), _useCustomLatestOTA);
    auto *cryptex1AppVOLHash = getSHA(cryptex1AppVOLTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));


    char *cryptex1AppTCStr = (elemExists("Cryptex1,AppTrustCache", manifeststr, getDeviceBoardNoCopy(), 0)
                              ? getPathOfElementInManifest(
                    "Cryptex1,AppTrustCache", manifeststr, getDeviceBoardNoCopy(), _useCustomLatestOTA) : nullptr);
    auto *cryptex1AppTCDGSTStr = getDigestOfElementInManifest("Cryptex1,AppTrustCache", manifeststr,
                                                              getDeviceBoardNoCopy(), _useCustomLatestOTA);
    auto *cryptex1AppTCHash = getSHA(cryptex1AppTCTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));

    char otaString[1024]{};
    if (cryptex1SysOSHash && cryptex1SysOSDGSTStr &&
        !memcmp(cryptex1SysOSDGSTStr, cryptex1SysOSHash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
        info("Using cached Cryptex1,SystemOS.\n");
        safeFree(cryptex1SysOSDGSTStr);
        safeFree(cryptex1SysOSHash);
    } else {
        if (cryptex1SysOSStr) {
            if (_useCustomLatestOTA) {
                snprintf(otaString, 1024, "%s%s", "AssetData/boot/", cryptex1SysOSStr);
                cryptex1SysOSStr = reinterpret_cast<char *>(&otaString);
            }
            info("Downloading Cryptex1,SystemOS dmg(this may take a while)\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), cryptex1SysOSStr, cryptex1SysOSTempPath.c_str()),
                      "Could not download Cryptex1,SystemOS\n");
        }
    }
    if (cryptex1SysVOLHash && cryptex1SysVOLDGSTStr &&
        !memcmp(cryptex1SysVOLDGSTStr, cryptex1SysVOLHash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
        info("Using cached Cryptex1,SystemVolume.\n");
        safeFree(cryptex1SysVOLDGSTStr);
        safeFree(cryptex1SysVOLHash);
    } else {
        if (cryptex1SysVOLStr) {
            if (_useCustomLatestOTA) {
                snprintf(otaString, 1024, "%s%s", "AssetData/boot/", cryptex1SysVOLStr);
                cryptex1SysVOLStr = reinterpret_cast<char *>(&otaString);
            }
            info("Downloading Cryptex1,SystemVolume root_hash\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), cryptex1SysVOLStr, cryptex1SysVOLTempPath.c_str()),
                      "Could not download Cryptex1,SystemVolume\n");
        }
    }
    if (cryptex1SysTCHash && cryptex1SysTCDGSTStr &&
        !memcmp(cryptex1SysTCDGSTStr, cryptex1SysTCHash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
        info("Using cached Cryptex1,SystemTrustCache.\n");
        safeFree(cryptex1SysTCDGSTStr);
        safeFree(cryptex1SysTCHash);
    } else {
        if (cryptex1SysTCStr) {
            if (_useCustomLatestOTA) {
                snprintf(otaString, 1024, "%s%s", "AssetData/boot/", cryptex1SysTCStr);
                cryptex1SysTCStr = reinterpret_cast<char *>(&otaString);
            }
            info("Downloading Cryptex1,SystemTrustCache\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), cryptex1SysTCStr, cryptex1SysTCTempPath.c_str()),
                      "Could not download Cryptex1,SystemTrustCache\n");
        }
    }
    if (cryptex1AppOSHash && cryptex1AppOSDGSTStr &&
        !memcmp(cryptex1AppOSDGSTStr, cryptex1AppOSHash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
        info("Using cached Cryptex1,AppOS.\n");
        safeFree(cryptex1AppOSDGSTStr);
        safeFree(cryptex1AppOSHash);
    } else {
        if (cryptex1AppOSStr) {
            if (_useCustomLatestOTA) {
                snprintf(otaString, 1024, "%s%s", "AssetData/boot/", cryptex1AppOSStr);
                cryptex1AppOSStr = reinterpret_cast<char *>(&otaString);
            }
            info("Downloading Cryptex1,AppOS dmg\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), cryptex1AppOSStr, cryptex1AppOSTempPath.c_str()),
                      "Could not download Cryptex1,AppOS\n");
        }
    }
    if (cryptex1AppVOLHash && cryptex1AppVOLDGSTStr &&
        !memcmp(cryptex1AppVOLDGSTStr, cryptex1AppVOLHash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
        info("Using cached Cryptex1,AppVolume.\n");
        safeFree(cryptex1AppVOLDGSTStr);
        safeFree(cryptex1AppVOLHash);
    } else {
        if (cryptex1AppVOLStr) {
            if (_useCustomLatestOTA) {
                snprintf(otaString, 1024, "%s%s", "AssetData/boot/", cryptex1AppVOLStr);
                cryptex1AppVOLStr = reinterpret_cast<char *>(&otaString);
            }
            info("Downloading Cryptex1,AppVolume root_hash\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), cryptex1AppVOLStr, cryptex1AppVOLTempPath.c_str()),
                      "Could not download Cryptex1,AppVolume\n");
        }
    }
    if(cryptex1AppTCHash && cryptex1AppTCDGSTStr && !memcmp(cryptex1AppTCDGSTStr, cryptex1AppTCHash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
        info("Using cached Cryptex1,AppTrustCache.\n");
        safeFree(cryptex1AppTCDGSTStr);
        safeFree(cryptex1AppTCHash);
    } else {
        if (cryptex1AppTCStr) {
            if (_useCustomLatestOTA) {
                snprintf(otaString, 1024, "%s%s", "AssetData/boot/", cryptex1AppTCStr);
                cryptex1AppTCStr = reinterpret_cast<char *>(&otaString);
            }
            info("Downloading Cryptex1,AppTrustCache\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), cryptex1AppTCStr, cryptex1AppTCTempPath.c_str()),
                      "Could not download Cryptex1,AppTrustCache\n");
        }
    }
    loadCryptex1(cryptex1SysOSTempPath, cryptex1SysVOLTempPath, cryptex1SysTCTempPath, cryptex1AppOSTempPath, cryptex1AppVOLTempPath, cryptex1AppTCTempPath);
}

void futurerestore::downloadLatestFirmwareComponents() {
    info("Downloading the latest firmware components...\n");
    char *manifeststr = getLatestManifest();
    if (elemExists("Rap,RTKitOS", manifeststr, getDeviceBoardNoCopy(), 0))
        downloadLatestRose();
    if (elemExists("SE,UpdatePayload", manifeststr, getDeviceBoardNoCopy(), 0))
        downloadLatestSE();
    if (elemExists("Savage,B0-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0) &&
        elemExists("Savage,B0-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0) &&
        elemExists("Savage,B2-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0) &&
        elemExists("Savage,B2-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0) &&
        elemExists("Savage,BA-Prod-Patch", manifeststr, getDeviceBoardNoCopy(), 0) &&
        elemExists("Savage,BA-Dev-Patch", manifeststr, getDeviceBoardNoCopy(), 0)) {
        downloadLatestSavage();
    }
    if (elemExists("BMU,DigestMap", manifeststr, getDeviceBoardNoCopy(), 0) ||
        elemExists("BMU,FirmwareMap", manifeststr, getDeviceBoardNoCopy(), 0))
        downloadLatestVeridian();
    if (elemExists("Timer,RestoreRTKitOS", manifeststr, getDeviceBoardNoCopy(), 0))
        downloadLatestTimer();
    if (elemExists("Baobab,TCON", manifeststr, getDeviceBoardNoCopy(), 0))
        downloadLatestBaobab();
    if (elemExists("Yonkers,PatchEpoch", manifeststr, getDeviceBoardNoCopy(), 0))
        downloadLatestYonkers();
    if (elemExists("Cryptex1,SystemOS", manifeststr, getDeviceBoardNoCopy(), 0))
        downloadLatestCryptex1();
    info("Finished downloading the latest firmware components!\n");
}

void futurerestore::downloadLatestBaseband() {
    auto manifeststr = std::string(getLatestManifest());
    saveStringToFile(manifeststr, basebandManifestTempPath);
    auto pathStr = getPathOfElementInManifest("BasebandFirmware", manifeststr.c_str(), getDeviceBoardNoCopy(), _useCustomLatestOTA);
    auto *bbcfgDigestString = getBBCFGDigestInManifest(manifeststr.c_str(), getDeviceBoardNoCopy(), 0);
    size_t basebandSize = 0;
    char *basebandData = nullptr;
    basebandData = readBaseband(basebandTempPath, basebandData, &basebandSize);
    bool cached = false;
    if(bbcfgDigestString != nullptr && basebandData != nullptr) {
        const char *bbcfgData = extractZipFileToString(basebandData, "bbcfg.mbn", (uint32_t *)&basebandSize);
        if(bbcfgData != nullptr) {
            auto *hash = getSHABuffer((char *) bbcfgData, basebandSize, 1);
            if(hash && bbcfgDigestString && !memcmp(bbcfgDigestString, hash, 32)) {
                info("Using cached Baseband.\n");
                safeFree(bbcfgDigestString);
                safeFree(hash);
                cached = true;
            }
        }
    }
    char otaString[1024]{};
    if(!cached && pathStr) {
        if(_useCustomLatestOTA) {
            snprintf(otaString, 1024, "%s%s", "AssetData/boot/", pathStr);
            pathStr = reinterpret_cast<char *>(&otaString);
        }
        info("Downloading Baseband\n\n");
        retassure(!downloadPartialzip(getLatestFirmwareUrl(), pathStr, basebandTempPath.c_str()),
                  "Could not download baseband\n");
    }
    setBasebandPath(basebandTempPath);
    setBasebandManifestPath(basebandManifestTempPath);
    loadBaseband(this->_basebandPath);
    loadBasebandManifest(this->_basebandManifestPath);
}

void futurerestore::downloadLatestSep() {
    auto manifestString = std::string(getLatestManifest());
    saveStringToFile(manifestString, sepManifestTempPath);
    auto pathString = getPathOfElementInManifest("SEP", manifestString.c_str(), getDeviceBoardNoCopy(), _useCustomLatestOTA);
    auto *digestString = getDigestOfElementInManifest("SEP",manifestString.c_str(), getDeviceBoardNoCopy(), _useCustomLatestOTA);
    auto *hash = getSHA(sepTempPath, ((_client->device->chip_id < 0x8010) ? 3 : 0));
    if(hash && digestString && !memcmp(digestString, hash, ((_client->device->chip_id < 0x8010) ? 20 : 48))) {
        info("Using cached SEP.\n");
        safeFree(digestString);
        safeFree(hash);
    } else {
        char otaString[1024]{};
        if(pathString) {
            if(_useCustomLatestOTA) {
                snprintf(otaString, 1024, "%s%s", "AssetData/boot/", pathString);
                pathString = reinterpret_cast<char *>(&otaString);
            }
            info("Downloading SEP\n\n");
            retassure(!downloadPartialzip(getLatestFirmwareUrl(), pathString, sepTempPath.c_str()), "Could not download SEP\n");
        }
    }
    setSepPath(sepTempPath);
    setSepManifestPath(sepManifestTempPath);
    loadSep(this->_sepPath);
    loadSepManifest(this->_sepManifestPath);
}

void futurerestore::loadSepManifest(const std::string& sepManifestPath) {
    this->_sepManifestPath = sepManifestPath;
    retassure(_sepbuildmanifest = loadPlistFromFile(sepManifestPath.c_str()),
              "Failed to load SEP Manifest");
}

void futurerestore::loadBasebandManifest(const std::string& basebandManifestPath) {
    this->_basebandManifestPath = basebandManifestPath;
    retassure(_basebandbuildmanifest = loadPlistFromFile(basebandManifestPath.c_str()),
              "Failed to load Baseband Manifest");
};

void futurerestore::loadRose(const std::string& rosePath) const {
    std::ifstream roseFileStream(rosePath, std::ios::in | std::ios::binary);
    retassure(roseFileStream.good(), "%s: failed init file stream for %s!\n", __func__, rosePath.c_str());
    _client->rosefwdatasize = futurerestore::getFileSize(rosePath);
    std::allocator<uint8_t> alloc;
    retassure(_client->rosefwdata = (char *)alloc.allocate(_client->rosefwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, rosePath.c_str());
    roseFileStream.read((char *) _client->rosefwdata,
                          (std::streamsize) _client->rosefwdatasize);
    retassure(roseFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, rosePath.c_str());
    retassure(*(uint64_t *) (_client->rosefwdata) != 0,
              "%s: failed to load Rose for %s with the size %zu!\n",
              __func__, rosePath.c_str(), _client->rosefwdatasize);
}

void futurerestore::loadSE(const std::string& sePath) const {
    std::ifstream seFileStream(sePath, std::ios::in | std::ios::binary);
    retassure(seFileStream.good(), "%s: failed init file stream for %s!\n", __func__, sePath.c_str());
    _client->sefwdatasize = futurerestore::getFileSize(sePath);
    std::allocator<uint8_t> alloc;
    retassure(_client->sefwdata = (char *)alloc.allocate(_client->sefwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, sePath.c_str());
    seFileStream.read((char *) _client->sefwdata,
                        (std::streamsize) _client->sefwdatasize);
    retassure(seFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, sePath.c_str());
    retassure(*(uint64_t *) (_client->sefwdata) != 0,
              "%s: failed to load SE for %s with the size %zu!\n",
              __func__, sePath.c_str(), _client->sefwdatasize);
}

void futurerestore::loadSavage(const std::array<std::string, 6>& savagePaths) const {
    int index = 0;
    for (const auto &savagePath: savagePaths) {
        std::ifstream savageFileStream(savagePath, std::ios::in | std::ios::binary);
        retassure(savageFileStream.good(), "%s: failed init file stream for %s!\n", __func__, savagePath.c_str());
        _client->savagefwdatasize[index] = futurerestore::getFileSize(savagePath);
        std::allocator<uint8_t> alloc;
        retassure(_client->savagefwdata[index] = (char *)alloc.allocate(_client->savagefwdatasize[index]),
                  "%s: failed to allocate memory for %s\n", __func__, savagePath.c_str());
        savageFileStream.read((char *) _client->savagefwdata[index],
                              (std::streamsize) _client->savagefwdatasize[index]);
        retassure(savageFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, savagePath.c_str());
        retassure(*(uint64_t *) (_client->savagefwdata[index]) != 0,
                  "%s: failed to load Savage for %s with the size %zu!\n",
                  __func__, savagePath.c_str(), _client->savagefwdatasize[index]);
        index++;
    }
}

void futurerestore::loadVeridian(const std::string& veridianDGMPath, const std::string& veridianFWMPath) const {
    std::ifstream veridianDGMFileStream(veridianDGMPath, std::ios::in | std::ios::binary);
    std::ifstream veridianFWMFileStream(veridianFWMPath, std::ios::in | std::ios::binary);
    retassure(veridianDGMFileStream.good(), "%s: failed init file stream for %s!\n", __func__, veridianDGMPath.c_str());
    retassure(veridianFWMFileStream.good(), "%s: failed init file stream for %s!\n", __func__,
              veridianFWMPath.c_str());
    _client->veridiandgmfwdatasize = futurerestore::getFileSize(veridianDGMPath);
    std::allocator<uint8_t> alloc;
    retassure(_client->veridiandgmfwdata = (char *)alloc.allocate(_client->veridiandgmfwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, veridianDGMPath.c_str());
    veridianDGMFileStream.read((char *) _client->veridiandgmfwdata,
                               (std::streamsize) _client->veridiandgmfwdatasize);
    retassure(veridianDGMFileStream.good(), "%s: failed init read stream for %s!\n", __func__, veridianDGMPath.c_str());
    retassure(*(uint64_t *) (_client->veridiandgmfwdata) != 0,
              "%s: failed to load Veridian for %s with the size %zu!\n",
              __func__, veridianDGMPath.c_str(), _client->veridiandgmfwdatasize);
    _client->veridianfwmfwdatasize = futurerestore::getFileSize(veridianFWMPath);
    retassure(_client->veridianfwmfwdata = (char *)alloc.allocate(_client->veridianfwmfwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, veridianFWMPath.c_str());
    veridianFWMFileStream.read((char *) _client->veridianfwmfwdata,
                               (std::streamsize) _client->veridianfwmfwdatasize);
    retassure(veridianFWMFileStream.good(), "%s: failed to read file stream for %s!\n", __func__,
              veridianFWMPath.c_str());
    retassure(*(uint64_t *) (_client->veridianfwmfwdata) != 0,
              "%s: failed to load Veridian for %s with the size %zu!\n",
              __func__, veridianFWMPath.c_str(), _client->veridianfwmfwdatasize);
}

void futurerestore::loadTimer(const std::string& timerPath) const {
    std::ifstream timerFileStream(timerPath, std::ios::in | std::ios::binary);
    retassure(timerFileStream.good(), "%s: failed init file stream for %s!\n", __func__, timerPath.c_str());
    _client->timerfwdatasize = futurerestore::getFileSize(timerPath);
    _client->rtimerfwdatasize = futurerestore::getFileSize(timerPath);
    std::allocator<uint8_t> alloc;
    retassure(_client->timerfwdata = (char *)alloc.allocate(_client->timerfwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, timerPath.c_str());
    retassure(_client->rtimerfwdata = (char *)alloc.allocate(_client->rtimerfwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, timerPath.c_str());
    timerFileStream.read((char *) _client->timerfwdata,
                      (std::streamsize) _client->timerfwdatasize);
    retassure(timerFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, timerPath.c_str());
    retassure(*(uint64_t *) (_client->timerfwdata) != 0,
              "%s: failed to load Timer for %s with the size %zu!\n",
              __func__, timerPath.c_str(), _client->timerfwdatasize);
    timerFileStream.read((char *) _client->rtimerfwdata,
                      (std::streamsize) _client->rtimerfwdatasize);
    retassure(timerFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, timerPath.c_str());
    retassure(*(uint64_t *) (_client->rtimerfwdata) != 0,
              "%s: failed to load Timer for %s with the size %zu!\n",
              __func__, timerPath.c_str(), _client->rtimerfwdatasize);
}

void futurerestore::loadBaobab(const std::string& baobabPath) const {
    std::ifstream baobobFileStream(baobabPath, std::ios::in | std::ios::binary);
    retassure(baobobFileStream.good(), "%s: failed init file stream for %s!\n", __func__, baobabPath.c_str());
    _client->baobabfwdatasize = futurerestore::getFileSize(baobabPath);
    std::allocator<uint8_t> alloc;
    retassure(_client->baobabfwdata = (char *)alloc.allocate(_client->baobabfwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, baobabPath.c_str());
    baobobFileStream.read((char *) _client->baobabfwdata,
                      (std::streamsize) _client->baobabfwdatasize);
    retassure(baobobFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, baobabPath.c_str());
    retassure(*(uint64_t *) (_client->baobabfwdata) != 0,
              "%s: failed to load Baobab for %s with the size %zu!\n",
              __func__, baobabPath.c_str(), _client->baobabfwdatasize);
}

void futurerestore::loadCryptex1(const std::string& cryptex1SysOSPath, const std::string& cryptex1SysVOLPath, const std::string& cryptex1SysTCPath, const std::string& cryptex1AppOSPath, const std::string& cryptex1AppVOLPath, const std::string& cryptex1AppTCPath) const {
    std::ifstream cryptex1SysOSFileStream(cryptex1SysOSPath, std::ios::in | std::ios::binary);
    retassure(cryptex1SysOSFileStream.good(), "%s: failed init file stream for %s!\n", __func__, cryptex1SysOSPath.c_str());
    _client->cryptex1sysosdatasize = futurerestore::getFileSize(cryptex1SysOSPath);
    std::allocator<uint8_t> alloc;

    retassure(_client->cryptex1sysosdata = (char *)alloc.allocate(_client->cryptex1sysosdatasize),
              "%s: failed to allocate memory for %s\n", __func__, cryptex1SysOSPath.c_str());
    cryptex1SysOSFileStream.read((char *) _client->cryptex1sysosdata,
                      (std::streamsize) _client->cryptex1sysosdatasize);
    retassure(cryptex1SysOSFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, cryptex1SysOSPath.c_str());
    retassure(*(uint64_t *) (_client->cryptex1sysosdata) != 0,
              "%s: failed to load Cryptex1,SystemOS for %s with the size %zu!\n",
              __func__, cryptex1SysOSPath.c_str(), _client->cryptex1sysosdatasize);

    std::ifstream cryptex1SysVOLFileStream(cryptex1SysVOLPath, std::ios::in | std::ios::binary);
    retassure(cryptex1SysVOLFileStream.good(), "%s: failed init file stream for %s!\n", __func__, cryptex1SysVOLPath.c_str());
    _client->cryptex1sysvoldatasize = futurerestore::getFileSize(cryptex1SysVOLPath);
    retassure(_client->cryptex1sysvoldata = (char *)alloc.allocate(_client->cryptex1sysvoldatasize),
              "%s: failed to allocate memory for %s\n", __func__, cryptex1SysVOLPath.c_str());
    cryptex1SysVOLFileStream.read((char *) _client->cryptex1sysvoldata,
                      (std::streamsize) _client->cryptex1sysvoldatasize);
    retassure(cryptex1SysVOLFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, cryptex1SysVOLPath.c_str());
    retassure(*(uint64_t *) (_client->cryptex1sysvoldata) != 0,
              "%s: failed to load Cryptex1,SystemVolume for %s with the size %zu!\n",
              __func__, cryptex1SysVOLPath.c_str(), _client->cryptex1sysvoldatasize);

    std::ifstream cryptex1SysTCFileStream(cryptex1SysTCPath, std::ios::in | std::ios::binary);
    retassure(cryptex1SysTCFileStream.good(), "%s: failed init file stream for %s!\n", __func__, cryptex1SysTCPath.c_str());
    _client->cryptex1systcdatasize = futurerestore::getFileSize(cryptex1SysTCPath);
    retassure(_client->cryptex1systcdata = (char *)alloc.allocate(_client->cryptex1systcdatasize),
              "%s: failed to allocate memory for %s\n", __func__, cryptex1SysTCPath.c_str());
    cryptex1SysTCFileStream.read((char *) _client->cryptex1systcdata,
                                 (std::streamsize) _client->cryptex1systcdatasize);
    retassure(cryptex1SysTCFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, cryptex1SysTCPath.c_str());
    retassure(*(uint64_t *) (_client->cryptex1systcdata) != 0,
              "%s: failed to load Cryptex1,SystemTrustCache for %s with the size %zu!\n",
              __func__, cryptex1SysTCPath.c_str(), _client->cryptex1systcdatasize);

    std::ifstream cryptex1AppOSFileStream(cryptex1AppOSPath, std::ios::in | std::ios::binary);
    retassure(cryptex1AppOSFileStream.good(), "%s: failed init file stream for %s!\n", __func__, cryptex1AppOSPath.c_str());
    _client->cryptex1apposdatasize = futurerestore::getFileSize(cryptex1AppOSPath);
    retassure(_client->cryptex1apposdata = (char *)alloc.allocate(_client->cryptex1apposdatasize),
              "%s: failed to allocate memory for %s\n", __func__, cryptex1AppOSPath.c_str());
    cryptex1AppOSFileStream.read((char *) _client->cryptex1apposdata,
                                 (std::streamsize) _client->cryptex1apposdatasize);
    retassure(cryptex1AppOSFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, cryptex1AppOSPath.c_str());
    retassure(*(uint64_t *) (_client->cryptex1apposdata) != 0,
              "%s: failed to load Cryptex1,AppOS for %s with the size %zu!\n",
              __func__, cryptex1AppOSPath.c_str(), _client->cryptex1apposdatasize);

    std::ifstream cryptex1AppVOLFileStream(cryptex1AppVOLPath, std::ios::in | std::ios::binary);
    retassure(cryptex1AppVOLFileStream.good(), "%s: failed init file stream for %s!\n", __func__, cryptex1AppVOLPath.c_str());
    _client->cryptex1appvoldatasize = futurerestore::getFileSize(cryptex1AppVOLPath);
    retassure(_client->cryptex1appvoldata = (char *)alloc.allocate(_client->cryptex1appvoldatasize),
              "%s: failed to allocate memory for %s\n", __func__, cryptex1AppVOLPath.c_str());
    cryptex1AppVOLFileStream.read((char *) _client->cryptex1appvoldata,
                                  (std::streamsize) _client->cryptex1appvoldatasize);
    retassure(cryptex1AppVOLFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, cryptex1AppVOLPath.c_str());
    retassure(*(uint64_t *) (_client->cryptex1appvoldata) != 0,
              "%s: failed to load Cryptex1,AppVolume for %s with the size %zu!\n",
              __func__, cryptex1AppVOLPath.c_str(), _client->cryptex1appvoldatasize);

    std::ifstream cryptex1AppTCFileStream(cryptex1AppTCPath, std::ios::in | std::ios::binary);
    retassure(cryptex1AppTCFileStream.good(), "%s: failed init file stream for %s!\n", __func__, cryptex1AppTCPath.c_str());
    _client->cryptex1apptcdatasize = futurerestore::getFileSize(cryptex1AppTCPath);
    retassure(_client->cryptex1apptcdata = (char *)alloc.allocate(_client->cryptex1apptcdatasize),
              "%s: failed to allocate memory for %s\n", __func__, cryptex1AppTCPath.c_str());
    cryptex1AppTCFileStream.read((char *) _client->cryptex1apptcdata,
                                 (std::streamsize) _client->cryptex1apptcdatasize);
    retassure(cryptex1AppTCFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, cryptex1AppTCPath.c_str());
    retassure(*(uint64_t *) (_client->cryptex1apptcdata) != 0,
              "%s: failed to load Cryptex1,AppTrustCache for %s with the size %zu!\n",
              __func__, cryptex1AppTCPath.c_str(), _client->cryptex1apptcdatasize);
}

void futurerestore::loadYonkers(const std::array<std::string, 16>& yonkersPaths) const {
    int index = 0;
    for (const auto &yonkersPath: yonkersPaths) {
        std::ifstream yonkersFileStream(yonkersPath, std::ios::in | std::ios::binary);
        retassure(yonkersFileStream.good(), "%s: failed init file stream for %s!\n", __func__, yonkersPath.c_str());
        _client->yonkersfwdatasize[index] = futurerestore::getFileSize(yonkersPath);
        std::allocator<uint8_t> alloc;
        retassure(_client->yonkersfwdata[index] = (char *)alloc.allocate(_client->yonkersfwdatasize[index]),
                  "%s: failed to allocate memory for %s\n", __func__, yonkersPath.c_str());
        yonkersFileStream.read((char *) _client->yonkersfwdata[index],
        (std::streamsize) _client->yonkersfwdatasize[index]);
        retassure(yonkersFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, yonkersPath.c_str());
        retassure(*(uint64_t *) (_client->yonkersfwdata[index]) != 0,
                  "%s: failed to load Yonkers for %s with the size %zu!\n",
                  __func__, yonkersPath.c_str(), _client->yonkersfwdatasize[index]);
        index++;
    }
}

void futurerestore::loadRamdisk(const std::string& ramdiskPath) const {
    std::ifstream ramdiskFileStream(ramdiskPath, std::ios::in | std::ios::binary);
    retassure(ramdiskFileStream.good(), "%s: failed init file stream for %s!\n", __func__, ramdiskPath.c_str());
    _client->ramdiskdatasize = futurerestore::getFileSize(ramdiskPath);
    std::allocator<uint8_t> alloc;
    retassure(_client->ramdiskdata = (char *)alloc.allocate(_client->ramdiskdatasize),
              "%s: failed to allocate memory for %s\n", __func__, ramdiskPath.c_str());
    ramdiskFileStream.read((char *) _client->ramdiskdata,
                           (std::streamsize) _client->ramdiskdatasize);
    retassure(ramdiskFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, ramdiskPath.c_str());
    retassure(*(uint64_t *) (_client->ramdiskdata) != 0,
              "%s: failed to load Ramdisk for %s with the size %zu!\n",
              __func__, ramdiskPath.c_str(), _client->ramdiskdatasize);
}

void futurerestore::loadKernel(const std::string& kernelPath) const {
    std::ifstream kernelFileStream(kernelPath, std::ios::in | std::ios::binary);
    retassure(kernelFileStream.good(), "%s: failed init file stream for %s!\n", __func__, kernelPath.c_str());
    _client->kerneldatasize = futurerestore::getFileSize(kernelPath);
    std::allocator<uint8_t> alloc;
    retassure(_client->kerneldata = (char *)alloc.allocate(_client->kerneldatasize),
              "%s: failed to allocate memory for %s\n", __func__, kernelPath.c_str());
    kernelFileStream.read((char *) _client->kerneldata,
                          (std::streamsize) _client->kerneldatasize);
    retassure(kernelFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, kernelPath.c_str());
    retassure(*(uint64_t *) (_client->kerneldata) != 0,
              "%s: failed to load Kernel for %s with the size %zu!\n",
              __func__, kernelPath.c_str(), _client->kerneldatasize);
}

void futurerestore::loadSep(const std::string& sepPath) const {
    std::ifstream sepFileStream(sepPath, std::ios::binary | std::ios::in);
    retassure(sepFileStream.good(), "%s: failed init file stream for %s!\n", __func__, sepPath.c_str());
    _client->sepfwdatasize = futurerestore::getFileSize(sepPath);
    std::allocator<uint8_t> alloc;
    retassure(_client->sepfwdata = (char *)alloc.allocate(_client->sepfwdatasize),
              "%s: failed to allocate memory for %s\n", __func__, sepPath.c_str());
    sepFileStream.read(reinterpret_cast<char*>(_client->sepfwdata),
                          static_cast<std::streamsize>(_client->sepfwdatasize));
    retassure(sepFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, sepPath.c_str());
    retassure(*(uint64_t *) (_client->sepfwdata) != 0,
              "%s: failed to load SEP for %s with the size %zu!\n",
              __func__, sepPath.c_str(), _client->sepfwdatasize);
}

void futurerestore::loadBaseband(const std::string& basebandPath) {
    std::ifstream basebandFileStream(basebandPath, std::ios::binary | std::ios::in);
    retassure(basebandFileStream.good(), "%s: failed init file stream for %s!\n", __func__, basebandPath.c_str());
    uint64_t *basebandFront = nullptr;
    std::allocator<uint8_t> alloc;
    retassure(basebandFront = (uint64_t *)alloc.allocate(sizeof(uint64_t)),
              "%s: failed to allocate memory for %s\n", __func__, basebandPath.c_str());
    basebandFileStream.read((char *)basebandFront, (std::streamsize)sizeof(uint64_t));
    retassure(basebandFileStream.good(), "%s: failed to read file stream for %s!\n", __func__, basebandPath.c_str());
    retassure(*(uint64_t *) (basebandFront) != 0, "%s: failed to load Baseband for %s!\n",
              __func__, basebandPath.c_str());
}

char *futurerestore::readBaseband(const std::string& basebandPath, char *data, size_t *sz) {
    std::ifstream basebandFileStream(basebandPath, std::ios::binary | std::ios::in);
    if(!basebandFileStream.good()) {
        return nullptr;
    }
    size_t basebandSize = futurerestore::getFileSize(basebandPath);
    uint64_t *basebandData = nullptr;
    std::allocator<uint8_t> alloc;
    basebandData = (uint64_t *)alloc.allocate(basebandSize);
    if(!basebandData) {
        return nullptr;
    }
    basebandFileStream.read((char *)basebandData, (std::streamsize)basebandSize);
    if(!basebandFileStream.good()) {
        return nullptr;
    }
    *sz = basebandSize;
    return reinterpret_cast<char *>(basebandData);
}

unsigned char *futurerestore::getSHABuffer(char *data, size_t dataSize, int type) {
    std::allocator<uint8_t> alloc;
    auto *fileHash = (unsigned char *)nullptr;
    switch(type) {
        case 0:
            fileHash = (unsigned char *) alloc.allocate(48);
            memset(fileHash, 0, 48);
            SHA384((const unsigned char*)data, (unsigned int)dataSize, fileHash);
            break;
        case 1:
            fileHash = (unsigned char *) alloc.allocate(32);
            memset(fileHash, 0, 32);
            SHA256((const unsigned char*)data, (unsigned int)dataSize, fileHash);
            break;
        case 2:
            fileHash = (unsigned char *) alloc.allocate(64);
            memset(fileHash, 0, 64);
            SHA512((const unsigned char*)data, (unsigned int)dataSize, fileHash);
            break;
        case 3:
            fileHash = (unsigned char *) alloc.allocate(20);
            memset(fileHash, 0, 20);
            SHA1((const unsigned char*)data, (unsigned int)dataSize, fileHash);
            break;
        default:
            fileHash = (unsigned char *) alloc.allocate(48);
            memset(fileHash, 0, 48);
            SHA384((const unsigned char*)data, (unsigned int)dataSize, fileHash);
            break;
    }
    return fileHash;
}

unsigned char *futurerestore::getSHABufferStream(std::ifstream &stream, int type) {
  std::allocator<uint8_t> alloc;
  auto *fileHash = (unsigned char *)nullptr;
  const size_t bufferSize = 0x100000;
  char *buffer = (char *)alloc.allocate(bufferSize);
  switch(type) {
  case 1:
    fileHash = (unsigned char *) alloc.allocate(32);
    memset(fileHash, 0, 32);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    while(stream.good()) {
      stream.read(buffer, bufferSize);
      std::streamsize bytesRead = stream.gcount();
      if (bytesRead > 0) {
        SHA256_Update(&sha256, buffer, bytesRead);
      }
    }
    SHA256_Final(fileHash, &sha256);
    break;
  case 2:
    fileHash = (unsigned char *) alloc.allocate(64);
    memset(fileHash, 0, 64);
    SHA512_CTX sha512;
    SHA512_Init(&sha512);

    while(stream.good()) {
      stream.read(buffer, bufferSize);
      std::streamsize bytesRead = stream.gcount();
      if (bytesRead > 0) {
        SHA512_Update(&sha512, buffer, bytesRead);
      }
    }
    SHA512_Final(fileHash, &sha512);
    break;
  case 3:
    fileHash = (unsigned char *) alloc.allocate(20);
    memset(fileHash, 0, 20);
    SHA_CTX sha1;
    SHA1_Init(&sha1);

    while(stream.good()) {
      stream.read(buffer, bufferSize);
      std::streamsize bytesRead = stream.gcount();
      if (bytesRead > 0) {
        SHA1_Update(&sha1, buffer, bytesRead);
      }
    }
    SHA1_Final(fileHash, &sha1);
    break;
  default:
    fileHash = (unsigned char *) alloc.allocate(48);
    memset(fileHash, 0, 48);
    SHA512_CTX sha384;
    SHA384_Init(&sha384);

    while(stream.good()) {
      stream.read(buffer, bufferSize);
      std::streamsize bytesRead = stream.gcount();
      if (bytesRead > 0) {
        SHA384_Update(&sha384, buffer, bytesRead);
      }
    }
    SHA384_Final(fileHash, &sha384);
    break;
  }
  free(buffer);
  stream.seekg(0, std::ios::beg);
  return fileHash;
}

size_t futurerestore::getFileSize(const std::string &name) {
#ifdef WIN32
    struct _stat64 st{0};
    if(_stat64(name.c_str(), &st) < 0) {
#else
    struct stat st{0};
    if(stat(name.c_str(), &st) < 0) {
#endif
        error("%s: failed to get file size for %s\n", __func__, name.c_str());
        return 0;
    }
    return st.st_size;
}

unsigned char *futurerestore::getSHA(const std::string& filePath, int type) {
    std::ifstream fileStream(filePath, std::ios::binary | std::ios::in);
    if(!fileStream.good()) {
        info("Cached %s not found, downloading a new one.\n", filePath.c_str());
        return nullptr;
    }
    size_t dataSize = futurerestore::getFileSize(filePath);
    if(!dataSize) {
        error("%s: failed to get file size for %s\n", __func__, filePath.c_str());
        return nullptr;
    }
    std::allocator<uint8_t> alloc;
    char *data = nullptr;
    try {
      data = (char *) alloc.allocate(8);
    } catch(std::bad_alloc const&) {
        if(!data) {
            error("%s: failed to allocate memory for %s\n", __func__, filePath.c_str());
            throw;
        }
    }
    if(!data) {
      error("%s: failed to allocate memory for %s\n", __func__, filePath.c_str());
      return nullptr;
    }
    fileStream.read((char *) data,
                       (std::streamsize) 8);
    if(!fileStream.good()) {
        info("Failed to read cached file %s, downloading a new one.\n", filePath.c_str());
        return nullptr;
    }
    if(*(uint64_t *)(data) == 0) {
        error("%s: failed to load File for %s with the size %zu!\n",
              __func__, filePath.c_str(), dataSize);
        return nullptr;
    }
    fileStream.seekg(0, std::ios::beg);
    return getSHABufferStream(fileStream, type);
}

#pragma mark static methods

inline void futurerestore::saveStringToFile(std::string &str, std::string &path) {
    if(str.empty() || path.empty()) {
        info("%s: No data to save!\n", __func__);
        return;
    }
    std::ofstream fileStream(path, std::ios::out | std::ios::binary);
    retassure(fileStream.good(), "%s: failed init file stream for %s!\n", __func__, path.c_str());
    fileStream.write(str.data(), static_cast<std::streamsize>(str.length()));
    retassure((fileStream.good()), "Can't save file at %s\n", path.c_str());
}

std::pair<const char *, size_t> futurerestore::getNonceFromSCAB(const char *scab, size_t scabSize) {
    retassure(scab, "Got empty SCAB\n");

    img4tool::ASN1DERElement bacs(scab, scabSize);

    try {
        bacs[3];
    } catch (...) {
        reterror("unexpected number of Elements in SCAB sequence (expects 4)\n");
    }

    img4tool::ASN1DERElement mainSet = bacs[1];

    for (auto &elem: mainSet) {
        if (*(uint8_t *) elem.buf() == 0x92) {
            return {(char *) elem.payload(), elem.payloadSize()};
        }
    }
    reterror("failed to get nonce from SCAB");
    return {NULL, 0};
}

uint64_t futurerestore::getEcidFromSCAB(const char *scab, size_t scabSize) {
    retassure(scab, "Got empty SCAB\n");

    img4tool::ASN1DERElement bacs(scab, scabSize);

    try {
        bacs[3];
    } catch (...) {
        reterror("unexpected number of Elements in SCAB sequence (expects 4)\n");
    }

    img4tool::ASN1DERElement mainSet = bacs[1];

    for (auto &elem: mainSet) {
        if (*(uint8_t *) elem.buf() == 0x81) {
            uint64_t ret = 0;
            for (int i = 0; i < elem.payloadSize(); i++) {
                ret <<= 8;
                ret |= ((uint8_t *) elem.payload())[i];
            }
            return __bswap_64(ret);
        }
    }

    reterror("failed to get ECID from SCAB");
    return 0;
}

std::pair<const char *, size_t> futurerestore::getRamdiskHashFromSCAB(const char *scab, size_t scabSize) {
    retassure(scab, "Got empty SCAB\n");

    img4tool::ASN1DERElement bacs(scab, scabSize);

    try {
        bacs[3];
    } catch (...) {
        reterror("unexpected number of Elements in SCAB sequence (expects 4)\n");
    }

    img4tool::ASN1DERElement mainSet = bacs[1];

    for (auto &elem: mainSet) {
        if (*(uint8_t *) elem.buf() == 0x9A) {
            return {(char *) elem.payload(), elem.payloadSize()};
        }
    }
    reterror("failed to get nonce from SCAB");
    return {NULL, 0};
}

plist_t futurerestore::loadPlistFromFile(const char *path) {
    plist_t ret = nullptr;

    FILE *f = fopen(path, "rb");
    if (!f) {
        error("Could not open file %s\n", path);
        return nullptr;
    }
    fseek(f, 0, SEEK_END);
    size_t bufSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = (char *) malloc(bufSize);
    if (!buf) {
        error("failed to alloc memory\n");
        return nullptr;
    }

    size_t freadRet = 0;
    if ((freadRet = fread(buf, 1, bufSize, f)) != bufSize) {
        error("fread=%zu but bufsize=%zu", freadRet, bufSize);
        return nullptr;
    }
    fclose(f);

    if (memcmp(buf, "bplist00", 8) == 0)
        plist_from_bin(buf, (uint32_t) bufSize, &ret);
    else
        plist_from_xml(buf, (uint32_t) bufSize, &ret);
    free(buf);

    return ret;
}

char *futurerestore::getPathOfElementInManifest(const char *element, const char *manifeststr, const char *boardConfig,
                                                int isUpdateInstall) {
    char *pathStr = nullptr;
    ptr_smart<plist_t> buildmanifest(NULL, plist_free);

    plist_from_xml(manifeststr, (uint32_t) strlen(manifeststr), &buildmanifest);

    if (plist_t identity = getBuildidentityWithBoardconfig(buildmanifest._p, boardConfig, isUpdateInstall))
        if (plist_t manifest = plist_dict_get_item(identity, "Manifest"))
            if (plist_t elem = plist_dict_get_item(manifest, element))
                if (plist_t info = plist_dict_get_item(elem, "Info"))
                    if (plist_t path = plist_dict_get_item(info, "Path"))
                        if (plist_get_string_val(path, &pathStr), pathStr)
                            goto noerror;

    reterror("Could not get %s path\n", element);
    noerror:
    return pathStr;
}

unsigned char *futurerestore::getDigestOfElementInManifest(const char *element, const char *manifeststr, const char *boardConfig,
                                                int isUpdateInstall) {
    char *digestStr = nullptr;
    ptr_smart<plist_t> buildmanifest(NULL, plist_free);
    uint64_t size;

    plist_from_xml(manifeststr, (uint32_t) strlen(manifeststr), &buildmanifest);

    if (plist_t identity = getBuildidentityWithBoardconfig(buildmanifest._p, boardConfig, isUpdateInstall)) {
        if (plist_t manifest = plist_dict_get_item(identity, "Manifest")) {
            if (plist_t elem = plist_dict_get_item(manifest, element)) {
                if (plist_t path = plist_dict_get_item(elem, "Digest")) {
                    if (plist_get_data_val(path, &digestStr, &size), digestStr) {
                        goto noerror;
                    }
                }
            }
        }
    }

    return nullptr;
    noerror:
    return (unsigned char *)digestStr;
}

unsigned char *futurerestore::getBBCFGDigestInManifest(const char *manifeststr, const char *boardConfig,
                                                int isUpdateInstall) {
    char *digestStr = nullptr;
    ptr_smart<plist_t> buildmanifest(NULL, plist_free);
    uint64_t size;

    plist_from_xml(manifeststr, (uint32_t) strlen(manifeststr), &buildmanifest);

    if (plist_t identity = getBuildidentityWithBoardconfig(buildmanifest._p, boardConfig, isUpdateInstall)) {
        if (plist_t manifest = plist_dict_get_item(identity, "Manifest")) {
            if (plist_t elem = plist_dict_get_item(manifest, "BasebandFirmware")) {
                if (plist_t path = plist_dict_get_item(elem, "BBCFG-DownloadDigest")) {
                    if (plist_get_data_val(path, &digestStr, &size), digestStr) {
                        goto noerror;
                    }
                }
            }
        }
    }

    return nullptr;
    noerror:
    return (unsigned char *)digestStr;
}

bool
futurerestore::elemExists(const char *element, const char *manifeststr, const char *boardConfig, int isUpdateInstall) {
    char *pathStr = nullptr;
    ptr_smart<plist_t> buildmanifest(NULL, plist_free);

    plist_from_xml(manifeststr, (uint32_t) strlen(manifeststr), &buildmanifest);

    if (plist_t identity = getBuildidentityWithBoardconfig(buildmanifest._p, boardConfig, isUpdateInstall))
        if (plist_t manifest = plist_dict_get_item(identity, "Manifest"))
            if (plist_t elem = plist_dict_get_item(manifest, element))
                if (plist_t info = plist_dict_get_item(elem, "Info"))
                    if (plist_t path = plist_dict_get_item(info, "Path"))
                        if (plist_get_string_val(path, &pathStr), pathStr)
                            goto noerror;

    return false;
    noerror:
    return true;
}

std::string futurerestore::getGeneratorFromSHSH2(plist_t shsh2) {
    plist_t pGenerator = nullptr;
    uint64_t gen = 0;
    char *genstr = nullptr;
    cleanup([&] {
        safeFree(genstr);
    });
    retassure(pGenerator = plist_dict_get_item(shsh2, "generator"), "signing ticket file does not contain generator");

    retassure(plist_get_node_type(pGenerator) == PLIST_STRING,
              "generator has unexpected type! We expect string of the format 0x%16llx");

    plist_get_string_val(pGenerator, &genstr);
    assure(genstr);


    gen = std::stoull(genstr, nullptr, 16);
    retassure(gen, "failed to parse generator. Make sure it is in format 0x%16llx");

    return {genstr};
}

// TODO: implement windows CI and enable update check
#ifndef WIN32

void futurerestore::checkForUpdates() {
    info("Checking for updates...\n");
    std::string url = "https://nightly.link/futurerestore/futurerestore/workflows/ci/main/Versioning.zip";
    bool fail = false;
    char *buffer = nullptr;
    uint32_t sz = 0;
    uint32_t sz2 = 0;
    if(download_to_buffer(url.c_str(), &buffer, &sz)) {
        fail = true;
    } else {
        sz2 = sz;
        const char *tmp = extractZipFileToString(buffer, "latest_build_num.txt", &sz);
        if(!tmp) {
            fail = true;
        } else {
            std::string num_str = std::string(tmp);
            if (num_str.empty()) {
                fail = true;
            } else {
                this->latest_num = num_str;
            }
        }
    }
    const char *tmp = extractZipFileToString(buffer, "latest_build_sha.txt", &sz2);
    if(!tmp) {
        fail = true;
    } else {
        std::string sha_str = std::string(tmp);
        if (sha_str.empty()) {
            fail = true;
        } else {
            this->latest_sha = sha_str;
        }
    }

    if(this->latest_num.empty() || this->latest_sha.empty() || fail) {
        info("ERROR: failed to check for futurerestore updates! continuing...\n");
        return;
    }
    bool updated = false;
    if(std::equal(this->current_sha.begin(), this->current_sha.end(), this->latest_sha.begin())) {
       updated = true;
    } else {
        if (std::stoi(this->current_num) < std::stoi(this->latest_num)) {
            info("Error: Futurerestore is outdated! Please download the latest futurerestore! exitting...\n");
            exit(-1);
        } else {
            updated = true;
        }
    }
    if(updated) {
        info("Futurerestore is up to date!\n");
    }
}

#endif

const char *futurerestore::extractZipFileToString(char *zip_buffer, const char *file, uint32_t *sz) {
    std::allocator<uint8_t> alloc;
    zip_error_t *err = nullptr;
    unsigned char *target_buffer = nullptr;
    zip_source_t *target_source_zip = zip_source_buffer_create(zip_buffer, *sz, 0, err);
    if(err != nullptr) {
        return nullptr;
    }
    if(target_source_zip == nullptr) {
        return nullptr;
    }
    zip_t *target_zip = zip_open_from_source(target_source_zip, 0, err);
    if(err != nullptr) {
        info("err: %d %s\n", __LINE__, zip_error_strerror(err));
    }
    if(target_zip == nullptr) {
        return nullptr;
    } else {
        size_t idx = zip_name_locate(target_zip, file, 0);
        if(idx < 0) {
            return nullptr;
        } else {
            struct zip_stat zstat{0};
            zip_stat_init(&zstat);
            if(zip_stat_index(target_zip, idx, 0, &zstat) != 0) {
                return nullptr;
            } else {
                struct zip_file* target_file = zip_fopen_index(target_zip, idx, 0);
                if (target_file == nullptr) {
                    return nullptr;
                } else {
                    target_buffer = (unsigned char*)alloc.allocate(zstat.size+1);
                    if(target_buffer == nullptr) {
                        return nullptr;
                    } else {
                        if (zip_fread(target_file, target_buffer, zstat.size) != zstat.size) {
                            return nullptr;
                        } else {
                            target_buffer[zstat.size] = '\0';
                            *sz = zstat.size;
                        }
                    }
                }
                zip_fclose(target_file);
            }
        }
    }
    return (const char *)target_buffer;
}

bool futurerestore::is32bit() const {
    bool ret = !is_image4_supported(_client);
    if(_client) {
        dfu_client_free(_client);
    }
    if(_client) {
        recovery_client_free(_client);
    }
    getDeviceMode(true);
    return ret;
}
