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
        featureFlagService.load() { [weak self] in
            self?.featureFlagService.stringVariation(evaluationId: "WebViewUrl") { (response) in
                self?.url = URL(string: response)
                self?.isLoading = false
            }
        }
    }
}
