# Surge W.I.P

RTSP implementation from scratch.

Currently implements RTSP Interleaved and handles SessionDescriptionV0 protocol and ignores RTCP. Only H264 depacketization so far.

## Compilation

To build for local development:

```bash
$ brew install cmake ffmpeg
$ mkdir build
$ cd build
$ cmake -DUNIT_TESTS=ON -DDEBUG_SYMBOLS=ON ../
$ make
```

Development RTSP Server

```bash
$ ffserver -f ffserver.conf
```

Run Tests:

```bash
$ ./test/testrunner
```

### XCode

If you prefer to use Xcode to develop use:

```bash
$ cmake -DUNIT_TESTS=ON -DDEBUG_SYMBOLS=ON -GXcode ../
```

Though the cmake build will need to be fixed/changed so tests will compile. This is broken because
of external project fetching (gtest/gmock).

### Clion

CLion automatically supports CMake projects out of the box. Other IDE support avaialble throught cmake generators see:

```bash
$ cmake --help
```

## Debugging

ScratchTest.cc is a test file for playing with usage of the library and i run:

```bash
$ lldb -- ./test/testrunner --gtest_filter=SCRATCH.SIMPLE_SCRATCH
```

## Profiling

To profile i use Valgrind:

```bash
$ brew install --HEAD valgrind
$ valgrind -- ./test/testrunner
```

## iOS

There is an iOS Integration Test within IntegrationTests/. To build a release library for iOS:

```bash
$ ./release-ios.sh
```

## Android

TODO

## Python

TODO
