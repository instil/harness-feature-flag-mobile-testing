#include "SessionDescriptionFactory.h"
#include "SessionDescriptionV0.h"

#include "Helpers.h"
#include "Logging.h"


namespace Surge {

    namespace SessionDescriptionFactory {
    
        std::vector<SessionDescription> ParseSessionDescriptionsFromBuffer(const std::string body) {
            std::vector<SessionDescription> palettes;

            std::vector<std::string> lines = SurgeUtil::StringSplit(body, "\r\n");
            if (lines.size() > 0) {
                std::string version_line = lines[0];

                if (version_line.compare("v=0") == 0) {
                    SessionDescriptionV0 session(body);
                    palettes.push_back(session);
                }
                else {
                    ERROR("Unhandled Session Description version: [" << version_line << "]");
                }
            }
            
            return palettes;
        }
        
    }
}