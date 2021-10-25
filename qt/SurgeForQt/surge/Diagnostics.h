#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <QObject>
#include <VideoFormat.h>

class Diagnostics : public QObject {
    Q_OBJECT

public:
    virtual ~Diagnostics() { }

signals:
    void FramerateChanged(int fps);
    void BitrateUpdated(int bitrate);
    void PacketCountUpdated(int packetLossCount, int successfulPacketCount, int outOfOrderPacketCount);
    void MediaSizeChanged(int width, int height);
    void VideoFormatChanged(VideoFormat format);
};


#endif // DIAGNOSTICS_H
