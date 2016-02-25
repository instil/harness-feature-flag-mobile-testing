#include <gtest/gtest.h>

#include "TestUtil.h"
#include "Logging.h"
#include "RtspClient.h"

#include "Cameras.h"

#include <cstdio>

TEST(SCRATCH, SIMPLE_SCRATCH) {
    SurgeTestUtil::SetupTestLogger();
    
    INFO("READY TO STREAM");

    Surge::RtspClient client;
    int return_code = client.Describe(CAMERA_URL, true, CAMERA_USER, CAMERA_PASS);

    client.StopClient();
}
