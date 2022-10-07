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
        self.featureFlagService.stringVariation(evaluationId: "YouTubeUrl") { [weak self] (response) in
            self?.youtubeUrl = response
            self?.isLoading = false
        }
    }
}
