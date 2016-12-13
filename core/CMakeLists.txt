cmake_minimum_required(VERSION 2.6.3)
project(Surge)

set(LIBRARY_TYPE STATIC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

include_directories(./lib/libuv-cmake/libuv/include)
add_subdirectory(./lib/libuv-cmake)

include_directories(./lib/uvw/src)

include_directories ("${PROJECT_BINARY_DIR}/src")
include_directories(src)
add_subdirectory(src)

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

include(CTest)
enable_testing()
option(UNIT_TESTS "Enable unit-tests" ON)
if(UNIT_TESTS)
  set(BUILD_GMOCK ON)
  set(BUILD_GTEST ON)
  include_directories(./lib/googletest/googletest/include/)
  include_directories(./lib/googletest/googlemock/include/)
  add_subdirectory(lib/googletest)
  add_subdirectory(test)
endif()
