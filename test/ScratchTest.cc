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
    
    // auto resp = client.Describe(CAMERA_URL, true, CAMERA_USER, CAMERA_PASS);
    // auto resp = client.Describe("http://localhost:8888/live.h264", false, "", "");
    
    Surge::DescribeResponse *resp = client.Describe("rtsp://localhost:8554/test.264",
                                                    false, "", "");
    delete resp;
    
    // OptionsResponse* resp = client.Options();


    client.StopClient();
}
