//
//  OpenSSLService.h
//  SurgeCore
//
//  Created by Paul Shields on 22/06/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef OpenSSLService_hpp
#define OpenSSLService_hpp

#include "openssl/ssl.h"
#include "openssl/err.h"

#include <functional>
#include <vector>

#include "ITransportInterface.h"

namespace Surge {
    typedef enum {
        CONNECTED = 0,
        DECRYPTED_DATA_AVAILABLE = 1,
        ENCRYPTED_DATA_AVAILABLE = 2,
        NO_RESPONSE = 3,
        ERROR = -1
    } TLSStatus;

    struct TLSResponse {
    private:
        Surge::TLSStatus statusCode;
        std::vector<char> *resizableDataBuffer;
        char *dataBuffer;
        size_t size;

    public:
        TLSResponse(Surge::TLSStatus statusCode, std::vector<char> *resizableDataBuffer, size_t size) : statusCode(statusCode), resizableDataBuffer(resizableDataBuffer), dataBuffer(resizableDataBuffer->data()), size(size) { }

        TLSResponse(Surge::TLSStatus statusCode, char *dataBuffer, size_t size) : statusCode(statusCode), resizableDataBuffer(nullptr), dataBuffer(dataBuffer), size(size) { }

        TLSResponse(Surge::TLSStatus statusCode) : statusCode(statusCode), resizableDataBuffer(nullptr), dataBuffer(nullptr), size(0) { }

        ~TLSResponse() {
            if (resizableDataBuffer != nullptr) {
                delete resizableDataBuffer;
            } else if (dataBuffer != nullptr) {
                free(dataBuffer);
            }
        }

        TLSStatus StatusCode() {
            return statusCode;
        }

        char *Data() {
            return dataBuffer;
        }

        size_t Size() {
            return size;
        }

        bool HasData() {
            return dataBuffer != nullptr && size > 0;
        }
    };

    class TLSClient {

    private:
        typedef enum {
            OK = 0,
            WRITE_TO_TRANSPORT = 1,
            READ_FROM_TRANSPORT = 2,
            UNKNOWN_ERROR = -1,
            TLS_ERROR = -2
        } OpenSSLStatus;

    public:
        TLSClient();
        ~TLSClient();

        void StartClient(ITransportInterface *transport);
        void StopClient();
        void OpenTLSConnection(std::function<void(TLSStatus)> callback);

        void RunTLSHandshake();
        bool ContinueTLSHandshake(const char* data, size_t size);

        Surge::TLSResponse DecryptData(const char* data, size_t size);
        Surge::TLSResponse EncryptData(const char* data, size_t size);

    private:
        void InitializeOpenSSL();
        void GenerateSSLContext();
        void SetupSSL();

        static int VerifyCertificate(int preverify, X509_STORE_CTX* x509_ctx);

        OpenSSLStatus InterpretSSLResult(SSL* ssl, int result) {
            switch (SSL_get_error(ssl, result)) {
                case SSL_ERROR_NONE: //0
                    return OK;
                case SSL_ERROR_SSL:  // 1
                    ERR_print_errors_fp(stdout);
                    return TLS_ERROR;

                case SSL_ERROR_WANT_READ: // 2
                    return WRITE_TO_TRANSPORT;

                case SSL_ERROR_WANT_WRITE: // 3
                    return READ_FROM_TRANSPORT;

                case SSL_ERROR_WANT_X509_LOOKUP:  // 4
                    return UNKNOWN_ERROR;
                case SSL_ERROR_ZERO_RETURN: // 5
                    return UNKNOWN_ERROR;
                case SSL_ERROR_SYSCALL: //6
                    return READ_FROM_TRANSPORT;
                case SSL_ERROR_WANT_CONNECT: //7
                    return UNKNOWN_ERROR;
                case SSL_ERROR_WANT_ACCEPT: //8
                    return UNKNOWN_ERROR;
                default:
                    return UNKNOWN_ERROR;
            }
        }

    private:
        SSL_CTX* sslContext;
        SSL* ssl;

        BIO *appBio;
        BIO *openSSLBio;

        ITransportInterface* transport;

        std::function<void(TLSStatus)> sslConnectedResponse;
    };
}

#endif /* OpenSSLService_hpp */
