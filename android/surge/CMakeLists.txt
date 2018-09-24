# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=format-security -fexceptions -O0")
set(CMAKE_ANDROID_STL_TYPE c++_shared)
set(CMAKE_ANDROID_API_MIN 21)
set(CMAKE_ANDROID_API 23)

include_directories(../../core/lib/libuv-cmake/libuv/include)
include_directories(../../core/lib/libuv-cmake/libuv/src)
include_directories(../../core/lib/uvw/src)
include_directories(../../core/src)

include(../../core/lib/openssl-mobile/openssl-android.cmake)

set(LIBUV_SRC_DIR ../../core/lib/libuv-cmake/libuv/src)
file(GLOB SurgeDependancies
  ${LIBUV_SRC_DIR}/*.c
  ${LIBUV_SRC_DIR}/unix/*.c)
file(GLOB SurgeDependanciesExclude
  ${LIBUV_SRC_DIR}/unix/aix.c
  ${LIBUV_SRC_DIR}/unix/aix-common.c
  ${LIBUV_SRC_DIR}/unix/bsd-ifaddrs.c
  ${LIBUV_SRC_DIR}/unix/cygwin.c
  ${LIBUV_SRC_DIR}/unix/darwin-proctitle.c
  ${LIBUV_SRC_DIR}/unix/darwin.c
  ${LIBUV_SRC_DIR}/unix/freebsd.c
  ${LIBUV_SRC_DIR}/unix/fsevents.c
  ${LIBUV_SRC_DIR}/unix/ibmi.c
  ${LIBUV_SRC_DIR}/unix/kqueue.c
  ${LIBUV_SRC_DIR}/unix/netbsd.c
  ${LIBUV_SRC_DIR}/unix/openbsd.c
  ${LIBUV_SRC_DIR}/unix/no-fsevents.c
  ${LIBUV_SRC_DIR}/unix/no-proctitle.c
  ${LIBUV_SRC_DIR}/unix/os390.c
  ${LIBUV_SRC_DIR}/unix/os390-syscalls.c
  ${LIBUV_SRC_DIR}/unix/posix-poll.c
  ${LIBUV_SRC_DIR}/unix/posix-hrtime.c
  ${LIBUV_SRC_DIR}/unix/sunos.c)
list(REMOVE_ITEM SurgeDependancies ${SurgeDependanciesExclude})

file(GLOB SurgeSources ../../core/src/*.cc)
file(GLOB SurgeJniSources src/main/jni/*.cpp)
add_library(surge SHARED ${SurgeSources} ${SurgeJniSources} ${SurgeDependancies})

find_library(NdkLogLib log)
target_link_libraries(surge ${NdkLogLib})
target_link_libraries(surge ${OpenSSL})