//
//  UIImageView.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 22/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit
import CoreImage
import CoreGraphics

extension UIImageView {
    func enableFaceDetection(inset: UIEdgeInsets) {
        self.subviews.forEach { $0.removeFromSuperview() }
        let accuracy = [CIDetectorAccuracy: CIDetectorAccuracyHigh]
        guard let image = self.image,
            let faceDetector = CIDetector(ofType: CIDetectorTypeFace, context: nil, options: accuracy) else {
                return
        }
        
        let faces = faceDetector.features(in: CIImage(image:image)!)
        guard let ciImageSize = CIImage(image:image)?.extent.size else { return }
        var transform = CGAffineTransform(scaleX: 1, y: -1)
        transform = transform.translatedBy(x: 0, y: -ciImageSize.height)
        
        for face in faces as! [CIFaceFeature] {
            var faceViewBounds = face.bounds.applying(transform)
            
            let viewSize = self.bounds.size
            let scale = min(viewSize.width / ciImageSize.width,
                            viewSize.height / ciImageSize.height)
            let offsetX = (viewSize.width - ciImageSize.width * scale) / 2
            let offsetY = (viewSize.height - ciImageSize.height * scale) / 2
            
            faceViewBounds.origin.x = max(0, faceViewBounds.origin.x - inset.left)
            faceViewBounds.origin.y = max(0, faceViewBounds.origin.y - inset.top)
            
            faceViewBounds.size.height = min(faceViewBounds.origin.y + faceViewBounds.size.height + inset.bottom + inset.top,(ciImageSize.height))
            faceViewBounds.size.height = faceViewBounds.size.height - faceViewBounds.origin.y
            
            faceViewBounds.size.width = min(faceViewBounds.origin.x + faceViewBounds.size.width + inset.left + inset.right, ciImageSize.width)
            faceViewBounds.size.width = faceViewBounds.size.width - faceViewBounds.origin.x
                        
            faceViewBounds = faceViewBounds.applying(CGAffineTransform(scaleX: scale, y: scale))
            faceViewBounds.origin.x += offsetX
            faceViewBounds.origin.y += offsetY
            
            let faceBox = UIView(frame: faceViewBounds)
            faceBox.layer.borderWidth = 2
            faceBox.layer.borderColor = UIColor.red.cgColor
            faceBox.backgroundColor = UIColor.clear

            self.addSubview(faceBox)
        }
    }
}
