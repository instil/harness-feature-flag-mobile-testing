//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class ConfigurationService {
    func get(key: String) -> String? {
        return Bundle.main.infoDictionary?[key] as? String
    }
    
    func getOrEmpty(key: String) -> String {
        return get(key: key) ?? ""
    }
}
