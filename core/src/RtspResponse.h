// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __RTSP_RESPONSE_H__
#define __RTSP_RESPONSE_H__

#include "Response.h"

#include "Logging.h"
#include "Helpers.h"

#include <cstdlib>
#include <string>
#include <map>

#include <sstream>
  using std::ostringstream;

#include <stdexcept>
  using std::runtime_error;

namespace Surge {

    class RtspResponse {
    public:
        RtspResponse(const Response* resp) : m_code(500), m_body(), m_headers(), m_rawResponse() {
            m_rawResponse = resp->StringDump();
            std::vector<std::string> lines = SurgeUtil::StringSplit(m_rawResponse, "\r\n");

            if (lines.size() < 1) {
                ostringstream message;
                message << "Invalid RTSP Response no headers";
                throw runtime_error{ message.str() };
            }

            const std::string status_line = lines[0];
            std::vector<std::string> status_line_tokens = SurgeUtil::StringSplit(status_line, ' ');
            
            if (status_line_tokens.size() < 3) {
                ostringstream message;
                message << "Invalid RTSP Response - not enough status tokens";
                throw runtime_error{ message.str() };
            }
            
            if (status_line_tokens[0].compare("RTSP/1.0") != 0) {
                ostringstream message;
                message << "Invalid RTSP Response - invalid protocol for rtsp response";
                throw runtime_error{ message.str() };
            }

            m_code = atoi(status_line_tokens[1].c_str());
            
            bool is_body = false;
            for (auto it = lines.begin() + 1; it != lines.end(); ++it) {       
                std::string current_line = *it;
                                
                if (!is_body && current_line.empty()) {
                    is_body = true;
                    continue;
                }

                if (is_body && !current_line.empty()) {
                    m_body += current_line + "\r\n";
                }
                else if (!is_body && !current_line.empty()) {
                    size_t pos = current_line.find(':');
                    std::string key = current_line.substr(0, pos);
                    std::string value = current_line.substr(pos + 2, current_line.length());

                    m_headers.insert(std::pair<std::string, std::string>(key, value));
                }
            }
        }

        RtspResponse(int code, std::string body): m_code(code), m_body(body), m_headers(), m_rawResponse() {}

        virtual ~RtspResponse() { }

        const int GetCode() const { return m_code; }

        const std::string GetBodyString() const { return m_body; }

        const unsigned char *GetBody() const { return (unsigned char*)m_body.c_str(); }

        size_t GetBodyLength() const { return GetBodyString().length(); }

        const std::map<std::string, std::string> &GetHeaders() const { return m_headers; }

        virtual const bool Ok() const { return GetCode() == 200; }

        const std::string HeaderValueForKey(const std::string& key) {
            std::string header_value;

            const std::map<std::string, std::string> headers = GetHeaders();
            auto search = headers.find(key);
            
            if (search != headers.end()) {
                header_value = search->second;
            }
            
            return header_value;
        }

        const std::string GetRawResponse() { return m_rawResponse; }
        
    private:
        int m_code;
        std::string m_body;
        std::map<std::string, std::string> m_headers;
        std::string m_rawResponse;
    };
    
};

#endif //__RTSP_RESPONSE_H__
