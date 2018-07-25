//
//  TLSFactory.h
//  SurgeCore
//
//  Created by Paul Shields on 24/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef TLSFactory_h
#define TLSFactory_h

#include "ITLSClient.h"
#include "TLSClient.h"

namespace Surge {
    class TLSFactory {
    public:
        static ITLSClient* GenerateTLSClient(bool certificateValidationEnabled, bool selfSignedCertsAllowed) {
            return new TLSClient(certificateValidationEnabled, selfSignedCertsAllowed);
        }

    private:
        TLSFactory() {}
    };
}
#endif /* TLSFactory_h */
