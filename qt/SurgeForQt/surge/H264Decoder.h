#ifndef H264DECODER_H
#define H264DECODER_H

#include <QMediaPlayerDecoder.h>

class H264Decoder : public QMediaPlayerDecoder {
    Q_OBJECT

public:
    H264Decoder(QAbstractVideoSurface *videoSurface, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker) : QMediaPlayerDecoder(videoSurface, dispatcher, diagnosticsTracker) { }

protected:
    VideoFormat GetDecoderFormat() {
        return VideoFormat::H264;
    }

private:
    QUrl GstreamerPipeline() {
#if defined(Q_PROCESSOR_ARM) && defined(Q_OS_LINUX)
        return QUrl("gst-pipeline: appsrc ! queue ! h264parse ! vpudec ! qtvideosink sync=false");
#elif defined(Q_OS_LINUX)
        return QUrl("gst-pipeline: appsrc ! h264parse ! avdec_h264 ! qtvideosink sync=false");
#else
        return QUrl();
#endif
    }

};

#endif // H264DECODER_H
