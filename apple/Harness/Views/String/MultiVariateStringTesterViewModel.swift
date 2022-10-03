//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateStringTesterViewModel: ObservableObject {
    @Published var stringFeatureFlag = "Loading..."

    @Inject private var featureFlagService: FeatureFlagService
    
    init() {
        featureFlagService.load() { [weak self] in
            self?.featureFlagService.stringVariation(evaluationId: "TestString") { (response) in
                self?.stringFeatureFlag = response
            }
        }
    }
}
