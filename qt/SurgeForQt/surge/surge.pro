QT += multimedia \
      multimediawidgets \
      widgets

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ByteStream.cpp \
    DateTimeConverter.cpp \
    DecoderFactory.cpp \
    DiagnosticsTracker.cpp \
    Dispatcher.cpp \
    H264NaluParser.cpp \
    MjpegDecoder.cpp \
    QMediaPlayerDecoder.cpp \
    SurgePlayer.cpp \
    SurgeRtspPlayer.cpp \
    SurgeVideoSurface.cpp

HEADERS += \
    ByteStream.h \
    DateTimeConverter.h \
    DecoderFactory.h \
    Diagnostics.h \
    DiagnosticsTracker.h \
    Dispatcher.h \
    H264Decoder.h \
    H264NaluParser.h \
    H264NaluSegment.h \
    H264NaluType.h \
    H265Decoder.h \
    MjpegDecoder.h \
    NaluMagicByteHeader.h \
    QMediaPlayerDecoder.h \
    RtspErrorCode.h \
    SurgeCoreDeclarations.h \
    SurgePlayer.h \
    SurgeRtspPlayer.h \
    Decoder.h \
    SurgeVideoSurface.h \
    UrlProtocol.h \
    VideoFormat.h

MOC_DIR = ./src/moc
OBJECTS_DIR = ./src/objects
RCC_DIR = ./src/resource

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

unix: LIBS += -L$$OUT_PWD/../surgecore/ -lsurgecore

INCLUDEPATH += $$PWD/../surgecore ../../../core/src/
DEPENDPATH += $$PWD/../surgecore

unix: PRE_TARGETDEPS += $$OUT_PWD/../surgecore/libsurgecore.a
