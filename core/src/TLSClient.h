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

#include "ITLSClient.h"

#define SSL_TLS_CLIENT_STORE_LOCATION 0

namespace Surge {

    class TLSClient : public ITLSClient {

    private:
        typedef enum {
            OK = 0,
            WRITE_TO_TRANSPORT = 1,
            READ_FROM_TRANSPORT = 2,
            HANDSHAKE_COMPLETE = 3,
            UNKNOWN_ERROR = -1,
            TLS_ERROR = -2
        } OpenSSLStatus;

    public:
        TLSClient(bool certificateValidationEnabled, bool selfSignedCertsAllowed);
        ~TLSClient() override;

        void StartClient(ISecureTransport *transport) override;
        void StopClient() override;
        void OpenTLSConnection(std::function<void(TLSStatus)> callback) override;

        Surge::TLSResponse DecryptData(const char* data, size_t size) override;
        Surge::TLSResponse EncryptData(const char* data, size_t size) override;

        void SetTrustedCertificate(const std::string& fileUri) override;

    private:
        void InitializeOpenSSL();
        void GenerateSSLContext();
        void SetupSSL();

        void RunTLSHandshake();
        OpenSSLStatus ContinueTLSHandshake(const char* data, size_t size);

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
                case SSL_ERROR_ZERO_RETURN: // 5
                    return UNKNOWN_ERROR;
                case SSL_ERROR_SYSCALL: //6
                    return READ_FROM_TRANSPORT;
                case SSL_ERROR_WANT_CONNECT: //7
                case SSL_ERROR_WANT_ACCEPT: //8
                default:
                    return UNKNOWN_ERROR;
            }
        }

        template <class T>
        static void StoreItemInSSLObject(SSL *ssl, int index, T *object) {
            SSL_set_ex_data(ssl, SSL_TLS_CLIENT_STORE_LOCATION, (void *)object);
        }

        template <class T>
        static T* RetrieveItemInSSLObject(SSL *ssl, int index) {
            return (T*)SSL_get_ex_data(ssl, 0);
        }

        template <class T>
        static T* RetrieveItemInSSLObject(X509_STORE_CTX *x509Certificate, int index) {
            SSL *ssl = (SSL *)X509_STORE_CTX_get_ex_data(x509Certificate, SSL_get_ex_data_X509_STORE_CTX_idx());
            return RetrieveItemInSSLObject<T>(ssl, index);
        }

    private:
        SSL_CTX* sslContext;
        SSL* ssl;

        BIO *appBio;
        BIO *openSSLBio;

        ISecureTransport* transport;

        std::function<void(TLSStatus)> sslConnectedResponse;

        bool certificateValidationEnabled;
        bool selfSignedCertsAllowed;
        std::string trustedCertificate;
    };
}

#endif /* OpenSSLService_hpp */
