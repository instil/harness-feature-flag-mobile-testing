//
//  Copyright 2022 Harness
//

import Foundation
import UIKit

class MainViewModel: ObservableObject {
    @Published var useRealService = true {
        didSet { featureFlagService.useRealService = useRealService }
    }
    
    @Inject private var featureFlagService: SwappableFeatureFlagService {
        didSet { self.useRealService = self.featureFlagService.useRealService }
    }
    
    func suspendApp() {
        UIApplication.shared.perform(#selector(NSXPCConnection.suspend))
    }
    
    func closeApp() {
        // NOTE: Including this in an app will cause it to be rejected from the app store
        exit(0)
    }
}
