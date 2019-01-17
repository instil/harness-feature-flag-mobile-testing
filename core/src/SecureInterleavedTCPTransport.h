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

#include "ITLSClient.h"

namespace Surge {
    class SecureInterleavedTCPTransport : public InterleavedRtspTransport, public ISecureTransport {
    public:
        SecureInterleavedTCPTransport(ITLSClient *tlsClient, TransportDelegate * const transportDelegate, ISocketHandlerDelegate *delegate);

        void RtspTcpOpen(SurgeUtil::Url &url, std::function<void(int)> callback) override;
        void RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) override;
        void RtspHandleReceive(const char* buffer, size_t size) override;
        void ArbitraryDataTransaction(const char *data, const size_t length) override;

        void UnencryptedArbitraryDataTransaction(const char *data, const size_t length) override;


    public:
        ITLSClient *tlsClient;
    };
}


#endif /* SecureInterleavedTCPTransport_h */
