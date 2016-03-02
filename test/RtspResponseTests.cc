#include <gtest/gtest.h>

#include "RtspResponse.h"

#include <cstring>

TEST(RTSP_RESPONSE, TestThatCanParseStatusCode) {
    const char *test_response =
        "RTSP/1.0 200 OK\r\nCSeq: 0\r\nContent-Type: application/sdp\r\n";

    Surge::Response resp((unsigned char *)test_response, strlen(test_response), false);
    Surge::RtspResponse response(&resp);

    ASSERT_EQ(response.GetCode(), 200);
}

TEST(RTSP_RESPONSE, TestThat200StatusCodeIsOk) {
    const char *test_response =
        "RTSP/1.0 200 OK\r\nCSeq: 0\r\nContent-Type: application/sdp\r\n";

    Surge::Response resp((unsigned char *)test_response, strlen(test_response), false);
    Surge::RtspResponse response(&resp);

    ASSERT_EQ(response.Ok(), true);
}

TEST(RTSP_RESPONSE, TestThatNon200StatusCodeIsNotOk) {
    const char *test_response =
        "RTSP/1.0 500 OK\r\nCSeq: 0\r\nContent-Type: application/sdp\r\n";

    Surge::Response resp((unsigned char *)test_response, strlen(test_response), false);
    Surge::RtspResponse response(&resp);

    ASSERT_EQ(response.Ok(), false);
}

TEST(RTSP_RESPONSE, TestThatCanParseBody) {
    const char *test_response =
        "RTSP/1.0 200 OK\r\nCSeq: 0\r\nContent-Type: application/sdp\r\n\r\nBODY\r\nBODY\r\n\r\n";

    Surge::Response resp((unsigned char *)test_response, strlen(test_response), false);
    Surge::RtspResponse response(&resp);

    ASSERT_STREQ(response.GetBodyString().c_str(), "BODY\r\nBODY\r\n");
}
