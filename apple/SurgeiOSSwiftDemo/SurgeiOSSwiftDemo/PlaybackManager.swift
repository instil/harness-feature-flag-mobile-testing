//
//  PlaybackManager.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import Foundation
import SurgeiOS

class PlaybackManager: NSObject {
    private(set) var streams: [PlaybackStream] = []
    
    // MARK: API
    
    func startStreamWithAddress(_ address: RtspStream) {
        let stream = PlaybackStream(withRSTPStream: address)
        stream.start()
        self.streams.append(stream)
    }
    
    func removeStream(_ stream: PlaybackStream) {
        stream.stop()
        if let index = self.streams.index(of: stream) {
            self.streams[index].player.playerView = UIImageView()
            self.streams.remove(at: index)
        }
    }
}
