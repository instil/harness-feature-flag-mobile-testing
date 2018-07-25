//
//  TLSResponse.h
//  SurgeCore
//
//  Created by Paul Shields on 24/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef TLSResponse_h
#define TLSResponse_h

#include <vector>

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
}
#endif /* TLSResponse_h */
