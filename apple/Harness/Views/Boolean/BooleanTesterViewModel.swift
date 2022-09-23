//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class BooleanTesterViewModel: ObservableObject {
    @Published var booleanOne: Bool = true

    let featureFlagService = FeatureFlagService()
    
    init() {
        featureFlagService.load() { [weak self] in
            self?.featureFlagService.boolVariation(evaluationId: "Boolean1") { (response) in
                self?.booleanOne = response?.identifier == "true"
                print(String(self?.booleanOne ?? false))
            }
        }
    }
}
