// -*-c++-*-
#ifndef __SESSION_DESCRIPTION_V0_H__
#define __SESSION_DESCRIPTION_V0_H__

#include "SessionDescription.h"
#include "MimeTypes.h"
#include "Helpers.h"
#include "Logging.h"

#include <string>


namespace Surge {

    class SessionDescriptionV0: public SessionDescription {
    public:
        SessionDescriptionV0(const std::string rawSessionDescription);
    };
    
}

#endif //__SESSION_DESCRIPTION_V0_H__
