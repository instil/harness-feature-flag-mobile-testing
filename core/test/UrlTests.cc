#include <gtest/gtest.h>

#include "Url.h"

TEST(URL, TestThatThePortCanBeParsed) {
    const char *test_url = "http://myurl:1234";

    SurgeUtil::Url url(test_url);
    ASSERT_EQ(1234, url.GetPort());
}

TEST(URL, TestThatHostNameCanBeParsed) {
    const char *test_url = "http://myurl:1234";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("myurl", url.GetHost().c_str());
}

TEST(URL, TestThatTheSchemeCanBeParsed) {
    const char *test_url = "http://myurl:1234/1/2/3/4";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("http://", url.GetScheme().c_str());
}

TEST(URL, TestThatPathsCanBeParsed) {
    const char *test_url = "http://myurl:1234/1/2/3/4";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("/1/2/3/4", url.GetPath().c_str());
}

TEST(URL, TestThatQueryParametersCanBeParsedFromUrlsWithAPath) {
    const char *test_url = "http://myurl:1234/1/2/3/4?myquery=1";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("/1/2/3/4", url.GetPath().c_str());
}

TEST(URL, TestThatQueryParametersCanBeParsed) {
    const char *test_url = "http://myurl:1234/1/2/3/4?myquery=1";

    SurgeUtil::Url url(test_url);

    ASSERT_STREQ("1", url.GetQueryParameters()["myquery"].c_str());
}

TEST(URL, TestThatHostCanBeParsedFromUrlsWithAuthCredentials) {
    const char *test_url = "rtsp://user:pass@192.168.1.1:8080/path";

    SurgeUtil::Url url(test_url);
    ASSERT_STREQ("192.168.1.1", url.GetHost().c_str());
}

TEST(URL, TestThatParsingAnInvalidSchemeResultsInAnException) {
    const char *test_url = "http:/myurl:1234";
    SurgeUtil::Url url(test_url);
    try {
        url.GetScheme();
    } catch (std::exception& ex) {
        EXPECT_STREQ("The supplied URL was malformed: scheme could not be parsed", ex.what());
    }
}
