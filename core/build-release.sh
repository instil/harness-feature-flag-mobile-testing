#!/usr/bin/env bash

set -e

cd $(dirname $0)

echo "Building library"
mkdir -p build
cd build
cmake -DUNIT_TESTS=ON -DCODE_COVERAGE=ON -DDEBUG_SYMBOLS=ON ../
make

mkdir -p release/include
cp ../src/*.h release/include
cp src/libsurge.a release
cd release
zip -r surge.zip .
cd ..

echo "Executing tests"
./test/testrunner #--gtest_output=xml:build/reports/test-results.xml
