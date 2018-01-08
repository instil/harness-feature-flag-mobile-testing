//
//  TransportProtocol.h
//  
//
//  Created by Paul Shields on 27/11/2017.
//

#ifndef TransportProtocol_h
#define TransportProtocol_h

namespace Surge {
    enum class TransportProtocol {
        InterleavedTcp,
        Udp,
        HttpTunneled
    };
}

#endif /* TransportProtocol_h */
