//
//  MutableCollectionExtensions.swift
//  SurgeiOSSwiftDemoApp
//
//  Created by Stevey Brown on 28/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import Foundation

extension MutableCollection where Element: Cacheable {
    func saveToCache() {
        let elements = self.map { $0 }
        let data = try! JSONEncoder().encode(elements)
        UserDefaults.standard.set(data, forKey: Element.cacheKey)
    }
    
    mutating func restoreFromCache() {
        if let savedData = UserDefaults.standard.data(forKey: Element.cacheKey),
            let savedElements = try! JSONDecoder().decode([Element].self, from: savedData) as? Self {
            self = savedElements
        }
    }
}
