cmake_minimum_required(VERSION 3.19...3.24 FATAL_ERROR)
set(ignoreMe "${NO_PKGCFG} ${ASAN} ${ASAN_FLAGS} ${ARCH}")
option(SUBPROJECT_BUILD "Enables submodules to build as statically internally linked libs instead of binaries/tools" ON)
option(ASAN "Enable AddressSanatizer" OFF)
option(NO_PKGCFG "Disable pkgconfig searching of libs, use dep_root for linking" OFF)
#execute_process(COMMAND git submodule update --init --recursive WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_STRIP_TRAILING_WHITESPACE)

project(futurerestore VERSION 2.0.0 LANGUAGES C CXX)

if(ARCH STREQUAL "")
    set(ARCH "${CMAKE_SYSTEM_PROCESSOR}")
endif()

if("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Darwin")
    set(CMAKE_OSX_ARCHITECTURES "${ARCH}")
    if((NOT DEFINED MINVER) AND NO_PKGCFG)
        if("${ARCH}" STREQUAL "x86_64")
            set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
        else()
            set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)
        endif()
    endif()
    if(DEFINED MINVER)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "${MINVER}")
    endif()
    set(PROJECT_C_FLAGS "-arch ${ARCH} -fembed-bitcode=off -pthread -mrelax-all -Wno-deprecated-declarations -Wno-int-conversion")
    if(NOT "${CMAKE_BUILD_TYPE}" MATCHES "Debug" AND NOT "${CMAKE_BUILD_TYPE}" MATCHES "RelWithDebInfo")
        message("")
    else()
        set(CMAKE_CXX_COMPILE_OPTIONS_IPO "-fno-lto")
        set(CMAKE_C_COMPILE_OPTIONS_IPO "-fno-lto")
        set(_CMAKE_LTO_THIN FALSE)
        set(__lto_flags "-fno-lto")
    endif()
    set(PROJECT_CXX_FLAGS "${PROJECT_C_FLAGS}")
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Linux")
    set(PROJECT_C_FLAGS "-arch ${ARCH} -Wl,--allow-multiple-definition -pthread -mrelax-all -Wno-deprecated-declarations -Wno-int-conversion")
    set(PROJECT_CXX_FLAGS "${PROJECT_C_FLAGS}")
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "MSYS" OR "${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
    set(PROJECT_C_FLAGS "-arch ${ARCH} -Wl,--allow-multiple-definition -pthread -fPIC -Wno-deprecated-declarations -Wno-int-conversion -DCURL_STATICLIB -Bstatic -O0")
    set(PROJECT_CXX_FLAGS "${PROJECT_C_FLAGS}")
endif()
if(ASAN)
    set(ASAN ON)
    set(ASAN_FLAGS "-fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer ${ASAN_FLAGS}")
    set(CMAKE_C_LINK_LIBRARY_FLAG "${CMAKE_C_LINK_LIBRARY_FLAG} ${ASAN_FLAGS}")
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)
    set(CMAKE_BUILD_RPATH
            "/usr/local/lib"
            "/usr/lib"
            "/opt/procursus/lib"
            "/Library/Developer/CommandLineTools/usr/lib/clang/14.0.0/lib/darwin"
            "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/lib/darwin/"
            "/Applications/Xcode_14.1.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/lib/darwin/"
            "/Applications/Xcode_14.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/lib/darwin/")
    #hardcoded because actions never updates xcode
else()
    set(ASAN_FLAGS "")
endif()

if(NOT NO_PKGCFG)
    find_package(PkgConfig REQUIRED FATAL_ERROR)
endif()


set(CMAKE_C_FLAGS_MINSIZEREL_INIT "${ADDITIONAL_C_FLAGS} -flto=thin ${PROJECT_C_FLAGS} ${CMAKE_C_FLAGS_MINSIZEREL_INIT}")
set(CMAKE_C_FLAGS_RELEASE_INIT "${ADDITIONAL_C_FLAGS} -flto=thin ${PROJECT_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE_INIT}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "${ADDITIONAL_C_FLAGS} ${PROJECT_C_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}")
set(CMAKE_C_FLAGS_DEBUG_INIT "${ADDITIONAL_C_FLAGS} ${PROJECT_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG_INIT} ${ASAN_FLAGS}")

set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT " ${ADDITIONAL_CXX_FLAGS} ${PROJECT_CXX_FLAGS} ${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "${ADDITIONAL_CXX_FLAGS} ${PROJECT_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE_INIT}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "${ADDITIONAL_CXX_FLAGS} ${PROJECT_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "${ADDITIONAL_CXX_FLAGS} ${PROJECT_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG_INIT} ${ASAN_FLAGS}")

set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL_INIT}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE_INIT}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_INIT}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_INIT}")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/tsschecker)
add_subdirectory(external/idevicerestore)
add_subdirectory(src)

#SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
