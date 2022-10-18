//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateStringWebViewModel: ObservableObject {
    @Published var url: URL? = nil
    
    var isLoading = true
    
    @Inject private var featureFlagService: FeatureFlagService
    @Inject private var configurationService: ConfigurationService
    
    init() {
        self.featureFlagService.stringVariation(evaluationId: configurationService.getOrEmpty(key: "STRING_WEB_VIEW_KEY")) { [weak self] (response) in
            self?.url = URL(string: response)
            self?.isLoading = false
        }
    }
}
