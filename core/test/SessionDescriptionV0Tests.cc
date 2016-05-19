#include <gtest/gtest.h>

#include "SessionDescriptionV0.h"

TEST(SESSION_DESCRIPTION_V0, TestThatControlUrlIsParsed) {
    std::string raw_sdp =
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0;\r\n";

    Surge::SessionDescriptionV0 sdp(raw_sdp);

    ASSERT_STREQ(sdp.GetControl().c_str(), "streamid=0");
}

TEST(SESSION_DESCRIPTION_V0, TestThatRtpMapIsParsed) {
    std::string raw_sdp =
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0;\r\n";

    Surge::SessionDescriptionV0 sdp(raw_sdp);

    ASSERT_STREQ(sdp.GetRtpMap().c_str(), "a=rtpmap:96 H264/90000");
}

TEST(SESSION_DESCRIPTION_V0, TestThatFmtpIsParsed) {
    std::string raw_sdp =
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0;\r\n";

    Surge::SessionDescriptionV0 sdp(raw_sdp);

    ASSERT_STREQ(sdp.GetFmtp().c_str(),
                 "a=fmtp:96 packetization-mode=1; "
                 "sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029");
}

TEST(SESSION_DESCRIPTION_V0, TestThatIncompleteUrlReturnsIncompleteFlag) {
    std::string raw_sdp =
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0;\r\n";

    Surge::SessionDescriptionV0 sdp(raw_sdp);

    ASSERT_EQ(sdp.IsControlUrlComplete(), false);
}

TEST(SESSION_DESCRIPTION_V0, TestThatH264SdpParsesAsCorrectMimeType) {
    std::string raw_sdp =
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0;\r\n";

    Surge::SessionDescriptionV0 sdp(raw_sdp);

    ASSERT_EQ(sdp.GetType(), Surge::H264);
}

TEST(SESSION_DESCRIPTION_V0, TestThatH264SdpParsesParameters) {
    std::string raw_sdp =
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0;\r\n";

    Surge::SessionDescriptionV0 sdp(raw_sdp);

    ASSERT_STREQ(sdp.GetFmtpH264ConfigParameters().c_str(), "Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==");
}

TEST(SESSION_DESCRIPTION_V0, TestThatInvalidMimeIsUnknown) {
    std::string raw_sdp =
        "v=0\r\n"
        "o=- 0 0 IN IP4 127.0.0.1\r\n"
        "s=No Title\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "t=0 0\r\n"
        "a=tool:libavformat 56.40.101\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 invalid-mime/90000\r\n"
        "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKaw05QFAFuhAAZdOwExLQCPGDEWA,aO68sA==; profile-level-id=640029\r\n"
        "a=control:streamid=0;\r\n";

    Surge::SessionDescriptionV0 sdp(raw_sdp);

    ASSERT_EQ(sdp.GetType(), Surge::UNKNOWN);
}

