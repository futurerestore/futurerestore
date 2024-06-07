//
//  futurerestore.hpp
//  futurerestore
//
//  Created by tihmstar on 14.09.16.
//  Copyright © 2016 tihmstar. All rights reserved.
//

#ifndef futurerestore_hpp
#define futurerestore_hpp

//make sure WIN32 is defined if compiling for windows
#if defined _WIN32 || defined __CYGWIN__
#ifndef WIN32
#define WIN32
#endif
#endif

#include <cstdio>
#include <functional>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <zip.h>
#include "idevicerestore.h"
#include <jssy.h>
#include <plist/plist.h>

template <typename T>
class ptr_smart {
    std::function<void(T)> _ptr_free = NULL;
public:
    T _p;
    ptr_smart(T p, std::function<void(T)> ptr_free){static_assert(std::is_pointer<T>(), "error: this is for pointers only\n"); _p = p;_ptr_free = ptr_free;}
    ptr_smart(T p){_p = p;}
    ptr_smart(){_p = NULL;}
    ptr_smart(ptr_smart &&p){ _p = p._p; _ptr_free = p._ptr_free; p._p = NULL; p._ptr_free = NULL;}
    ptr_smart& operator =(ptr_smart &&p){_p = p._p; _ptr_free = p._ptr_free; p._p = NULL; p._ptr_free = NULL; return *this;}
    T operator =(T p){ _p = p; return _p;}
    T operator =(T &p){_p = p; p = NULL; return _p;}
    T *operator&(){return &_p;}
    explicit operator const T() const {return _p;}
    operator const void*() const {return _p;}
    ~ptr_smart(){if (_p) (_ptr_free) ? _ptr_free(_p) : free((void*)_p);}
};

class futurerestore {
//    struct idevicerestore_client_t* _client;
    char *_ibootBuild = nullptr;
    bool _didInit = false;
    std::vector<plist_t> _aptickets;
    std::vector<std::pair<char *, size_t>>_im4ms;
    int _foundnonce = -1;
    bool _isUpdateInstall = false;
    bool _isPwnDfu = false;
    bool _noIBSS = false;
    bool _setNonce = false;
    bool _serial = false;
    bool _noRestore = false;
    bool _noRSEP = false;

    char *_firmwareJson = nullptr;
    char *_betaFirmwareJson = nullptr;
    char *_otaFirmwareJson = nullptr;
    jssytok_t *_firmwareTokens = nullptr;;
    jssytok_t *_betaFirmwareTokens = nullptr;
    jssytok_t *_otaFirmwareTokens = nullptr;
    char *_latestManifest = nullptr;
    char *_latestFirmwareUrl = nullptr;
    bool _useCustomLatest = false;
    bool _useCustomLatestBuildID = false;
    bool _useCustomLatestBeta = false;
    bool _useCustomLatestOTA = false;
    bool _useAppleDB = false;
    std::string _customLatest;
    std::string _customLatestBuildID;
    const char *_model = nullptr;
    const char *_board = nullptr;

    plist_t _sepbuildmanifest = nullptr;
    plist_t _basebandbuildmanifest = nullptr;
    plist_t _buildidentity = nullptr;

    std::string _ramdiskPath;
    std::string _kernelPath;
    std::string _sepPath;
    std::string _sepManifestPath;
    std::string _basebandPath;
    std::string _basebandManifestPath;


// TODO: implement windows CI and enable update check
#ifndef WIN32
    std::string latest_num;
    std::string latest_sha;
    std::string current_num = VERSION_COMMIT_COUNT;
    std::string current_sha = VERSION_COMMIT_SHA;
#endif

    const char *_custom_nonce = nullptr;
    const char *_boot_args = nullptr;

    bool _noCache = false;
    bool _skipBlob = false;

    bool _enterPwnRecoveryRequested = false;
    bool _rerestoreiOS9 = false;
    //methods
    void enterPwnRecovery(plist_t build_identity, std::string bootargs);
    void patchKernel(plist_t build_identity, std::string custom_seed = "");

public:
    void test() const;
    struct idevicerestore_client_t* _client;
    explicit futurerestore(bool isUpdateInstall = false, bool isPwnDfu = false, bool noIBSS = false, bool setNonce = false, bool serial = false, bool noRestore = false, bool noRSEP = false);
    bool init();
    int getDeviceMode(bool reRequest) const;
    uint64_t getDeviceEcid() const;
    void putDeviceIntoRecovery();
    void setAutoboot(bool val) const;
    void exitRecovery() const;
    void waitForNonce();
    void waitForNonce(std::vector<const char *>nonces, size_t nonceSize);
    void loadAPTickets(const std::vector<const char *> &apticketPaths);
    char *getiBootBuild();
    
    plist_t nonceMatchesApTickets();
    std::pair<const char *,size_t> nonceMatchesIM4Ms();

