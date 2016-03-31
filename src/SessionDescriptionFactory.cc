#include "SessionDescriptionFactory.h"
#include "SessionDescriptionV0.h"

#include "Helpers.h"
#include "Logging.h"


namespace Surge {

    namespace SessionDescriptionFactory {
    
        std::vector<SessionDescription> ParseSessionDescriptionsFromBuffer(const std::string& body) {
            std::vector<SessionDescription> palettes;

            std::vector<std::string> lines = SurgeUtil::StringSplit(body, "\n");
            if (lines.size() > 0) {
                std::string version_line = lines[0];

                if (version_line.find("v=0") != std::string::npos) {
                    SessionDescriptionV0 session(body);
                    palettes.push_back(session);
                }
                else if (!version_line.empty()) {
                    ERROR("Unhandled Session Description version: [" << version_line << "]");
                }
                else {
                    ERROR("No Session Description available");
                }
            }
            
            return palettes;
        }
        
    }
}
