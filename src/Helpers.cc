#include "Helpers.h"

#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <string>


int SurgeUtil::SetNonBlocking(const int fd)
{
    return fcntl(fd, F_SETFL, O_NONBLOCK);
}

std::vector<std::string> SurgeUtil::StringSplit(const std::string& text, char sep)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    
    tokens.push_back(text.substr(start));
    return tokens;
}

std::vector<std::string> SurgeUtil::StringSplit(const std::string& input, std::string token)
{
    std::vector<std::string> output;
    std::string data = input;
    
    size_t pos = std::string::npos;
    
    do {
        pos = data.find(token);
        output.push_back(data.substr(0, pos));
        
        if (std::string::npos != pos) {
            data = data.substr(pos + token.size());
        }
    } while (std::string::npos != pos);
    
    return output;
}

std::string SurgeUtil::FindKeyAndValuePair(const std::vector<std::string>* values, const std::string& key)
{
    for (auto it = values->begin(); it != values->end(); ++it) {
        std::string value = *it;
        if (value.find(key) != std::string::npos) {
            return value;
        }
    }
    return std::string();
}
