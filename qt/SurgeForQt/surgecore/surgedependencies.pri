mac: SURGE_PLATFORM = macos
else: SURGE_PLATFORM = linux

contains(QT_ARCH, arm64): SURGE_ARCH = armv8
else: SURGE_ARCH = x86

INCLUDEPATH += $$PWD/../../../core/lib/libuv-cmake/include \
               $$PWD/../../../core/lib/openssl-mobile/include

LIBS += -L$$PWD/../../../core/lib/libuv-cmake/lib/$$SURGE_PLATFORM/$$SURGE_ARCH -luv_a \
        -L$$PWD/../../../core/lib/openssl-mobile/lib/$$SURGE_PLATFORM/$$SURGE_ARCH -lssl -lcrypto -ldl
