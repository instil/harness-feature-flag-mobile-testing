#include "RtspClient.h"
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
    INFO("DESCRIBE: " << url);
    
    SurgeUtil::Url urlModel(url);

    std::string host = urlModel.GetHost();
    int port = urlModel.GetPort();
    std::string scheme = urlModel.GetScheme();
    std::string path = urlModel.GetPath();
    auto params = urlModel.GetQueryParameters();

    INFO("HOST: [" << host << "]");
    INFO("PORT: [" << port << "]");
    INFO("SCHEME: [" << scheme << "]");
    INFO("PATH: [" << path << "]");
    
    return 0;
}

void Surge::RtspClient::StopClient() {
        
}

