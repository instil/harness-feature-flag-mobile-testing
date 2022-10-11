//
//  Copyright 2022 Harness
//

import Foundation

class MockFeatureFlagService: FeatureFlagService {
    func load(_ callback: @escaping()->()) { }
    
    func destroy() { }
    
    func boolVariation(evaluationId: String, _ callback: @escaping(_ result: Bool)->()) {
        callback(true)
    }
    
    func stringVariation(evaluationId: String, _ callback: @escaping(_ result: String)->()) {
        callback("Mocked string response.")
    }
    
    func numberVariation(evaluationId: String, _ callback: @escaping (Int) -> ()) {
        callback(1)
    }
    
    func jsonVariation(evaluationId: String, _ callback: @escaping (String) -> ()) {
        callback("{}")
    }
}
