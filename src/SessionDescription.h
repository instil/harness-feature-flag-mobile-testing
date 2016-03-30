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
                              m_control(""),
                              m_rtpMap(""),
                              m_fmtp(""),
                              m_frameRate(-1),
                              m_resolutionWidth(-1),
                              m_resolutionHeight(-1) { }
        
        SessionDescription(RtspSessionType type,
                           std::string control,
                           std::string rtpMap,
                           std::string fmtp,
                           int frameRate,
                           int width,
                           int height): m_type(type),
                                        m_control(control),
                                        m_rtpMap(rtpMap),
                                        m_fmtp(fmtp),
                                        m_frameRate(frameRate),
                                        m_resolutionWidth(width),
                                        m_resolutionHeight(height) { }

        virtual ~SessionDescription() { }

        RtspSessionType GetType() const { return m_type; }

        bool IsControlUrlComplete() const { return m_control.find("://") != std::string::npos; }

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
            return m_fmtp.substr(pos + 21, end - pos - 21);
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

        SessionDescription* DeepCopy() const {
            return new SessionDescription(m_type,
                                          m_control,
                                          m_rtpMap,
                                          m_fmtp,
                                          m_frameRate,
                                          m_resolutionWidth,
                                          m_resolutionHeight);
        }

        static RtspSessionType GetTypeFromMime(const std::string raw_mime) {
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

    protected:
        RtspSessionType m_type;
        std::string m_control;
        std::string m_rtpMap;
        std::string m_fmtp;
        int m_frameRate;
        int m_resolutionWidth;
        int m_resolutionHeight;
    };
    
}

#endif //__SESSION_DESCRIPTION_H__
