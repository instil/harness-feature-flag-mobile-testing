//
//  SecureInterleavedTCPTransport.h
//  SurgeCore
//
//  Created by Paul Shields on 09/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef SecureInterleavedTCPTransport_h
#define SecureInterleavedTCPTransport_h

#include "InterleavedRtspTransport.h"

#include "TLSClient.h"

namespace Surge {
    class SecureInterleavedTCPTransport : public InterleavedRtspTransport {
    public:
        SecureInterleavedTCPTransport(ISocketHandlerDelegate *delegate, TransportDelegate *test);
        ~SecureInterleavedTCPTransport();

        void RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) override;
        void RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) override;
        void RtspHandleReceive(const char* buffer, size_t size) override;


    public:
        TLSClient *tlsClient;
    };
}


#endif /* SecureInterleavedTCPTransport_h */
