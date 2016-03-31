// -*-c++-*-
#ifndef __SESSION_DESCRIPTION_FACTORY_H__
#define __SESSION_DESCRIPTION_FACTORY_H__

#include "SessionDescription.h"

#include <vector>
#include <string>

namespace Surge {

    namespace SessionDescriptionFactory {

        std::vector<SessionDescription> ParseSessionDescriptionsFromBuffer(const std::string& body);
        
    };
    
};

#endif //__SESSION_DESCRIPTION_FACTORY_H__
