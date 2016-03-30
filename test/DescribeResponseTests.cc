#include <gtest/gtest.h>

#include "DescribeResponse.h"

TEST(DESCRIBE_RESPONSE, TestThatValidDescribeResponseWithPaletteIsOk) {

    std::string raw_response =
        "RTSP/1.0 200 OK\r\n"
        "CSeq: 1\r\n"
        "Date: Wed, 30 Mar 2016 14:36:09 GMT\r\n"
        "Content-Base: rtsp://localhost:8554/test.264/\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 287\r\n"
        "\r\n"
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0\r\n"
        "\r\n";

    Surge::Response wrapped_response((unsigned char *)raw_response.c_str(), raw_response.length());
    Surge::DescribeResponse response(&wrapped_response);

    ASSERT_EQ(response.Ok(), true);
}

TEST(DESCRIBE_RESPONSE, TestThat200DescribeResponseWithMissingBodyIsNotOk) {

    std::string raw_response =
        "RTSP/1.0 200 OK\r\n"
        "CSeq: 1\r\n"
        "Date: Wed, 30 Mar 2016 14:36:09 GMT\r\n"
        "Content-Base: rtsp://localhost:8554/test.264/\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 287\r\n"
        "\r\n";

    Surge::Response wrapped_response((unsigned char *)raw_response.c_str(), raw_response.length());
    Surge::DescribeResponse response(&wrapped_response);

    ASSERT_EQ(response.Ok(), false);
}
