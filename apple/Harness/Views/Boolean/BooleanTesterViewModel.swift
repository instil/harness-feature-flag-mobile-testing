//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class BooleanTesterViewModel: ObservableObject {
    @Published var isLoading: Bool = true
    @Published var booleanOne: Bool = true
    @Published var booleanTwo: Bool = true
    @Published var booleanThree: Bool = true
    @Published var booleanFour: Bool = true
    @Published var booleanFive: Bool = true

    @Inject private var featureFlagService: FeatureFlagService
    @Inject private var configurationService: ConfigurationService
    
    private var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        self.featureFlagService.boolVariation(evaluationId: configurationService.getOrEmpty(key: "BOOLEAN_TESTER_KEY_ONE")) { [weak self] (response) in
            self?.booleanOne = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: configurationService.getOrEmpty(key: "BOOLEAN_TESTER_KEY_TWO")) { [weak self] (response) in
            self?.booleanTwo = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: configurationService.getOrEmpty(key: "BOOLEAN_TESTER_KEY_THREE")) { [weak self] (response) in
            self?.booleanThree = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: configurationService.getOrEmpty(key: "BOOLEAN_TESTER_KEY_FOUR")) { [weak self] (response) in
            self?.booleanFour = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: configurationService.getOrEmpty(key: "BOOLEAN_TESTER_KEY_FIVE")) { [weak self] (response) in
            self?.booleanFive = response
            self?.receivedResponses += 1
        }
    }
}
