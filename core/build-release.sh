#!/usr/bin/env bash

set -e

mkdir build
cd build

cmake -DCODE_COVERAGE=ON -DDEBUG_SYMBOLS=ON -DUNIT_TESTS=ON ../
make

# lcov --zerocounters --directory ../
# lcov --capture --initial --directory ../ --output-file app
#
# # run the tests
# valgrind --leak-check=full --track-origins=yes --trace-children=yes --xml=yes --xml-file=./valgrind.result -- ./test/testrunner --gtest_output=xml:gtestresults.xml
#
# lcov --no-checksum --directory . --capture --output-file app.info
# python ../tools/lcov_cobertura.py app.info
#
# popd
#
# rm -f ./cppcheck.xml
# rm -f ./src/wrappers/java/interface_wrap.* # don't scan the swig stuff nothing i can do about those issues.
# cppcheck --std=c++11 --xml --xml-version=2 --enable=warning,performance,portability,information ./src/ 2> cppcheck.xml
