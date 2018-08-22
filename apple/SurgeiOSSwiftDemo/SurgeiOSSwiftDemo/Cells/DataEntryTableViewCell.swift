//
//  DataEntryTableViewCell.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

class DataEntryTableViewCell: UITableViewCell {
    var inputType: FieldType?
    // Callback for when textfield changes
    var inputChanged: ((FieldType, String) -> Void)?
    
    @IBOutlet weak var titleLabel: UILabel!
    @IBOutlet weak var inputTextfield: UITextField! {
        didSet {
            inputTextfield.tintColor = .black
            inputTextfield.addTarget(self, action: #selector(textFieldDidChange(textField:)), for: .editingChanged)
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
        self.selectionStyle = .none
    }
    
    @objc private func textFieldDidChange(textField: UITextField){
        guard let inputType = self.inputType else {
            fatalError("Input Type must be set during cell creation")
        }
        self.inputChanged?(inputType, textField.text ?? "")
    }
}
