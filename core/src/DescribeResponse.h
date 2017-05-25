// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __DESCRIBE_RESPONSE_H__
#define __DESCRIBE_RESPONSE_H__

#include "Response.h"
#include "RtspResponse.h"
#include "SessionDescriptionFactory.h"

#include <vector>

namespace Surge {

    class DescribeResponse: public RtspResponse {
    public:
        DescribeResponse(const Response* resp, SessionDescriptionFactory* factory)
            : RtspResponse(resp),
              sessionDescriptions(factory->ParseSessionDescriptionsFromBuffer(GetBodyString()))
            { }

        const std::vector<SessionDescription> GetSessionDescriptions() const { return sessionDescriptions; }

        const bool Ok() const override { return GetCode() == 200 && sessionDescriptions.size() > 0; }

    private:
        std::vector<SessionDescription> sessionDescriptions;
    };
    
};

#endif //__DESCRIBE_RESPONSE_H__
