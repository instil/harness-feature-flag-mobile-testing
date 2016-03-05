cmake_minimum_required(VERSION 2.6.3)
project(Surge)

include(TestBigEndian)
test_big_endian(TEST_ENDIAN)
message(STATUS "ENDIAN: ${TEST_ENDIAN}")
set(SURGE_IS_BIG_ENDIAN ${TEST_ENDIAN})

set(LIBRARY_TYPE STATIC)

option(FOR_IOS "Enable iOS specific extensions" OFF)
if(FOR_IOS)
  add_definitions(-D__IS_IOS__)
  set(LIBRARY_TYPE STATIC)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "8.0")
  set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "8.0")
endif()

option(FOR_ANDROID "Enable Android specific extensions" OFF)
if(FOR_ANDROID)
  add_definitions(-D__IS_ANDROID__)
  set(LIBRARY_TYPE SHARED)
endif()

option(BUILD_JAR "Build jni wrapper jar" OFF)
if(BUILD_JAR)
  find_package(Java REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.6" "-target" "1.6")
endif()

option(BUILD_JNI_WRAPPER "Build JNI Wrapper" OFF)
option(BUILD_XAMARIN_IOS_WRAPPER "Build xamarin iOS Wrapper" OFF)
option(BUILD_XAMARIN_ANDROID_WRAPPER "Build xamarin Android Wrapper" OFF)

Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

option(DEBUG_SYMBOLS "Turn on Debug Symbols" OFF)
if(DEBUG_SYMBOLS)
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

option(CODE_COVERAGE "Turn on Code Coverage" OFF)
if(CODE_COVERAGE)
  MESSAGE(STATUS "Turning on code-coverage")
  Set(CMAKE_CXX_FLAGS "-std=c++11 -g -O0 -Wall -fprofile-arcs -ftest-coverage")
  Set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
endif()

EXEC_PROGRAM(./version.sh "${PROJECT_SOURCE_DIR}"
  ARGS --version
  OUTPUT_VARIABLE SURGE_VERSION_STRING
  RETURN_VALUE)
EXEC_PROGRAM(./version.sh "${PROJECT_SOURCE_DIR}"
  ARGS --builder
  OUTPUT_VARIABLE SURGE_BUILDER
  RETURN_VALUE)
EXEC_PROGRAM(./version.sh "${PROJECT_SOURCE_DIR}"
  ARGS --date
  OUTPUT_VARIABLE SURGE_BUILD_DATE
  RETURN_VALUE)
configure_file (
  "${PROJECT_SOURCE_DIR}/src/Config.h.in"
  "${PROJECT_BINARY_DIR}/src/Config.h"
  )

include_directories ("${PROJECT_BINARY_DIR}/src")

include(CTest)
enable_testing()

include_directories(src)
add_subdirectory(src)

option(UNIT_TESTS "Enable unit-tests" OFF)
if(UNIT_TESTS)
  add_subdirectory(test)
endif()
