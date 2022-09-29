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

    let featureFlagService = FeatureFlagService()

    var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        featureFlagService.load() { [weak self] in
            self?.featureFlagService.boolVariation(evaluationId: "Boolean1") { (response) in
                self?.booleanOne = response
                self?.receivedResponses += 1
            }
            
            self?.featureFlagService.boolVariation(evaluationId: "Boolean2") { (response) in
                self?.booleanTwo = response
                self?.receivedResponses += 1
            }
            
            self?.featureFlagService.boolVariation(evaluationId: "Boolean3") { (response) in
                self?.booleanThree = response
                self?.receivedResponses += 1
            }
            
            self?.featureFlagService.boolVariation(evaluationId: "Boolean4") { (response) in
                self?.booleanFour = response
                self?.receivedResponses += 1
            }
            
            self?.featureFlagService.boolVariation(evaluationId: "Boolean5") { (response) in
                self?.booleanFive = response
                self?.receivedResponses += 1
            }
        }
    }
}
