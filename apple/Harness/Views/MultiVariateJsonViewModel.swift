//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateJsonViewModel: ObservableObject {
    @Published var isLoading: Bool = true
    @Published var json: String = ""
    
    @Inject private var featureFlagService: FeatureFlagService
    
    private var receivedResponses = 0 {
        didSet { isLoading = receivedResponses < 5 }
    }
    
    init() {
        self.featureFlagService.jsonVariation(evaluationId: "Json") { [weak self] (response) in
            self?.json = response
            self?.isLoading = false
        }
    }
}
