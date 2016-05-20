#include <gtest/gtest.h>

#include "Surge.h"
#include "TestUtil.h"
#include "Logging.h"
#include "RtspClient.h"
#include "IRtspClientDelegate.h"
#include "SessionDescriptionFactory.h"
#include "NaluParser.h"

#include <thread>
#include <chrono>

#include <cstdio>

class Delegate: public Surge::IRtspClientDelegate {
public:

    ~Delegate() {
        if (m_palettes) {   
            delete m_palettes;
        }
    }

    void ClientDidTimeout() {
        ERROR("TIMEOUT");
    }

    void StreamConfigChanged(bool wasRedirect) { }

    void OnSessionDescription(const Surge::RawSessionDescription& description)  {
        m_palettes = Surge::SessionDescriptionFactory::ParseSessionDescriptionsFromBuffer(description);
    }

    Surge::SessionDescription GetBestSession() {
        Surge::SessionDescription palette = m_palettes->at(0);
        return palette;
    }

    void Payload(const unsigned char* buffer, size_t length,
                 const unsigned char *extension_header, size_t extension_length) {
        // print out nalus
        INFO("PAYLOAD");
        SurgeTestUtil::PrintOutAllNaluTypes((const unsigned char*)buffer, length);
    }

private:
    std::vector<Surge::SessionDescription> *m_palettes;
};


TEST(SCRATCH, /*DISABLED_*/SIMPLE_SCRATCH) {
    
    Surge::StartErrorDispatcher();
    SurgeTestUtil::SetupTestLogger();

    Delegate delegate;
    Surge::RtspClient client(&delegate);
    
    Surge::DescribeResponse *describe_response = client.Describe("rtsp://localhost:8554/test.264");
    
    if (describe_response == nullptr) {
        ERROR("Server is unreachable!");
        return;
    }

    if (!describe_response->Ok()) {
        delete describe_response;
        ERROR("Invalid describe response!");
        return;
    }
    delete describe_response;

    Surge::RtspResponse* setup_response = client.Setup(false);
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
