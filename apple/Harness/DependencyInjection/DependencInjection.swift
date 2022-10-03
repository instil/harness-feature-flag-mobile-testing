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
        let container = Container()
        
        container.autoregister(FeatureFlagService.self, initializer: SwappableFeatureFlagService.init)
        
        return container
    }
}
