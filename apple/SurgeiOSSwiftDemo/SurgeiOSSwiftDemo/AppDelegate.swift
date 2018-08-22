//
//  AppDelegate.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

extension UIColor {
    static var primaryTint: UIColor {
        return UIColor(red: 54.0/255.0, green: 121.0/255.0, blue: 166.0/255.0, alpha: 1.0)
    }
}

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
