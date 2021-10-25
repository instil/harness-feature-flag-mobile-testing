#ifndef H265DECODER_H
#define H265DECODER_H

#include <QMediaPlayerDecoder.h>

class H265Decoder : public QMediaPlayerDecoder {
    Q_OBJECT

public:
    H265Decoder(QAbstractVideoSurface *videoSurface, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker) : QMediaPlayerDecoder(videoSurface, dispatcher, diagnosticsTracker) { }

protected:
    VideoFormat GetDecoderFormat() {
        return VideoFormat::H265;
    }

private:
    QUrl GstreamerPipeline() {

#if defined(Q_PROCESSOR_ARM) && defined(Q_OS_LINUX)
        return QUrl("gst-pipeline: appsrc ! h265parse ! vpudec ! qtvideosink sync=false");
#elif defined(Q_OS_LINUX)
        return QUrl("gst-pipeline: appsrc ! h265parse ! avdec_h265 ! qtvideosink sync=false");
#else
        return QUrl();
#endif
    }
};

#endif // H265DECODER_H
