//
//  Regex.h
//  SurgeCore
//
//  Created by Paul Shields on 14/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef Regex_h
#define Regex_h

#include <stdio.h>
#include <vector>
#include <string>
#include <regex>

namespace SurgeUtil {
    namespace RegexUtils {
        static std::vector<std::string> Split(const std::string &inputString, const std::string &regexString) {
            std::vector<std::string> result;
            std::regex regex(regexString, std::regex::extended);
            std::smatch matches;

            if (std::regex_search(inputString, matches, regex)) {
                for (size_t i = 0; i < matches.size(); ++i) {
                    result.push_back(matches[i].str());
                }
            }

            return result;
        }
    }
}


#endif /* Regex_h */
