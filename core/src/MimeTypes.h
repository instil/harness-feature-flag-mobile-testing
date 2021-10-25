// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __MIME_TYPES_H__
#define __MIME_TYPES_H__

#include <string>

namespace Surge {

    enum RtspSessionType : int {
        H264,
        H265,
        MP4V,
        MJPEG,
        UNKNOWN
    };

    namespace RtspSessionTypeString {
        static std::string FromEnum(RtspSessionType type) {
            switch (type) {
            case H264:
                return "H.264";
            case H265:
                return "H.265";
            case MP4V:
                return "MP4V";
            case MJPEG:
                return "MJPEG";
            default:
                return "Unknown";
            }
        }
    };

};

#endif //__MIME_TYPES_H__
