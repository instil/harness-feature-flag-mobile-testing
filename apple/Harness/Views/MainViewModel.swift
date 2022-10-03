//
//  Copyright 2022 Harness
//

import Foundation

class MainViewModel: ObservableObject {
    @Published var useRealService = true {
        didSet { featureFlagService.useRealService = useRealService }
    }
    
    @Inject private var featureFlagService: SwappableFeatureFlagService {
        didSet { self.useRealService = self.featureFlagService.useRealService }
    }
}
