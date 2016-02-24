#include "RtspClient.h"
#include "Logging.h"

Surge::RtspClient::RtspClient() {
    
}

Surge::RtspClient::~RtspClient() {
    
}

int Surge::RtspClient::Describe(const std::string url) {
    INFO("DESCRIBE: " << url);
    return 0;
}

void Surge::RtspClient::StopClient() {
        
}

