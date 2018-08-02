//
//  SecureInterleavedTCPTransport.cc
//  SurgeCore
//
//  Created by Paul Shields on 09/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "SecureInterleavedTCPTransport.h"

#include "Logging.h"

Surge::SecureInterleavedTCPTransport::SecureInterleavedTCPTransport(ITLSClient *tlsClient, ISocketHandlerDelegate *delegate, TransportDelegate *test) : tlsClient(tlsClient), Surge::InterleavedRtspTransport(delegate, test) { }

Surge::SecureInterleavedTCPTransport::~SecureInterleavedTCPTransport() {
    if (tlsClient != nullptr) {
        delete tlsClient;
    }
}

void Surge::SecureInterleavedTCPTransport::RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) {
    INFO("Starting TLS Client");
    tlsClient->StartClient(this);

    Transport::RtspTcpOpen(host, port, [&, callback] (int result) {
        if (result == 0) {
            INFO("Opening TLS connection");
            tlsClient->OpenTLSConnection([&, callback](Surge::TLSStatus result) {
                callback(result);
            });
        }
    });
}

/** TODO: Find a way to make more generic; leave writing to the socket to the superclass somehow */
void Surge::SecureInterleavedTCPTransport::RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) {
    DEBUG("Sending TLS encrypted RTSP command to server");

    rtspCallback = callback;

    auto response = tlsClient->EncryptData((char *) command->BytesPointer(), command->PointerLength());
    if (response.StatusCode() == ERROR) {
        ERROR("Failed to encrypt data; skipping transaction.");
        return;
    }

    SafeRunLibuvCommand([&]() {
        m_tcp->write(GenerateRtspDataPtr(response.Data(), response.Size()),
                    response.Size());
        m_tcp->read();
    });
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
