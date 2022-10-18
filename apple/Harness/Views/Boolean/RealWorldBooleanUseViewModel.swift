//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class RealWorldBooleanUseViewModel: ObservableObject {
    @Published var isLoading: Bool = true
    @Published var boolean: Bool = true
    
    @Inject private var featureFlagService: FeatureFlagService
    
    private var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        self.featureFlagService.boolVariation(evaluationId: "RealWorldBoolean") { [weak self] (response) in
            self?.boolean = response
            self?.isLoading = false
        }
    }
}
