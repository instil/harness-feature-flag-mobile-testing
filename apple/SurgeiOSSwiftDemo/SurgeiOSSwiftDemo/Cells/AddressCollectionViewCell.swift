//
//  AddressCollectionViewCell.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

class AddressCollectionViewCell: UICollectionViewCell {
    @IBOutlet weak var titleLabel: UILabel!
    @IBOutlet weak var subtitleLabel: UILabel!
    @IBOutlet weak var selectionButton: UIButton! {
        didSet {
            selectionButton.tintColor = UIColor.primaryTint
        }
    }
    @IBOutlet weak var selectionButtonViewContainer: UIView!
    @IBOutlet weak var primaryStackView: UIStackView!
    
    func toggleEditMode(_ isInEditMode: Bool, withDeletionToggled: Bool = false) {
        if isInEditMode {
            self.selectionButtonViewContainer.isHidden = false
            self.selectionButton.isHidden = false
        } else {
            self.selectionButtonViewContainer.isHidden = true
            self.selectionButton.isHidden = true
        }
        
        if withDeletionToggled {
            self.selectionButton.imageView?.image = UIImage(named: "circle-checked-icon")
        } else {
            self.selectionButton.imageView?.image = UIImage(named: "circle-outline-icon")
        }
    }
}
