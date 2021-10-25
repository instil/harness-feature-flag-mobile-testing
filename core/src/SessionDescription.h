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
                              bitrate(0),
                              payloadType(0) { }
        
        SessionDescription(RtspSessionType type,
                           std::string control,
                           std::string rtpMap,
                           std::string fmtp,
                           bool isNative,
                           int framerate,
                           int fpsFraction,
                           int width,
                           int height,
                           int bitrate,
                           uint8_t payloadType)
                : type(type),
                  control(control),
                  rtpMap(rtpMap),
                  formatParameters(fmtp),
                  isNative(isNative),
                  framerate(framerate),
                  fpsFraction(fpsFraction),
                  width(width),
                  height(height),
                  bitrate(bitrate),
                  payloadType(payloadType) { }

        virtual ~SessionDescription() { }

        RtspSessionType GetType() const { return type; }

        const std::string GetTypeAsString() const { return RtspSessionTypeString::FromEnum(type); }

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

        const uint8_t GetPayloadType() const { return payloadType; }

        const std::string GetFmtpH264ConfigParameters() const {
            if (type != RtspSessionType::H264) {
                return std::string();
            }

            return GetFormatParametersValueFromKey("sprop-parameter-sets");
        }

        const std::string GetFmtpH265Vps() const {
            if (type != RtspSessionType::H265) {
                return std::string();
            }

            return GetFormatParametersValueFromKey("sprop-vps");
        }

        const std::string GetFmtpH265Sps() const {
            if (type != RtspSessionType::H265) {
                return std::string();
            }

            return GetFormatParametersValueFromKey("sprop-sps");
        }

        const std::string GetFmtpH265Pps() const {
            if (type != RtspSessionType::H265) {
                return std::string();
            }

            return GetFormatParametersValueFromKey("sprop-pps");
        }

        const std::string GetFormatParametersValueFromKey(const std::string key) const {
            size_t pos = formatParameters.find(key + "=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = formatParameters.find(";", pos);
            return formatParameters.substr(pos + key.length() + 1, end - pos - key.length() - 1);
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
            else if (raw_mime.find("H265") != std::string::npos) {
                return H265;
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
        uint8_t payloadType;

        bool isNative;
        int framerate;
        int fpsFraction;
        int width;
        int height;
        int bitrate;
    };
    
}

#endif //__SESSION_DESCRIPTION_H__
