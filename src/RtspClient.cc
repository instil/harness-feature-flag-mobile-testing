#include "RtspClient.h"
#include "RtspCommandFactory.h"
#include "Logging.h"
#include "Url.h"

Surge::RtspClient::RtspClient() : m_keeepAliveIntervalInSeconds(60),
                                  m_sequenceNumber(1),
                                  m_url(),
                                  m_session(),
                                  m_socketHandler()
{
    
}

Surge::RtspClient::~RtspClient() {
    m_socketHandler.StopRunning();
}

Surge::DescribeResponse* Surge::RtspClient::Describe(const std::string url,
                                                     bool requires_auth,
                                                     const std::string user,
                                                     const std::string password) {
    m_url = url;
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

    RtspCommand* describe = RtspCommandFactory::DescribeRequest(url, GetNextSequenceNumber(), true);
    Response* raw_resp = m_socketHandler.RtspTransaction(describe, true);
    delete describe;
    
    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to DESCRIBE!");
        return nullptr;
    }
    
    DescribeResponse *resp = new DescribeResponse(raw_resp);
    delete raw_resp;

    return resp;
}

Surge::SetupResponse* Surge::RtspClient::Setup() {
    RtspCommand* setup = RtspCommandFactory::SetupRequest(m_url + "/track1", GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(setup, true);
    delete setup;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to SETUP!");
        return nullptr;
    }

    SetupResponse* resp = new SetupResponse(raw_resp);
    delete raw_resp;
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Options() {    
    RtspCommand* options = RtspCommandFactory::OptionsRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(options, true);
    delete options;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to OPTIONS!");
        return nullptr;
    }

    RtspResponse* resp = new RtspResponse(raw_resp);
    delete raw_resp;

    return resp;
}

void Surge::RtspClient::StopClient() {
    
    m_socketHandler.StopRunning();
    
}

