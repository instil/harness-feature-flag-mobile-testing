#include "RtspClient.h"
#include "Logging.h"

Surge::RtspClient::RtspClient() {
    
}

Surge::RtspClient::~RtspClient() {
    
}

int Surge::RtspClient::Describe(const std::string url,
                                bool requires_auth,
                                const std::string user,
                                const std::string password) {
    INFO("DESCRIBE: " << url);

    
    
    return 0;
}

void Surge::RtspClient::StopClient() {
        
}

