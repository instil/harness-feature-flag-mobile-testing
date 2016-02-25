#include <gtest/gtest.h>

#include "TestUtil.h"
#include "Logging.h"

#include <cstdio>

TEST(SCRATCH, SIMPLE_SCRATCH) {
    printf("1\n");
    SurgeTestUtil::SetupTestLogger();
    printf("2\n");
    INFO("READY TO STREAM");
    printf("3\n");
}
