# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=format-security -fexceptions -O0")
set(CMAKE_ANDROID_STL_TYPE c++_shared)
set(CMAKE_ANDROID_API_MIN 21)
set(CMAKE_ANDROID_API 23)

include_directories(../../core/lib/uvw/src)
include_directories(../../core/src)

include(../../core/lib/openssl-mobile/openssl-android.cmake)
include(../../core/lib/libuv-cmake/libuv-android.cmake)

file(GLOB SurgeSources ../../core/src/*.cc)
file(GLOB SurgeJniSources src/main/jni/*.cpp)
add_library(surge SHARED ${SurgeSources} ${SurgeJniSources}) #  ${SurgeDependancies}

find_library(NdkLogLib log)
target_link_libraries(surge ${NdkLogLib})
target_link_libraries(surge ${OpenSSL})
target_link_libraries(surge ${libuv})