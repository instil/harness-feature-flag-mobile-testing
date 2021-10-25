#ifndef URLPROTOCOL_H
#define URLPROTOCOL_H

#include <QString>

enum UrlProtocol {
    UNKNOWN = 0,
    RTSP,
    HTTP
};

static UrlProtocol UrlProtocolFromString(QString url) {
    if (url.startsWith("rtsp")) {
        return RTSP;
    } else if (url.startsWith("http")) {
        return HTTP;
    } else {
        return UNKNOWN;
    }
}

#endif // URLPROTOCOL_H
