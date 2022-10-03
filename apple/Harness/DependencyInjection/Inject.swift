//
//  Copyright 2022 Harness
//

import Foundation

@propertyWrapper
struct Inject<I> {
    let wrappedValue: I
    
    init() {
        //Resolve the interface to an implementation.
        self.wrappedValue = DependencyInjection.shared.resolve(I.self)
    }
}
