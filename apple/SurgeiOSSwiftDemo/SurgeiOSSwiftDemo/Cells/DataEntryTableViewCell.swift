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
    
    @IBOutlet var titleLabel: UILabel!
    @IBOutlet var inputTextfield: UITextField!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        self.selectionStyle = .none
    }
    
    @IBAction func textfieldDidChangeAction(_ sender: UITextField) {
        guard let inputType = self.inputType else { return }
        self.inputChanged?(inputType, sender.text ?? "")
    }
}
