QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += $$files(../../../core/src/*.cc, true)

HEADERS += $$files(../../../core/src/*.h, true)

DISTFILES += \
    surgedependencies.pri

MOC_DIR = ./src/moc
OBJECTS_DIR = ./src/objects
RCC_DIR = ./src/resource

INCLUDEPATH += \
    $$PWD/../../../core/lib/uvw/src

include(surgedependencies.pri)
