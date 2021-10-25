TEMPLATE = lib
CONFIG += static exceptions
CONFIG -= debug_and_release

TARGET = googletest

INCLUDEPATH += \
    $$PWD/../../../core/lib/googletest/googletest/include \
    $$PWD/../../../core/lib/googletest/googlemock/include \
    $$PWD/../../../core/lib/googletest/googletest \
    $$PWD/../../../core/lib/googletest/googlemock

SOURCES = \
    $$PWD/../../../core/lib/googletest/googletest/src/gtest-all.cc \
    $$PWD/../../../core/lib/googletest/googlemock/src/gmock-all.cc
