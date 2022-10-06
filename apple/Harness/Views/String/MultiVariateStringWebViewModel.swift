//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateStringWebViewModel: ObservableObject {
    @Published var url: URL? = nil
    
    var isLoading = true
    
    @Inject private var featureFlagService: FeatureFlagService
    
    init() {
        self.featureFlagService.stringVariation(evaluationId: "WebViewUrl") { [weak self] (response) in
            self?.url = URL(string: response)
            self?.isLoading = false
        }
    }
}
