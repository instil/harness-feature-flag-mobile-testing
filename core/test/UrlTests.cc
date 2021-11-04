#include <gtest/gtest.h>

#include "Url.h"

TEST(URL, TestThatThePortCanBeParsed) {
    const char *test_url = "rtsp://myurl:1234";

    SurgeUtil::Url url(test_url);
    ASSERT_EQ(1234, url.GetPort());
}

TEST(URL, TestThatHostNameCanBeParsed) {
    const char *test_url = "rtsp://myurl:1234";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("myurl", url.GetHost().c_str());
}

TEST(URL, TestThatTheSchemeCanBeParsed) {
    const char *test_url = "rtsp://myurl:1234/1/2/3/4";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("rtsp", url.GetProtocol().c_str());
}

TEST(URL, TestThatPathsCanBeParsed) {
    const char *test_url = "rtsp://myurl:1234/1/2/3/4";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("1/2/3/4", url.GetPath().c_str());
}

TEST(URL, TestThatUrlsWithoutAuthCredentialsDoNotContainCredentials) {
    const char *test_url = "rtsp://myurl:1234/1/2/3/4";

    SurgeUtil::Url url(test_url);
    ASSERT_FALSE(url.ContainsCredentials());
}

TEST(URL, TestThatUrlsWithAuthCredentialsContainCredentials) {
    const char *test_url = "rtsp://user:pass@192.168.1.1:8080/path";

    SurgeUtil::Url url(test_url);
    ASSERT_TRUE(url.ContainsCredentials());
}

TEST(URL, TestThatHostCanBeParsedFromUrlsWithAuthCredentials) {
    const char *test_url = "rtsp://user:pass@192.168.1.1:8080/path";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("192.168.1.1", url.GetHost().c_str());
}

TEST(URL, TestThatCredentialsCanBeParsedFromUrlsWithAuthCredentials) {
    const char *test_url = "rtsp://user:pass@192.168.1.1:8080/path";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("user", url.GetUsername().c_str());
    ASSERT_STREQ("pass", url.GetPassword().c_str());
}

TEST(URL, TestThatVeryLongUrlCanBeCorrectlyParsed) {
    const char *test_url = "rtsps://stream-ue1-bravo.dropcam.com:443/sdm_live_stream/CiQA2vuxrwSdq8xV8wb7ypchRnBgj9eLFJ5VKzFYGHQCfxEtb8cScQBBuHgoDVDWfN6XOLzR5p2hx7TK78qMLJ2ThtfRrfvSBhokGnY7GrFlfsH_rh8TPghOeW1VdtfsauFk2n_kHsjsndE4uyymMtCLcydS9mgWdF_WXt_Sknkab-ZoB-3UK11qEQknXIQuWlE1US0Rs3YS?auth=g.0.eyJraWQiOiIyMzhiNTUxZ";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("stream-ue1-bravo.dropcam.com", url.GetHost().c_str());
    ASSERT_EQ(443, url.GetPort());
    ASSERT_STREQ("sdm_live_stream/CiQA2vuxrwSdq8xV8wb7ypchRnBgj9eLFJ5VKzFYGHQCfxEtb8cScQBBuHgoDVDWfN6XOLzR5p2hx7TK78qMLJ2ThtfRrfvSBhokGnY7GrFlfsH_rh8TPghOeW1VdtfsauFk2n_kHsjsndE4uyymMtCLcydS9mgWdF_WXt_Sknkab-ZoB-3UK11qEQknXIQuWlE1US0Rs3YS", url.GetPath().c_str());
}

// Known issue, to be fixed in next PR.
TEST(URL, DISABLED_TestThatParsingAnInvalidSchemeResultsInAnException) {
    const char *test_url = "rtsp:/myurl:1234";
    SurgeUtil::Url url(test_url);
    try {
        url.GetProtocol();
    } catch (std::exception& ex) {
        EXPECT_STREQ("The supplied URL was malformed: scheme could not be parsed", ex.what());
    }
}
