//
//  RSTPAddress.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import Foundation

struct RSTPStream: Codable {
    fileprivate static let SaveKey = "Saved_Streams"
    
    var address: String
    var name: String?
    var username: String?
    var password: String?
    
    init(address: String = "") {
        self.address = address
    }
    
    static func saveStreams(streams: [RSTPStream]) {
        let streamsData = try! JSONEncoder().encode(streams)
        UserDefaults.standard.set(streamsData, forKey: RSTPStream.SaveKey)
    }
    
    static func savedStreams() -> [RSTPStream] {
        if let savedStreamsData = UserDefaults.standard.data(forKey: RSTPStream.SaveKey) {
            let savedStreams = try! JSONDecoder().decode([RSTPStream].self, from: savedStreamsData)
            return savedStreams
        }
        return []
    }
}

