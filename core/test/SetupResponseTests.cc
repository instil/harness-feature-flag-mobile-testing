#include <gtest/gtest.h>

#include "SetupResponse.h"

TEST(SETUP_RESPONSE, TestThatValidSetupResponseIsOk) {
    std::string raw_response =
        "RTSP/1.0 200 OK\r\n"
        "CSeq: 2\r\n"
        "Date: Wed, 30 Mar 2016 14:31:33 GMT\r\n"
        "Session: e685a29b5421ead7\r\n"
        "Transport: RTP/AVP/TCP;interleaved=0-1\r\n"
        "\r\n";

    Surge::Response wrapped_response((unsigned char *)raw_response.c_str(), raw_response.length());
    Surge::SetupResponse response(&wrapped_response);

    ASSERT_EQ(response.Ok(), true);
}

TEST(SETUP_RESPONSE, TestThatSpecifiedTimeoutIsParsed) {
    std::string raw_response =
        "RTSP/1.0 200 OK\r\n"
        "CSeq: 2\r\n"
        "Date: Wed, 30 Mar 2016 14:31:33 GMT\r\n"
        "Session: e685a29b5421ead7;timeout=123\r\n"
        "Transport: RTP/AVP/TCP;interleaved=0-1\r\n"
        "\r\n";

    Surge::Response wrapped_response((unsigned char *)raw_response.c_str(), raw_response.length());
    Surge::SetupResponse response(&wrapped_response);

    ASSERT_EQ(response.GetTimeoutSeconds(), 123);
}

TEST(SETUP_RESPONSE, TestThatInterleavedChannelsAreParsed) {
    std::string raw_response =
        "RTSP/1.0 200 OK\r\n"
        "CSeq: 2\r\n"
        "Date: Wed, 30 Mar 2016 14:31:33 GMT\r\n"
        "Session: e685a29b5421ead7;timeout=123\r\n"
        "Transport: RTP/AVP/TCP;interleaved=123-456\r\n"
        "\r\n";

    Surge::Response wrapped_response((unsigned char *)raw_response.c_str(), raw_response.length());
    Surge::SetupResponse response(&wrapped_response);

    ASSERT_EQ(response.GetRtpInterleavedChannel(), 123);
    ASSERT_EQ(response.GetRtcpInterleavedChannel(), 456);
}

TEST(SETUP_RESPONSE, TestThatSessionTokenCanBeParsedAlongWithTimeout) {
    std::string raw_response =
        "RTSP/1.0 200 OK\r\n"
        "CSeq: 2\r\n"
        "Date: Wed, 30 Mar 2016 14:31:33 GMT\r\n"
        "Session: e685a29b5421ead7;timeout=123\r\n"
        "Transport: RTP/AVP/TCP;interleaved=123-456\r\n"
        "\r\n";

    Surge::Response wrapped_response((unsigned char *)raw_response.c_str(), raw_response.length());
    Surge::SetupResponse response(&wrapped_response);

    ASSERT_STREQ(response.GetSession().c_str(), "e685a29b5421ead7");
}

TEST(SETUP_RESPONSE, TestThatSessionTokenCanBeParsed) {
    std::string raw_response =
        "RTSP/1.0 200 OK\r\n"
        "CSeq: 2\r\n"
        "Date: Wed, 30 Mar 2016 14:31:33 GMT\r\n"
        "Session: e685a29b5421ead7\r\n"
        "Transport: RTP/AVP/TCP;interleaved=123-456\r\n"
        "\r\n";

    Surge::Response wrapped_response((unsigned char *)raw_response.c_str(), raw_response.length());
    Surge::SetupResponse response(&wrapped_response);

    ASSERT_STREQ(response.GetSession().c_str(), "e685a29b5421ead7");
}
