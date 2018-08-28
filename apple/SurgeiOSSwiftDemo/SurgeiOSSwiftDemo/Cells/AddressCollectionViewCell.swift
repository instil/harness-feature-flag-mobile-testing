//
//  AddressCollectionViewCell.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

class AddressCollectionViewCell: UICollectionViewCell {
    @IBOutlet var titleLabel: UILabel!
    @IBOutlet var subtitleLabel: UILabel!
    @IBOutlet var selectionButton: UIButton! {
        didSet {
            selectionButton.tintColor = UIColor.primaryTint
        }
    }
    @IBOutlet weak var selectionButtonViewContainer: UIView!
    
    func toggleEditMode(_ isInEditMode: Bool, withDeletionToggled: Bool = false) {
        self.selectionButtonViewContainer.isHidden = !isInEditMode
        self.selectionButton.isHidden = !isInEditMode
        
        let image = withDeletionToggled ? UIImage(named: "circle-checked-icon") : UIImage(named: "circle-outline-icon")
        self.selectionButton.imageView?.image = image
    }
}
