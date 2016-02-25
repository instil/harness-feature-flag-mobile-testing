// -*-c++-*-
#ifndef __URL_H__
#define __URL_H__

#include <string>
#include <map>

namespace SurgeUtil {
    
    class Url {
    public:
        Url(std::string url): m_rawUrl(url) { }

        int GetPort() {
            std::string raw_host = GetRawHostAndPort();
            size_t pos = raw_host.find(':');
            if (pos == std::string::npos) {
                return 554;
            }

            std::string port_string = raw_host.substr(pos + 1, raw_host.length());
            return atoi(port_string.c_str());
        }

        std::string GetHost() {
            std::string raw_host = GetRawHostAndPort();
            std::string host = raw_host;
            size_t pos = raw_host.find(':');
            if (pos != std::string::npos) {
                host = raw_host.substr(0, pos);
            }
            return host;
        }

        std::string GetRawHostAndPort() {
            size_t pos = m_rawUrl.find("://");
            std::string url_no_scheme = m_rawUrl.substr(pos + 3, m_rawUrl.length());
            pos = url_no_scheme.find('/');

            std::string raw_host = url_no_scheme.substr(0, pos);
            return raw_host;
        }

        std::string GetScheme() {
            size_t pos = m_rawUrl.find("://");
            return m_rawUrl.substr(0, pos);
        }

        std::string GetFullPath() {
            size_t pos = m_rawUrl.find("://");
            std::string url_no_scheme = m_rawUrl.substr(pos + 3, m_rawUrl.length());
            pos = url_no_scheme.find('/');
            return url_no_scheme.substr(pos, url_no_scheme.length());
        }

        std::string GetPath() {
            std::string full_path = GetFullPath();
            size_t pos = full_path.find('?');
            if (pos == std::string::npos) {
                return full_path;
            }

            return full_path.substr(0, pos);
        }

        std::map<std::string, std::string> GetQueryParameters() {
            std::map<std::string, std::string> parameters;

            std::string full_path = GetFullPath();
            size_t pos = full_path.find('?');
            if (pos != std::string::npos) {
                std::string parameters_string = full_path.substr(pos + 1, full_path.length());
                std::vector<std::string> param_tokens = split(parameters_string, '&');

                for (auto it = param_tokens.begin(); it != param_tokens.end(); ++it) {
                    std::string raw_param = *it;
                    pos = raw_param.find('=');
                    std::string key = raw_param.substr(0, pos);
                    std::string value = raw_param.substr(pos + 1, raw_param.length());
                    parameters.insert(std::pair<std::string, std::string>(key, value));
                }
            }
            
            return parameters;
        }

    private:
        std::vector<std::string> split(const std::string &text, char sep) {
            std::vector<std::string> tokens;
            std::size_t start = 0, end = 0;
            while ((end = text.find(sep, start)) != std::string::npos) {
                tokens.push_back(text.substr(start, end - start));
                start = end + 1;
            }
            tokens.push_back(text.substr(start));
            return tokens;
        }
        
        std::string m_rawUrl;
    };
}

#endif //__URL_H__
