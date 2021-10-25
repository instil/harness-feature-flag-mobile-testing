#include "DecoderFactory.h"

#include <H264Decoder.h>
#include <H265Decoder.h>
#include <MjpegDecoder.h>
#include <QDebug>
#include <MimeTypes.h>

Decoder *DecoderFactory::CreateDecoder(Surge::RtspSessionType codecType, QAbstractVideoSurface *videoView, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker) {
    switch (codecType) {
    case Surge::RtspSessionType::MJPEG:
        return new MjpegDecoder(videoView, dispatcher, diagnosticsTracker);
    case Surge::RtspSessionType::H264:
        return new H264Decoder(videoView, dispatcher, diagnosticsTracker);
    case Surge::RtspSessionType::H265:
        return new H265Decoder(videoView, dispatcher, diagnosticsTracker);
    }

    qCritical() << "Could not create decoder for type " << QString::fromStdString(Surge::RtspSessionTypeString::FromEnum(codecType));
    return NULL;
}
