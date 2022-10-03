//
//  Copyright 2022 Harness
//

import Foundation

class MockFeatureFlagService: FeatureFlagService {
    func load(_ callback: @escaping()->()) { }
    
    func boolVariation(evaluationId: String, _ callback: @escaping(_ result: Bool)->()) {
          callback(true)
    }
}