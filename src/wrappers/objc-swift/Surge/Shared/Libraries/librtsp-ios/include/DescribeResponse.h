// -*-c++-*-
#ifndef __DESCRIBE_RESPONSE_H__
#define __DESCRIBE_RESPONSE_H__

#include "Response.h"
#include "RtspResponse.h"
#include "SessionDescriptionFactory.h"

#include <vector>

namespace Surge {

    class DescribeResponse: public RtspResponse {
    public:
        DescribeResponse(const Response* resp)
            : RtspResponse(resp),
              m_palettes(SessionDescriptionFactory::ParseSessionDescriptionsFromBuffer(GetBodyString()))
            { }

        const std::vector<SessionDescription> GetPalettes() const { return m_palettes; }

        const bool Ok() const override { return GetCode() == 200 && m_palettes.size() > 0; }

    private:
        std::vector<SessionDescription> m_palettes;
    };
    
};

#endif //__DESCRIBE_RESPONSE_H__
