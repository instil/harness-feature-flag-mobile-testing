#include <gtest/gtest.h>

#include "Surge.h"
#include "TestUtil.h"
#include "Logging.h"
#include "RtspClient.h"
#include "IRtspClientDelegate.h"
#include "NaluParser.h"
#include <thread>
#include <chrono>

#include <cstdio>

class Delegate: public Surge::IRtspClientDelegate {
public:

    void ClientDidTimeout() {
        ERROR("TIMEOUT");
    }

    void StreamConfigChanged(bool wasRedirect) {
        
    }

    virtual void ClientReceivedFrame(const unsigned char *frameBuffer,
                                     size_t length,
                                     int32_t width,
                                     int32_t height,
                                     int32_t presentationTime,
                                     int32_t duration) {
        // print out nalus
        INFO("PAYLOAD");
        SurgeTestUtil::PrintOutAllNaluTypes((const unsigned char*)frameBuffer, length);
    }
    
    virtual void ClientReceivedExtendedHeader(const unsigned char * buffer,
                                              size_t length) {
        INFO("EXTENDED HEADER RECEIVED");
    }
    
};
