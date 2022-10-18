//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateNumberViewModel: ObservableObject {
    @Published var isLoading: Bool = true
    @Published var number: Int? = nil
    
    @Inject private var featureFlagService: FeatureFlagService
    
    private var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        self.featureFlagService.numberVariation(evaluationId: "Number") { [weak self] (response) in
            self?.number = response
            self?.isLoading = false
        }
    }
}
