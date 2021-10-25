QT += testlib
QT += multimedia \
      multimediawidgets \
      widgets
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++14

TEMPLATE = app

SOURCES +=  \
    ByteStreamTest.cpp \
    DateTimeConverterTest.cpp \
    DecoderFactoryTest.cpp \
    DiagnosticsTrackerTest.cpp \
    DispatcherTest.cpp \
    H264DecoderTest.cpp \
    H264NaluParserTest.cpp \
    H265DecoderTest.cpp \
    MjpegDecoderTest.cpp \
    SurgeVideoSurfaceTest.cpp \
    TestSuite.cpp \
    main.cpp

HEADERS += \
    ByteStreamTest.h \
    DateTimeConverterTest.h \
    DecoderFactoryTest.h \
    DiagnosticsTrackerTest.h \
    DispatcherTest.h \
    H264DecoderTest.h \
    H264NaluParserTest.h \
    H265DecoderTest.h \
    MjpegDecoderTest.h \
    SurgeVideoSurfaceTest.h \
    TestSuite.h \
    TestUtils.h \

MOC_DIR = ./src/moc
OBJECTS_DIR = ./src/objects
RCC_DIR = ./src/resource

LIBS += -L$$OUT_PWD/../surge/ -lsurge \
        -L$$OUT_PWD/../surgecore/ -lsurgecore

INCLUDEPATH += $$PWD/../surge ../../../core/src/
DEPENDPATH += $$PWD/../surge

PRE_TARGETDEPS += $$OUT_PWD/../surge/libsurge.a

include(../googletest/googletest.pri)
include(../surgecore/surgedependencies.pri)
