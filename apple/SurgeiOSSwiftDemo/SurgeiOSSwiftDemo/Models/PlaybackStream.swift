//
//  PlaybackStream.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import Foundation
import SurgeiOS

class PlaybackStream: NSObject, SurgeRtspPlayerDelegate {
    enum PlayingState {
        case playing, paused, stopped, none
    }
    
    private(set) var player = SurgeRtspPlayer()
    private(set) var details: RtspStream
    
    var status = PlayingState.none
    
    init(withRSTPStream details: RtspStream) {
        self.details = details
        super.init()
        self.player.delegate = self
    }
    
    //MARK: API
    
    func start() {
        guard let url = URL(string: self.details.address) else { return }
        DispatchQueue.global(qos: .background).async {
            if let username = self.details.username, let password = self.details.password {
                self.player.initiatePlayback(of: url, withUsername: username, andPassword: password)
            } else {
                self.player.initiatePlayback(of: url)
            }
            self.status = .playing
        }
    }
    
    func play() {
        DispatchQueue.global(qos: .background).async {
            self.player.play()
            self.status = .playing
        }
    }
    
    func stop() {
        DispatchQueue.global(qos: .background).async {
            self.player.stop()
            self.status = .stopped
        }
    }
    
    func pause() {
        DispatchQueue.global(qos: .background).async {
            self.player.pause()
            self.status = .paused
        }
    }
    
    //MARK: SurgeRtspPlayerDelegate
    func rtspPlayerInitiatedPlayback(_ player: SurgeRtspPlayer) {
        print("Initiated Playback!!")
    }
    
    func rtspPlayerDidTimeout(_ player: SurgeRtspPlayer) {
        print("Did timeout!!")
    }
    
    func rtspPlayerDidBeginPlayback(_ player: SurgeRtspPlayer) {
        print("Did begin playback!!")
    }
}
