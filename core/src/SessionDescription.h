// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __SESSION_DESCRIPTION_H__
#define __SESSION_DESCRIPTION_H__

#include "MimeTypes.h"
#include "Logging.h"

#include <string>
#include <vector>


namespace Surge {

    class SessionDescription {
    public:

        SessionDescription(): type(UNKNOWN),
                              control(""),
                              rtpMap(""),
                              formatParameters(""),
                              isNative(true),
                              framerate(-1),
                              fpsFraction(-1),
                              width(-1),
                              height(-1),
                              bitrate(0) { }
        
        SessionDescription(RtspSessionType type,
                           std::string control,
                           std::string rtpMap,
                           std::string fmtp,
                           bool isNative,
                           int framerate,
                           int fpsFraction,
                           int width,
                           int height,
                           int bitrate)
                : type(type),
                  control(control),
                  rtpMap(rtpMap),
                  formatParameters(fmtp),
                  isNative(isNative),
                  framerate(framerate),
                  fpsFraction(fpsFraction),
                  width(width),
                  height(height),
                  bitrate(bitrate) { }

        virtual ~SessionDescription() { }

        RtspSessionType GetType() const { return type; }

        bool IsControlUrlComplete() const { return control.find("://") != std::string::npos; }

        const std::string GetControl() const { return control; }

        const std::string GetRtpMap() const { return rtpMap; }

        const std::string GetFmtp() const { return formatParameters; }

        const bool IsNative() const { return isNative; }

        const int GetFramerate() const { return framerate; }
        
        const int GetFpsFraction() const { return fpsFraction; }
        
        const int GetWidth() const { return width; }
        
        const int GetHeight() const { return height; }
        
        const int GetBitrate() const { return bitrate; }
        
        const std::string GetFmtpH264ConfigParameters() const {
            if (type != RtspSessionType::H264) {
                return std::string();
            }
            
            size_t pos = formatParameters.find("sprop-parameter-sets=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = formatParameters.find(";", pos);
            return formatParameters.substr(pos + 21, end - pos - 21);
        }

        const std::string GetFmtpConfigParameters() const {
            size_t pos = formatParameters.find("config=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = formatParameters.find(";", pos);
            if (end == std::string::npos) {
                end = formatParameters.length();
            }
            
            return formatParameters.substr(pos + 7, end - pos);
        }

        static RtspSessionType GetTypeFromMime(const std::string& raw_mime) {
            if (raw_mime.find("H264") != std::string::npos) {
                return H264;
            }
            else if (raw_mime.find("MP4V-ES") != std::string::npos) {
                return MP4V;
            }
            else if (raw_mime.find("JPEG") != std::string::npos) {
                return MJPEG;
            }
            return UNKNOWN;
        }
        
        const int GetTimeoutOffset() {
            if (fpsFraction > 1) {
                return (fpsFraction * 1000);
            }
            
            return 0;
        }

    protected:
        RtspSessionType type;
        std::string control;
        std::string rtpMap;
        std::string formatParameters;
        
        bool isNative;
        int framerate;
        int fpsFraction;
        int width;
        int height;
        int bitrate;
    };
    
}

#endif //__SESSION_DESCRIPTION_H__
