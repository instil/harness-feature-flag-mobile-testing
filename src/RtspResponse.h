// -*-c++-*-
#ifndef __RTSP_RESPONSE_H__
#define __RTSP_RESPONSE_H__

#include "Response.h"

#include "Logging.h"
#include "Helpers.h"

#include <string>
#include <map>

namespace Surge {

    class RtspResponse {
    public:
        RtspResponse(const Response* resp) {
            const std::string buffer = resp->StringDump();
            std::vector<std::string> lines = SurgeUtil::StringSplit(buffer, "\r\n");

            if (lines.size() < 1) {
                ERROR("Invalid RTSP Response: " << buffer);
                return;
            }

            const std::string status_line = lines[0];
            std::vector<std::string> status_line_tokens = SurgeUtil::StringSplit(status_line, ' ');
            
            if (status_line_tokens.size() < 3) {
                ERROR("Invalid RTSP Response: " << buffer);
                return;
            }
            
            if (status_line_tokens[0].compare("RTSP/1.0") != 0) {
                ERROR("Invalid RTSP Response: " << buffer);
                return;
            }

            m_code = std::stoi(status_line_tokens[1], nullptr);
            
            bool is_body = false;
            for (auto it = lines.begin() + 1; it != lines.end(); ++it) {       
                std::string current_line = *it;
                                
                if (!is_body && current_line.empty()) {
                    is_body = true;
                    continue;
                }

                if (is_body) {
                    m_body += current_line;
                }
                else {
                    size_t pos = current_line.find(':');
                    std::string key = current_line.substr(0, pos);
                    std::string value = current_line.substr(pos + 2, current_line.length());

                    m_headers.insert(std::pair<std::string, std::string>(key, value));
                }
            }
        }

        const int GetCode() const { return m_code; }

        const std::string GetBody() const { return m_body; }

        const std::map<std::string, std::string> GetHeaders() const { return m_headers; }

        const bool Ok() const { return GetCode() == 200; }
        
    private:
        int m_code;
        std::string m_body;
        std::map<std::string, std::string> m_headers;
    };
    
};

#endif //__RTSP_RESPONSE_H__
