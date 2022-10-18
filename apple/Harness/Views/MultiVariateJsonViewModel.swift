//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateJsonViewModel: ObservableObject {
    @Published var isLoading: Bool = true
    @Published var json: String = ""
    
    @Inject private var featureFlagService: FeatureFlagService
    @Inject private var configurationService: ConfigurationService
    
    private var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        self.featureFlagService.jsonVariation(evaluationId: configurationService.getOrEmpty(key: "JSON_KEY")) { [weak self] (response) in
            self?.json = response
            self?.isLoading = false
        }
    }
}
