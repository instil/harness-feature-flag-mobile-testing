//
//  Copyright 2022 Harness
//

import Foundation

class SwappableFeatureFlagService: FeatureFlagService {
    @Inject private var harnessService: HarnessFeatureFlagService
    @Inject private var mockService: MockFeatureFlagService
    
    var useRealService = true
    
    func load(_ callback: @escaping () -> ()) {
        mockService.load { }
        harnessService.load(callback)
    }
    
    func boolVariation(evaluationId: String, _ callback: @escaping (Bool) -> ()) {
        if (self.useRealService) {
            harnessService.boolVariation(evaluationId: evaluationId, callback)
        } else {
            mockService.boolVariation(evaluationId: evaluationId, callback)
        }
    }
}
