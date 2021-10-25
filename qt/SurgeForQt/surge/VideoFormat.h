#ifndef VIDEOFORMAT_H
#define VIDEOFORMAT_H

#include <QObject>

enum VideoFormat : int {
    H264,
    H265,
    MJPEG
};

Q_DECLARE_METATYPE(VideoFormat);

#endif // VIDEOFORMAT_H
