#ifndef DECODERFACTORYTEST_H
#define DECODERFACTORYTEST_H

#include <QtTest>
#include <DecoderFactory.h>
#include <TestSuite.h>

class DecoderFactoryTest : public TestSuite {
    Q_OBJECT

private slots:
    void ShouldCreateAMJpegDecoder();
    void ShouldCreateAH264Decoder();
    void ShouldCreateH265Decoder();
    void ShouldReturnNullForAnUnsupportedDecoderType();

private:
    DecoderFactory target;
};

static DecoderFactoryTest TEST_DECODERFACTORY;

#endif // DECODERFACTORYTEST_H
