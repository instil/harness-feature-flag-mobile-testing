//
//  SecureInterleavedTCPTransport.cc
//  SurgeCore
//
//  Created by Paul Shields on 09/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "SecureInterleavedTCPTransport.h"

#include "Logging.h"

Surge::SecureInterleavedTCPTransport::SecureInterleavedTCPTransport(ITLSClient *tlsClient, TransportDelegate * const transportDelegate, ISocketHandlerDelegate *delegate, long long rtspTimeoutMs) : tlsClient(tlsClient), Surge::InterleavedRtspTransport(transportDelegate, delegate, rtspTimeoutMs) { }

void Surge::SecureInterleavedTCPTransport::RtspTcpOpen(SurgeUtil::Url &url, std::function<void(int)> callback) {
    INFO("Starting TLS Client");
    tlsClient->StartClient(this);

    Transport::RtspTcpOpen(url, [&, callback] (int result) {
        if (result == 0) {
            INFO("Opening TLS connection");
            tlsClient->OpenTLSConnection([&, callback](Surge::TLSStatus result) {
                callback(result);
            });
        }
    });
}

void Surge::SecureInterleavedTCPTransport::RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) {
    DEBUG("Sending TLS encrypted RTSP command to server");

    rtspCallback = callback;

    auto response = tlsClient->EncryptData((char *) command->BytesPointer(), command->PointerLength());
    if (response.StatusCode() == ERROR) {
        ERROR("Failed to encrypt data; skipping transaction.");
        return;
    }

    Transport::RtspTransaction(response.Data(), response.Size(), callback);
}

void Surge::SecureInterleavedTCPTransport::RtspHandleReceive(const char* buffer, size_t size) {
    auto result = tlsClient->DecryptData(buffer, size);

    if (result.StatusCode() == DECRYPTED_DATA_AVAILABLE) {
        Transport::RtspHandleReceive(result.Data(), result.Size());
    }
}

void Surge::SecureInterleavedTCPTransport::ArbitraryDataTransaction(const char *data, const size_t length) {
    auto response = tlsClient->EncryptData(data, length);
    if (response.StatusCode() == ERROR) {
        ERROR("Failed to encrypt data; skipping transaction.");
        return;
    }

    InterleavedRtspTransport::ArbitraryDataTransaction(response.Data(), response.Size());
}

void Surge::SecureInterleavedTCPTransport::UnencryptedArbitraryDataTransaction(const char *data, const size_t length) {
    InterleavedRtspTransport::ArbitraryDataTransaction(data, length);
}
