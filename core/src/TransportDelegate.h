//
//  TransportDelegate.h
//  SurgeCore
//
//  Created by Paul Shields on 26/06/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef TransportDelegate_h
#define TransportDelegate_h

#include "RTPPacket.h"

namespace Surge {

    class TransportDelegate {
    public:

        virtual ~TransportDelegate() { }

        virtual void RtpPacketReceived(RtpPacket *packet) = 0;
    };

}

#endif /* TransportDelegate_h */
