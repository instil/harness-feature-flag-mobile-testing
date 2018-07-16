//
//  OpenSSLService.cc
//  SurgeCore
//
//  Created by Paul Shields on 22/06/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "TLSClient.h"

#include "openssl/crypto.h"

#include <math.h>

#include "Logging.h"

Surge::TLSClient::TLSClient() :
    sslContext(nullptr),
    ssl(nullptr),
    appBio(nullptr),
    openSSLBio(nullptr),
    transport(nullptr) { }

Surge::TLSClient::~TLSClient() {
    StopClient();
}

void Surge::TLSClient::StartClient(ITransportInterface *transport) {
    this->transport = transport;

    InitializeOpenSSL();
    GenerateSSLContext();
    SetupSSL();
}

void Surge::TLSClient::StopClient() {
    if (ssl != nullptr) {
        SSL_shutdown(ssl);
        BIO_free(appBio);
        BIO_free(openSSLBio);
        SSL_CTX_free(sslContext);
        SSL_free(ssl);

        ssl = nullptr;
        sslContext = nullptr;
        appBio = nullptr;
        openSSLBio = nullptr;
    }
}

void Surge::TLSClient::InitializeOpenSSL() {
    // Generic Initialization
    (void)SSL_library_init();
    SSL_load_error_strings();

    OPENSSL_init_crypto(0, NULL);
}

void Surge::TLSClient::GenerateSSLContext() {
    // Support all TLS/SSL versions?
    const SSL_METHOD* method = SSLv23_method();
    if (method == NULL) {
        ERROR("Error?");
    }

    // Creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections.
    sslContext = SSL_CTX_new(method);
    if (sslContext == NULL) {
        ERROR("Error?");
    }
    

    // Verifty vertificate callback
    SSL_CTX_set_verify(sslContext, SSL_VERIFY_PEER, VerifyCertificate);
    SSL_CTX_set_verify_depth(sslContext, 4);

    //
    SSL_CTX_set_read_ahead(sslContext, true);

    // Disables SSL & compression support
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(sslContext, flags);
}

void Surge::TLSClient::SetupSSL() {
    // Creates a new SSL structure which is needed to hold the data for a TLS/SSL connection
    ssl = SSL_new(sslContext);

    // Set SSL object to client mode
    SSL_set_connect_state(ssl);

    // BIO_s_bio() returns the method for a BIO pair. A BIO pair is a pair of source/sink BIOs
    // where data written to either half of the pair is buffered and can be read from the other half.
    // Very useful: https://www.openssl.org/docs/man1.1.0/crypto/BIO_s_bio.html
    BIO_new_bio_pair(&appBio, 0, &openSSLBio, 0);

    // Connects the BIOs rbio and wbio for the read and write operations of the TLS/SSL (encrypted) side of ssl.
    SSL_set_bio(ssl, openSSLBio, openSSLBio);
}

int Surge::TLSClient::VerifyCertificate(int preverify, X509_STORE_CTX* x509_ctx) {
    // Always verify certificate - bad idea!
    return 1;
}

void Surge::TLSClient::OpenTLSConnection(std::function<void(TLSStatus)> callback) {
    sslConnectedResponse = callback;

    RunTLSHandshake();
}

void Surge::TLSClient::RunTLSHandshake() {
    // Initiates handshake, when in client mode.
    int result = SSL_do_handshake(ssl);
    INFO("RV: " << result);

    if (InterpretSSLResult(ssl, result) == WRITE_TO_TRANSPORT) {
        int handshakeDataLength = BIO_pending(appBio);

        char *handshakeTransaction = (char*)malloc(handshakeDataLength);
        int readLength = BIO_read(appBio, handshakeTransaction, handshakeDataLength);

        transport->ArbitraryDataTransaction(handshakeTransaction, handshakeDataLength);

        free(handshakeTransaction);
    }
}

Surge::TLSResponse Surge::TLSClient::DecryptData(const char* data, size_t size) {
    if (!SSL_is_init_finished(ssl)) {
        // Handshaking not complete yet, check if we need to send anything else to the socket
        if (ContinueTLSHandshake(data, size)) {
            sslConnectedResponse(CONNECTED);
        }

        return TLSResponse(NO_RESPONSE);
    }

    /**
     Standard data decryption

     Notes about this logic:

     - The input buffer may contain 1 or more packets of data.
     - BIO_write has a maxsize of 1 max sized TLS packet.
        - Thus need to loop to ensure all input data is consumed.
     - SSL_read will only read 1 decrypted packet of data off at a time.
        - Need to loop to read all written packets off and manually append them
          into a single output buffer.
     - There is no way to determine the size of buffer required for SSL_read before execution.
        - Need to be able to resize the output buffer on the fly.
        - SSL_pending will tell you the amount of data left for the last decrypted packet left,
          so we resize the buffer and look to read off the rest of the decrypted packet.
        - Of course, there may be another packet to decrypt after this, so we loop again, try a
          read with a buffer of 0 length, get the amount of data that needs read, then resize
          and loop again...
     */

    std::vector<char> *tlsDataBuffer = new std::vector<char>(size);

    size_t tlsDataBufferSize = 0;
    size_t bytesWrittenToBio = 0;
    do {
        int bioWriteResult = BIO_write(appBio, data + bytesWrittenToBio, (int)(size - bytesWrittenToBio));
        if (bioWriteResult > 0) {
            bytesWrittenToBio += bioWriteResult;
        }

        int sslReadResult = 0;
        do {
            sslReadResult = SSL_read(ssl,
                                     tlsDataBuffer->data() + tlsDataBufferSize,
                                     std::max(tlsDataBuffer->size() - tlsDataBufferSize, (size_t)0));

            if (SSL_pending(ssl) > 0) {
                tlsDataBuffer->resize(tlsDataBuffer->size() + SSL_pending(ssl));
            }

            if (sslReadResult > 0) {
                tlsDataBufferSize += sslReadResult;
            }

            if (InterpretSSLResult(ssl, sslReadResult) == TLS_ERROR) {
                return TLSResponse(ERROR);
            }
        } while (sslReadResult > 0 || SSL_pending(ssl) > 0);

    } while (bytesWrittenToBio < size);

    return TLSResponse(DECRYPTED_DATA_AVAILABLE, tlsDataBuffer, tlsDataBufferSize);
}

bool Surge::TLSClient::ContinueTLSHandshake(const char* data, size_t size) {
    BIO_write(appBio, data, (int)size);

    int handshakeStatus = SSL_do_handshake(ssl);

    if (InterpretSSLResult(ssl, handshakeStatus) == OK) {
        INFO("Handshake complete");
        return true;
    } else {
        RunTLSHandshake();
    }

    return false;
}

Surge::TLSResponse Surge::TLSClient::EncryptData(const char* data, size_t size) {
    SSL_write(ssl, data, size);

    int pendingDataLength = BIO_pending(appBio);

    if (pendingDataLength > 0) {
        char *tlsDataBuffer = (char*) malloc(pendingDataLength);
        size_t tlsDataBufferSize = pendingDataLength;

        BIO_read(appBio, tlsDataBuffer, pendingDataLength);

        return TLSResponse(ENCRYPTED_DATA_AVAILABLE, tlsDataBuffer, tlsDataBufferSize);
    }

    return TLSResponse(ERROR);
}
