//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class MultiVariateStringYouTubeViewModel: ObservableObject {
    @Published var youtubeUrl = ""
    @Published var isLoading = true
    
    var youtubeUrlIsValid: Bool {
        get { !youtubeUrl.isEmpty && !isLoading }
    }

    @Inject private var featureFlagService: FeatureFlagService
    
    init() {
        featureFlagService.load() { [weak self] in
            self?.featureFlagService.stringVariation(evaluationId: "YouTubeUrl") { (response) in
                self?.youtubeUrl = response
                self?.isLoading = false
            }
        }
    }
}
