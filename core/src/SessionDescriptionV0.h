// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __SESSION_DESCRIPTION_V0_H__
#define __SESSION_DESCRIPTION_V0_H__

#include "SessionDescription.h"
#include "MimeTypes.h"
#include "Helpers.h"
#include "Logging.h"
#include "RegexUtils.h"

#include <string>

#define EXTRACT_PAYLOAD_TYPE_REGEX "rtpmap:([0-9]+)"

namespace Surge {

    class SessionDescriptionV0: public SessionDescription {
    public:
        SessionDescriptionV0(const std::string& rawSessionDescription);

    private:
        void ExtractPayloadTypeFrom(std::string &rtpMap) {
            auto result = SurgeUtil::RegexUtils::Split(rtpMap, EXTRACT_PAYLOAD_TYPE_REGEX);
            if (result.size() >= 2) {
                payloadType = stoi(result[1]);
            }
        }
    };
    
}

#endif //__SESSION_DESCRIPTION_V0_H__
