// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __SESSION_DESCRIPTION_FACTORY_H__
#define __SESSION_DESCRIPTION_FACTORY_H__

#include "SessionDescription.h"

#include <vector>
#include <string>

namespace Surge {

    class SessionDescriptionFactory {

    public:
        std::vector<SessionDescription> ParseSessionDescriptionsFromBuffer(const std::string& body);
        
    protected:
        virtual std::vector<SessionDescription> ParseSessionDescription(const std::string& versionLine, const std::string& body);
        
    };
    
};

#endif //__SESSION_DESCRIPTION_FACTORY_H__
