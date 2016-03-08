#include <gtest/gtest.h>

#include "Surge.h"
#include "TestUtil.h"
#include "Logging.h"
#include "RtspClient.h"
#include "DelegateInterface.h"
#include "NaluParser.h"
#include <thread>
#include <chrono>

#include "Cameras.h"

#include <cstdio>

class Delegate: public Surge::RtspClientDelegate {
public:

    void ClientDidTimeout() {
        
    }

    void Payload(const unsigned char* buffer, size_t length) {
        // print out nalus
        SurgeTestUtil::PrintOutAllNaluTypes(buffer, length);
    }
    
};


TEST(SCRATCH, SIMPLE_SCRATCH) {

    Surge::StartErrorDispatcher();
    SurgeTestUtil::SetupTestLogger();
    
    INFO("READY TO STREAM");

    Delegate delegate;
    Surge::RtspClient client(&delegate);
    
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

    Surge::RtspResponse* play_response = client.Play(false);
    delete play_response;

    // sleep
    std::this_thread::sleep_for (std::chrono::seconds(10));
    
    client.StopClient();

    Surge::CloseErrorDispatcher();
}
