// -*-c++-*-
#ifndef __SESSION_DESCRIPTION_FACTORY_H__
#define __SESSION_DESCRIPTION_FACTORY_H__

#include <SessionDescription.h>

#include <vector>
#include <cstdlib>

namespace Surge {

    namespace SessionDescriptionFactory {

        std::vector<SessionDescription> ParseSessionDescriptionsFromBuffer(const unsigned char *buffer, size_t length);
        
    };
    
};

#endif //__SESSION_DESCRIPTION_FACTORY_H__
