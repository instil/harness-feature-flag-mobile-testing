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
    
    private var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        self.featureFlagService.boolVariation(evaluationId: "Boolean1") { [weak self] (response) in
            self?.booleanOne = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: "Boolean2") { [weak self] (response) in
            self?.booleanTwo = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: "Boolean3") { [weak self] (response) in
            self?.booleanThree = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: "Boolean4") { [weak self] (response) in
            self?.booleanFour = response
            self?.receivedResponses += 1
        }
        
        self.featureFlagService.boolVariation(evaluationId: "Boolean5") { [weak self] (response) in
            self?.booleanFive = response
            self?.receivedResponses += 1
        }
    }
}
