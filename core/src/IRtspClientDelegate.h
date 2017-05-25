// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __IRTSP_CLIENT_DELEGATE_H__
#define __IRTSP_CLIENT_DELEGATE_H__

#include "SessionDescription.h"

namespace Surge {

    class IRtspClientDelegate {
    public:

        virtual ~IRtspClientDelegate() { };

        // TIMEOUT
        virtual void ClientDidTimeout() = 0;

        // VIDEO
        virtual void ClientReceivedFrame(const unsigned char * frameBuffer,
                                         size_t length,
                                         int32_t width,
                                         int32_t height,
                                         int32_t presentationTime,
                                         int32_t duration) = 0;

        // ANNOUNCE / REDIRECT
        virtual void StreamConfigChanged(bool wasRedirect) = 0;

        virtual void ClientReceivedExtendedHeader(const unsigned char * buffer,
                                         size_t length) = 0;

    };
    
};

#endif // __IRTSP_CLIENT_DELEGATE_H__
