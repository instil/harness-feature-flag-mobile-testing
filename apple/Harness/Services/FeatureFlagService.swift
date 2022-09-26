//
//  Copyright 2022 Harness
//

import Foundation
import ff_ios_client_sdk

class FeatureFlagService {
    var inInitialized: Bool {
        get {
            CfClient.sharedInstance.isInitialized
        }
    }
    
    func load(_ callback: @escaping()->()) {
        guard let apiKey = Bundle.main.infoDictionary?["HARNESS_SDK_KEY"] as? String else {
            return
        }
        
        let config = CfConfiguration.builder()
          .setStreamEnabled(true)
          .build()
        
        let target = CfTarget.builder().setIdentifier("TargetInstil").build()
        
        CfClient.sharedInstance.initialize(
          apiKey: apiKey,
          configuration: config,
          target: target
        ) { result in
            switch result {
            case .failure(let error):
                NSLog("Feature Flag SDK could not be initialized: Error \(error)")
            case .success():
                NSLog("Feature Flag SDK initialized!")
                callback()
            }
        }
    }
    
    func boolVariation(evaluationId: String, _ callback: @escaping(_ result: Evaluation?)->()) {
        if (!self.inInitialized) {
            return
        }
        
        CfClient.sharedInstance.boolVariation(evaluationId: evaluationId, { (eval) in
          callback(eval)
        })
    }
    
    func registerEventsListener() {
        CfClient.sharedInstance.registerEventsListener() { (result) in
            switch result {
            case .failure(let error):
                print(error)
            case .success(let eventType):
                switch eventType {
                case .onPolling:
                    print("Event: Received all evaluation flags")
                case .onEventListener(let evaluation):
                    print("Event: Received an evaluation flag, \(evaluation!)")
                case .onComplete:
                    print("Event: SSE stream has completed")
                case .onOpen:
                    print("Event: SSE stream has been opened")
                case .onMessage(let messageObj):
                    print(messageObj?.event ?? "Event: Message received")
                }
            }
        }
    }
}
