//
//  StreamCollectionViewCell.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

class SURTestFaceDetectionImageView: UIImageView {
    var timer = Timer()
    var faceDetectionEnabled = false {
        didSet {
            configureFaceDetection()
        }
    }
    
    func configureFaceDetection() {
        if faceDetectionEnabled {
            timer = Timer.scheduledTimer(timeInterval: 0.2, target: self, selector: #selector(self.detectFaces), userInfo: nil, repeats: true)
        } else {
            timer.invalidate()
            self.subviews.forEach { $0.removeFromSuperview() }
        }
    }
    
    @objc func detectFaces(){
        self.enableFaceDetection(inset: UIEdgeInsetsMake(5,5,5,5))
    }
}

class StreamCollectionViewCell: UICollectionViewCell {
    typealias CloseStreamCallback = (PlaybackStream) -> Void
    
    @IBOutlet weak var playingIndicatorImageView: UIImageView!
    @IBOutlet weak var titleLabel: UILabel!
    @IBOutlet weak var playerImageView: SURTestFaceDetectionImageView!
    @IBOutlet weak var removeButton: UIButton!
    @IBOutlet weak var faceToggleButton: UIButton! {
        didSet {
            faceToggleButton.tintColor = .white
        }
    }
    
    weak var stream: PlaybackStream?
    var streamSelectedToCloseCallback: CloseStreamCallback?

    override func awakeFromNib() {
        super.awakeFromNib()
        setUpGestureRecognizer()
        setupInterface(isPaused: false)
    }
    
    override func prepareForReuse() {
        super.prepareForReuse()
        self.playerImageView.image = nil
        self.stream = nil
    }
    
    private func configureStream() {
        guard let stream = stream else { return }
        stream.player.playerView = playerImageView
        titleLabel.text = stream.details.address
    }
    
    private func setUpGestureRecognizer() {
        let gestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(StreamCollectionViewCell.handleTap))
        playerImageView.addGestureRecognizer(gestureRecognizer)
        playerImageView.isUserInteractionEnabled = true
    }
    
    @objc private func handleTap() {
        if self.stream?.status == .playing {
            self.stream?.pause()
            setupInterface(isPaused: true)
        } else {
            self.stream?.play()
            setupInterface(isPaused: false)
        }
    }
    
    private func setupInterface(isPaused: Bool) {
        self.playingIndicatorImageView.image = UIImage(named: isPaused ? "play-icon": "pause-icon")
        self.playingIndicatorImageView.alpha = 1.0
        self.titleLabel.alpha = 1.0

        if !isPaused {
            UIView.animate(withDuration: 3.0) {
                self.playingIndicatorImageView.alpha = 0.0
                self.titleLabel.alpha = 0.0
            }
        }
    }
    
    @IBAction func closeStreamAction(_ sender: UIButton) {
        guard let stream = self.stream else { return }
        self.streamSelectedToCloseCallback?(stream)
    }
    
    @IBAction func faceDetectionAction(_ sender: UIButton) {
        self.playerImageView.faceDetectionEnabled = !self.playerImageView.faceDetectionEnabled
        if self.playerImageView.faceDetectionEnabled {
            sender.setImage(UIImage(named: "face-on-icon"), for: .normal)
        } else {
            sender.setImage(UIImage(named: "face-off-icon"), for: .normal)
        }
    }
}

extension StreamCollectionViewCell {
    func configureCellWithStream(_ stream: PlaybackStream, withRemoveCallback: @escaping (PlaybackStream) -> Void) {
        self.stream = stream
        self.streamSelectedToCloseCallback = withRemoveCallback
        configureStream()
    }
}
