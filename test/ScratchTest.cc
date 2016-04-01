#include <gtest/gtest.h>

#include "Surge.h"
#include "TestUtil.h"
#include "Logging.h"
#include "RtspClient.h"
#include "DelegateInterface.h"
#include "NaluParser.h"
#include <thread>
#include <chrono>

#include <cstdio>

class Delegate: public Surge::RtspClientDelegate {
public:

    void ClientDidTimeout() {
        ERROR("TIMEOUT");
    }

    void StreamConfigChanged(bool wasRedirect) {
        
    }

    void Payload(const char* buffer, size_t length) {
        // print out nalus
        INFO("PAYLOAD");
        SurgeTestUtil::PrintOutAllNaluTypes((const unsigned char*)buffer, length);
    }
    
};


TEST(SCRATCH, DISABLED_SIMPLE_SCRATCH) {
    
    Surge::StartErrorDispatcher();
    SurgeTestUtil::SetupTestLogger();

    Delegate delegate;
    Surge::RtspClient client(&delegate);
    
    Surge::DescribeResponse *describe_response = client.Describe("rtsp://localhost:8554/test.264",
                                                                 false, "", "");
    
    if (describe_response == nullptr) {
        ERROR("Server is unreachable!");
        return;
    }

    if (!describe_response->Ok()) {
        delete describe_response;
        ERROR("Invalid describe response!");
        return;
    }
    
    Surge::SessionDescription palette = describe_response->GetPalettes()[0];
    delete describe_response;

    Surge::RtspResponse* setup_response = client.Setup(palette, false);
    delete setup_response;

    Surge::RtspResponse* play_response = client.Play(false);
    delete play_response;

    // sleep
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // stop
    client.StopClient();

    // close up
    Surge::CloseErrorDispatcher();
}
