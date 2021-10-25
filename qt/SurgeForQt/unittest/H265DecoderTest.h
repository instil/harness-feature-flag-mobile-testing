#ifndef H265DECODERTEST_H
#define H265DECODERTEST_H

#include <QtTest>
#include <QVideoWidget>
#include "H265Decoder.h"
#include "TestSuite.h"

class H265DecoderTest : public TestSuite {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void ShouldDecodeValidH265Stream();
    void ShouldNotDecodeInvalidH265Stream();
    void ShouldFirePlaybackStartedSignal();

private:
    QByteArray* ValidH265Stream();

private:
    H265Decoder *target;
    QVideoWidget *videoWidget;
    Dispatcher dispatcher;
    QThread *thread;
};

//static H265DecoderTest TEST_H265DECODER;

#endif // H265DECODERTEST_H
