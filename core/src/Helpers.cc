// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Helpers.h"

#include <fcntl.h>


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
