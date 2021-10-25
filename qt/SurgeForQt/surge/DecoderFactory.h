#ifndef DECODERFACTORY_H
#define DECODERFACTORY_H

#include <Decoder.h>
#include <Dispatcher.h>
#include <QVideoWidget>
#include <SurgeCoreDeclarations.h>

class DecoderFactory {
public:
    Decoder *CreateDecoder(Surge::RtspSessionType codecType, QAbstractVideoSurface *videoView, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker);
};

#endif // DECODERFACTORY_H
