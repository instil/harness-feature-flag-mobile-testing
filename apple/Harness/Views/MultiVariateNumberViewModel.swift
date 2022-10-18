//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateNumberViewModel: ObservableObject {
    @Published var isLoading: Bool = true
    @Published var number: Int? = nil
    
    @Inject private var featureFlagService: FeatureFlagService
    @Inject private var configurationService: ConfigurationService
    
    private var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        self.featureFlagService.numberVariation(evaluationId: configurationService.getOrEmpty(key: "NUMBER_KEY")) { [weak self] (response) in
            self?.number = response
            self?.isLoading = false
        }
    }
}
