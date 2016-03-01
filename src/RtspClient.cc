#include "RtspClient.h"
#include "RtspCommandFactory.h"
#include "Logging.h"
#include "Url.h"

Surge::RtspClient::RtspClient() {
    
}

Surge::RtspClient::~RtspClient() {
    
}

int Surge::RtspClient::Describe(const std::string url,
                                bool requires_auth,
                                const std::string user,
                                const std::string password) {
    SurgeUtil::Url url_model(url);
    std::string host = url_model.GetHost();
    int port = url_model.GetPort();
    int retval = m_socketHandler.RtspTcpOpen(host, port);
    if (retval != 0) {
        ERROR("Failed to open [rtsp://" << host << ":" << port << "]");
        return -1;
    }

    if (requires_auth) {
        RtspCommandFactory::SetBasicAuthCredentials(user.c_str(), password.c_str());
    }

    RtspCommand* describe = RtspCommandFactory::DescribeRequest(url, 0, true);
    //Response* resp = m_socketHandler.RtspTransaction(describe, true);

    delete describe;
    
    return 0;
}

void Surge::RtspClient::StopClient() {
        
}

