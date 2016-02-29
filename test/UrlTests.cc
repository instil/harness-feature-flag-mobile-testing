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
