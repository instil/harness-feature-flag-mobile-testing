//
//  RSTPAddress.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import Foundation

protocol Cacheable: Codable {
    static var cacheKey: String { get }
}

struct RtspStream: Cacheable, Equatable {
    static var cacheKey: String { return "Saved_Streams" }
    
    var address: String
    var name: String?
    var username: String?
    var password: String?
    
    init(address: String = "") {
        self.address = address
    }
}

