//
//  ITLSClient.h
//  SurgeCore
//
//  Created by Paul Shields on 24/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef ITLSClient_h
#define ITLSClient_h

#include <functional>

#include "TLSResponse.h"
#include "ITransportInterface.h"

namespace Surge {
    class ITLSClient {

    public:
        virtual ~ITLSClient() { };

        virtual void StartClient(ITransportInterface *transport) = 0;
        virtual void StopClient() = 0;
        virtual void OpenTLSConnection(std::function<void(TLSStatus)> callback) = 0;

        virtual Surge::TLSResponse DecryptData(const char* data, size_t size) = 0;
        virtual Surge::TLSResponse EncryptData(const char* data, size_t size) = 0;

        virtual void SetTrustedCertificate(const std::string& fileUri) = 0;

    };
}

#endif /* ITLSClient_h */
