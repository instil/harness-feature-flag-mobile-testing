//
//  Copyright 2022 Harness
//

import SwiftUI

@main
struct HarnessApp: App {
    @UIApplicationDelegateAdaptor(HarnessAppDelegate.self) var appDelegate
    
    var body: some Scene {
        WindowGroup {
            MainView()
        }
    }
}

class HarnessAppDelegate: NSObject, UIApplicationDelegate {
    @Inject private var featureFlagService: FeatureFlagService

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey : Any]? = nil) -> Bool {
        featureFlagService.load {
            NSLog("TEST APP: Feature flag service has finished loading")
        }
        
        return true
    }
        
    func applicationWillTerminate(_ application: UIApplication) {
        NSLog("TEST APP: App is terminating, cleaning up Harness SDK.")
        featureFlagService.destroy()
    }
}
