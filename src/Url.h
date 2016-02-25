// -*-c++-*-
#ifndef __URL_H__
#define __URL_H__

#include <string>

namespace SurgeUtil {
    class Url {
    public:
        Url(std::string url): m_rawUrl(url) { }

        int GetPort() {
            return 0;
        }

        std::string GetHost() {
            return "";
        }

        std::string GetScheme() {
            return "";
        }

        std::string GetPath() {
            return "";
        }

    private:
        std::string m_rawUrl;
    };
}

#endif //__URL_H__
