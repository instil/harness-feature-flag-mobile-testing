//
//  Copyright 2022 Harness
//

import Foundation
import Swinject
import SwinjectAutoregistration

class DependencyInjection {
    static let shared = DependencyInjection()
    
    private var container: Container
    
    init() {
        self.container = Self.buildContainer()
    }
    
    func resolve<T>(_ type: T.Type) -> T {
        container.resolve(T.self)!
    }
    
    private static func buildContainer() -> Container {
        let container = Container(defaultObjectScope: .container)
        
        container.autoregister(ConfigurationService.self, initializer: ConfigurationService.init)
        
        container.autoregister(HarnessFeatureFlagService.self, initializer: HarnessFeatureFlagService.init)
        container.autoregister(MockFeatureFlagService.self, initializer: MockFeatureFlagService.init)
        container.autoregister(SwappableFeatureFlagService.self, initializer: SwappableFeatureFlagService.init)
        container.register(FeatureFlagService.self) { r in r.resolve(SwappableFeatureFlagService.self)! }
        
        return container
    }
}
