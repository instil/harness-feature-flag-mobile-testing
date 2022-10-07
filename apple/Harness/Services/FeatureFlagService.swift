//
//  Copyright 2022 Harness
//

import Foundation

protocol FeatureFlagService {
    func load(_ callback: @escaping()->())
    func destroy()
    func boolVariation(evaluationId: String, _ callback: @escaping(_ result: Bool)->())
    func stringVariation(evaluationId: String, _ callback: @escaping(_ result: String)->())
}
