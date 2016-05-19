#include <gtest/gtest.h>

#include "Url.h"

TEST(URL, TestThatUrlCanParsePort) {
    const char *test_url = "http://myurl:1234";

    SurgeUtil::Url url(test_url);
    ASSERT_EQ(url.GetPort(), 1234);
}

TEST(URL, TestThatUrlCanParseHost) {
    const char *test_url = "http://myurl:1234";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ(url.GetHost().c_str(), "myurl");
}

TEST(URL, TestThatUrlCanParseScheme) {
    const char *test_url = "http://myurl:1234/1/2/3/4";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ(url.GetScheme().c_str(), "http");
}

TEST(URL, TestThatUrlCanParsePath) {
    const char *test_url = "http://myurl:1234/1/2/3/4";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ(url.GetPath().c_str(), "/1/2/3/4");
}

TEST(URL, TestThatUrlCanParsePathWithQueries) {
    const char *test_url = "http://myurl:1234/1/2/3/4?myquery=1";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ(url.GetPath().c_str(), "/1/2/3/4");
}

TEST(URL, TestThatUrlCanParseQueries) {
    const char *test_url = "http://myurl:1234/1/2/3/4?myquery=1";

    SurgeUtil::Url url(test_url);

    ASSERT_STREQ(url.GetQueryParameters()["myquery"].c_str(), "1");
}

TEST(URL, TestThatUrlCanHandleAuthCredentials) {
    const char *test_url = "rtsp://user:pass@192.168.1.1:8080/path";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ(url.GetHost().c_str(), "192.168.1.1");
}
