// -*-c++-*-
#ifndef __DESCRIBE_RESPONSE_H__
#define __DESCRIBE_RESPONSE_H__

#include "Response.h"
#include "RtspResponse.h"

namespace Surge {

    class DescribeResponse: public RtspResponse {
    public:
        DescribeResponse(const Response* resp): RtspResponse(resp) {
            // TODO... palettes
        }
    };
    
};

#endif //__DESCRIBE_RESPONSE_H__
