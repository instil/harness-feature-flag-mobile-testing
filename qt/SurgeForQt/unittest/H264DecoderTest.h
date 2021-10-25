#ifndef H264DECODERTEST_H
#define H264DECODERTEST_H

#include <QtTest>
#include <QVideoWidget>
#include "H264Decoder.h"
#include "TestSuite.h"

class H264DecoderTest : public TestSuite {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void ShouldDecodeValidH264Stream();
    void ShouldNotDecodeInvalidH264Stream();
    void ShouldFirePlaybackStartedSignal();

private:
    QByteArray* ValidH264Stream();

private:
    H264Decoder *target;
    QVideoWidget *videoWidget;
    Dispatcher dispatcher;
};

static H264DecoderTest TEST_H264DECODER;

#endif // H264DECODERTEST_H
