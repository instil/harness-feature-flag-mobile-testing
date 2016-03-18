// -*-c++-*-
#ifndef __SESSION_DESCRIPTION_H__
#define __SESSION_DESCRIPTION_H__

#include "MimeTypes.h"
#include "Logging.h"

#include <string>
#include <vector>


namespace Surge {

    class SessionDescription {
    public:

        SessionDescription(): m_type(UNKNOWN),
                              m_controlUrlIsComplete(false),
                              m_control(""),
                              m_rtpMap(""),
                              m_fmtp(""),
                              m_frameRate(-1),
                              m_resolutionWidth(-1),
                              m_resolutionHeight(-1) { }
        
        SessionDescription(RtspSessionType type,
                           bool completeControlUrl,
                           std::string control,
                           std::string rtpMap,
                           std::string fmtp,
                           int frameRate,
                           int width,
                           int height): m_type(type),
                                        m_controlUrlIsComplete(completeControlUrl),
                                        m_control(control),
                                        m_rtpMap(rtpMap),
                                        m_fmtp(fmtp),
                                        m_frameRate(frameRate),
                                        m_resolutionWidth(width),
                                        m_resolutionHeight(height) { }

        virtual ~SessionDescription() { }

        RtspSessionType GetType() const { return m_type; }

        bool IsControlUrlComplete() const { return m_controlUrlIsComplete; }

        const std::string GetControl() const { return m_control; }

        const std::string GetRtpMap() const { return m_rtpMap; }

        const std::string GetFmtp() const { return m_fmtp; }

        const std::string GetFmtpH264ConfigParameters() const {
            if (m_type != RtspSessionType::H264) {
                return std::string();
            }
            
            size_t pos = m_fmtp.find("sprop-parameter-sets=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = m_fmtp.find(";", pos);
            if (end == std::string::npos) {
                end = m_fmtp.length();
            }
            
            return m_fmtp.substr(pos + 21, end - pos);
        }

        const std::string GetFmtpConfigParameters() const {
            size_t pos = m_fmtp.find("config=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = m_fmtp.find(";", pos);
            if (end == std::string::npos) {
                end = m_fmtp.length();
            }
            
            return m_fmtp.substr(pos + 7, end - pos);
        }

        int GetFrameRate() const { return m_frameRate; }

        int GetResolutionWidth() const { return m_resolutionWidth; }

        int GetResolutionHeight() const { return m_resolutionHeight; }

        static RtspSessionType GetTypeFromMime(const std::string raw_mime) {
            if (raw_mime.find("H264") != std::string::npos) {
                return H264;
            }
            else if (raw_mime.find("MP4V-ES") != std::string::npos) {
                return MP4V;
            }
            return UNKNOWN;
        }

    protected:
        RtspSessionType m_type;
        bool m_controlUrlIsComplete;
        std::string m_control;
        std::string m_rtpMap;
        std::string m_fmtp;
        int m_frameRate;
        int m_resolutionWidth;
        int m_resolutionHeight;
    };
    
}

#endif //__SESSION_DESCRIPTION_H__
