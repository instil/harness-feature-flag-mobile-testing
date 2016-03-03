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
    
    // Surge::DescribeResponse *describe_response = client.Describe(CAMERA_URL, true, CAMERA_USER, CAMERA_PASS);
    Surge::DescribeResponse *describe_response = client.Describe("rtsp://localhost:8554/test.264", false, "", "");
    Surge::SessionDescription palette = describe_response->GetPalettes()[0];

    delete describe_response;
    
    Surge::RtspResponse *options_response = client.Options();
    delete options_response;

    Surge::RtspResponse* setup_response = client.Setup(palette);
    delete setup_response;

    Surge::RtspResponse* play_response = client.Play();
    delete play_response;

    client.StopClient();
}