    void loadFirmwareTokens();
    const char *getDeviceModelNoCopy();
    const char *getDeviceBoardNoCopy();
    char *getLatestManifest();
    char *getLatestFirmwareUrl();
    std::string getSepManifestPath(){return _sepManifestPath;}
    std::string getBasebandManifestPath(){return _basebandManifestPath;}

// TODO: implement windows CI and enable update check
#ifndef WIN32
    void checkForUpdates();
#endif
    void downloadLatestRose();
    void downloadLatestSE();
    void downloadLatestSavage();
    void downloadLatestVeridian();
    void downloadLatestTimer();
    void downloadLatestBaobab();
    void downloadLatestCryptex1();
    void downloadLatestYonkers();
    void downloadLatestFirmwareComponents();
    void downloadLatestBaseband();
    void downloadLatestSep();
    
    void loadSepManifest(const std::string& sepManifestPath);
    void loadBasebandManifest(const std::string& basebandManifestPath);
    void loadRose(const std::string& rosePath) const;
    void loadSE(const std::string& sePath) const;
    void loadSavage(const std::array<std::string, 6>& savagePaths) const;
    void loadVeridian(const std::string& veridianDGMPath, const std::string& veridianFWMPath) const;
    void loadTimer(const std::string& timerPath) const;
    void loadBaobab(const std::string& baobabPath) const;
    void loadCryptex1(const std::string& cryptex1SysOSPath, const std::string& cryptex1SysVOLPath, const std::string& cryptex1SysTCPath, const std::string& cryptex1AppOSPath, const std::string& cryptex1AppVOLPath, const std::string& cryptex1AppTCPath) const;
    void loadYonkers(const std::array<std::string, 16>& yonkersPaths) const;
    void loadRamdisk(const std::string& ramdiskPath) const;
    void loadKernel(const std::string& kernelPath) const;
    void loadSep(const std::string& sepPath) const;
    static void loadBaseband(const std::string& basebandPath);
    static char *readBaseband(const std::string& basebandPath, char *data, size_t *sz);
    static unsigned char *getSHABuffer(char *data, size_t dataSize, int type = 0);
    static unsigned char *getSHABufferStream(std::ifstream &stream, int type = 0);
    static size_t getFileSize(const std::string &name) ;
    static unsigned char *getSHA(const std::string& filePath, int type = 0) ;

    void setCustomLatest(std::string version){_customLatest = std::move(version); _useCustomLatest = true;}
    void setCustomLatestBuildID(std::string version, bool beta, bool ota){_customLatestBuildID = std::move(version); _useCustomLatest = false; _useCustomLatestBuildID = true; _useCustomLatestBeta = beta; _useCustomLatestOTA = ota;}
    void setSepPath(std::string sepPath) {_sepPath = std::move(sepPath);}
    void setSepManifestPath(std::string sepManifestPath) {_sepManifestPath = std::move(sepManifestPath);}
    void setRamdiskPath(std::string ramdiskPath) {_ramdiskPath = std::move(ramdiskPath);}
    void setKernelPath(std::string kernelPath) {_kernelPath = std::move(kernelPath);}
    void setBasebandPath(std::string basebandPath) {_basebandPath = std::move(basebandPath);}
    void setBasebandManifestPath(std::string basebandManifestPath) {_basebandManifestPath = std::move(basebandManifestPath);}
    void setNonce(const char *custom_nonce){_custom_nonce = custom_nonce;};
    void setBootArgs(const char *boot_args){_boot_args = boot_args;};
    void disableCache(){_noCache = true;};
    void skipBlobValidation(){_skipBlob = true;};

    bool is32bit() const;

    uint64_t getBasebandGoldCertIDFromDevice() const;
    
    void doRestore(const char *ipsw);

#ifdef __APPLE__
    static int findProc(const char *procName, bool load);
    static void daemonManager(bool load);
#endif

    ~futurerestore();
    
    static std::pair<const char *,size_t> getRamdiskHashFromSCAB(const char* scab, size_t scabSize);
    static std::pair<const char *,size_t> getNonceFromSCAB(const char* scab, size_t scabSize);
    static uint64_t getEcidFromSCAB(const char* scab, size_t scabSize);
    static plist_t loadPlistFromFile(const char *path);
    static void saveStringToFile(std::string &str, std::string &path);
    static char *getPathOfElementInManifest(const char *element, const char *manifeststr, const char *boardConfig, int isUpdateInstall);
    static unsigned char *getDigestOfElementInManifest(const char *element, const char *manifeststr, const char *boardConfig, int isUpdateInstall);
    static unsigned char *getBBCFGDigestInManifest(const char *manifeststr, const char *boardConfig, int isUpdateInstall);
    static bool elemExists(const char *element, const char *manifeststr, const char *boardConfig, int isUpdateInstall);
    static std::string getGeneratorFromSHSH2(plist_t shsh2);
    static const char *extractZipFileToString(char *zip_buffer, const char *file, uint32_t *sz);

};

#endif /* futurerestore_hpp */
