#include <gtest/gtest.h>

#include "RtspResponse.h"

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

TEST(RTSP_RESPONSE, TestThatCanAccessBody) {
    const char *test_response =
        "RTSP/1.0 200 OK\r\nCSeq: 0\r\nContent-Type: application/sdp\r\n\r\nBODYBODY";

    Surge::Response resp((unsigned char *)test_response, strlen(test_response), false);
    Surge::RtspResponse response(&resp);

    ASSERT_STREQ(response.GetBody().c_str(), "BODYBODY");
}
