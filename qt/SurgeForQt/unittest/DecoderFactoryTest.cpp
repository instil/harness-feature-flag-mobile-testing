#include "DecoderFactoryTest.h"

#include <MjpegDecoder.h>
#include <TestUtils.h>
#include <MimeTypes.h>
#include <H264Decoder.h>
#include <H265Decoder.h>

void DecoderFactoryTest::ShouldCreateAMJpegDecoder() {
    auto result = target.CreateDecoder(Surge::RtspSessionType::MJPEG, NULL, NULL, NULL);

    IsInstanceOf<Decoder *, MjpegDecoder *>(result);
}

void DecoderFactoryTest::ShouldCreateAH264Decoder() {
    auto result = target.CreateDecoder(Surge::RtspSessionType::H264, NULL, NULL, NULL);

    IsInstanceOf<Decoder *, H264Decoder *>(result);
}

void DecoderFactoryTest::ShouldCreateH265Decoder() {
    auto result = target.CreateDecoder(Surge::RtspSessionType::H265, NULL, NULL, NULL);

    IsInstanceOf<Decoder *, H265Decoder *>(result);
}

void DecoderFactoryTest::ShouldReturnNullForAnUnsupportedDecoderType() {
    auto result = target.CreateDecoder(Surge::RtspSessionType::UNKNOWN, NULL, NULL, NULL);

    QCOMPARE((long)result, NULL);
}
