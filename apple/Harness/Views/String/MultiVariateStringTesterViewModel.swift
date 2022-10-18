//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateStringTesterViewModel: ObservableObject {
    @Published var stringFeatureFlag = "Loading..."

    @Inject private var featureFlagService: FeatureFlagService
    @Inject private var configurationService: ConfigurationService
    
    init() {
        self.featureFlagService.stringVariation(evaluationId: configurationService.getOrEmpty(key: "STRING_TESTER_KEY")) { [weak self] (response) in
            self?.stringFeatureFlag = response
        }
    }
}
