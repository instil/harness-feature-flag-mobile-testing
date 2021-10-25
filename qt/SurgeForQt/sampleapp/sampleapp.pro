QT += quick \
      multimedia \
      multimediawidgets \
      widgets

CONFIG += c++14

SOURCES += \
        MainViewModel.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    MainViewModel.h \
    StacktracePrinter.h \
    ViewModel.h

MOC_DIR = ./src/moc
OBJECTS_DIR = ./src/objects
RCC_DIR = ./src/resource

INCLUDEPATH += $$PWD/../surge

LIBS += -L$$OUT_PWD/../surge/ -lsurge \
        -L$$OUT_PWD/../surgecore/ -lsurgecore

include(../surgecore/surgedependencies.pri)

# Debug symbols for segfault stacktrace logging
linux: QMAKE_LFLAGS += -g -rdynamic
