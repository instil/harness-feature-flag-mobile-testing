//
//  AppDelegate.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {
    var window: UIWindow?
    
    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        UINavigationBar.appearance().barTintColor = UIColor.primaryTint
        UINavigationBar.appearance().titleTextAttributes = [NSAttributedStringKey.foregroundColor: UIColor.white]
        UINavigationBar.appearance().isTranslucent  = false
        UIView.appearance(whenContainedInInstancesOf: [UIAlertController.self]).tintColor = UIColor.primaryTint
        self.window?.tintColor = .white
        
        return true
    }
}
