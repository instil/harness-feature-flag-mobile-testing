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

#include <string>


namespace Surge {

    class SessionDescriptionV0: public SessionDescription {
    public:
        SessionDescriptionV0(const std::string& rawSessionDescription);
    };
    
}

#endif //__SESSION_DESCRIPTION_V0_H__
