#include "RtspClient.h"
#include "RtspCommandFactory.h"
#include "Logging.h"
#include "Url.h"

Surge::RtspClient::RtspClient() {
    
}

Surge::RtspClient::~RtspClient() {
    
}

Surge::DescribeResponse* Surge::RtspClient::Describe(const std::string url,
                                                     bool requires_auth,
                                                     const std::string user,
                                                     const std::string password) {
    SurgeUtil::Url url_model(url);
    std::string host = url_model.GetHost();
    int port = url_model.GetPort();
    int retval = m_socketHandler.RtspTcpOpen(host, port);
    if (retval != 0) {
        ERROR("Failed to open [rtsp://" << host << ":" << port << "]");
        return nullptr;
    }

    m_socketHandler.StartRunning();
    if (requires_auth) {
        RtspCommandFactory::SetBasicAuthCredentials(user.c_str(), password.c_str());
    }

    RtspCommand* describe = RtspCommandFactory::DescribeRequest(url, 0, true);
    Response* raw_resp = m_socketHandler.RtspTransaction(describe, true);
    delete describe;
    
    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to describe!");
        return nullptr;
    }

    INFO("DESCRIBE RESP: " << raw_resp->StringDump());
    
    Surge::DescribeResponse *resp = new Surge::DescribeResponse(raw_resp);
    delete raw_resp;

    return resp;
}

void Surge::RtspClient::StopClient() {
    
    m_socketHandler.StopRunning();
    
}

