//
//  AddAddressViewController.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

extension UIViewController {
    func showAlert(title: String, message: String) {
        let alert = UIAlertController(title: title, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "Okay", style: .default, handler: nil))
        self.present(alert, animated: true, completion: nil)
    }
}

enum FieldType: Int {
    case address = 0
    case name, username, password
    
    func asTitle() -> String {
        switch self {
        case .address: return "Address"
        case .name: return "Name"
        case .username: return "Username"
        case .password: return "Password"
        }
    }
}

class AddAddressViewController: UITableViewController {
    var streamDetails = RSTPStream()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.title = "Add Address"
        
        let doneButton = UIBarButtonItem(barButtonSystemItem: .done, target: self, action: #selector(AddAddressViewController.doneAction))
        self.navigationItem.rightBarButtonItem = doneButton

        self.tableView.register(UINib(nibName: "DataEntryTableViewCell", bundle: nil), forCellReuseIdentifier: "DataEntryTableViewCell")
        self.tableView.rowHeight = UITableViewAutomaticDimension
        self.tableView.estimatedRowHeight = UITableViewAutomaticDimension
    }
    
    @objc func doneAction() {
        // Basic validation for address
        guard self.streamDetails.address.hasPrefix("rtsp://") else {
            self.showAlert(title: "Invalid RTSP", message: "The RSTP stream adrress is invalid, please ensure you include the full url.")
            return
        }
        
        var existingStreams = RSTPStream.savedStreams()
        // Check for duplicated stream address
        guard existingStreams.filter({ $0.address == self.streamDetails.address }).count == 0 else {
            self.showAlert(title: "Already Added", message: "The stream address is already added as a saved stream.")
            return
        }
        
        existingStreams.append(self.streamDetails)
        RSTPStream.saveStreams(streams: existingStreams)
        self.navigationController?.popViewController(animated: true)
    }

    // MARK: - Table view data source

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return 4
    }

    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "DataEntryTableViewCell", for: indexPath) as! DataEntryTableViewCell
        guard let field = FieldType(rawValue: indexPath.row) else { fatalError("unable to map index to a field type") }
        cell.inputType = field
        cell.titleLabel.text = field.asTitle()
        cell.inputChanged = handleCellTextfieldInputChange
        
        if field == .address {
            cell.inputTextfield.placeholder = ""
            cell.inputTextfield.becomeFirstResponder()
        }
        
        return cell
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if let cell = tableView.cellForRow(at: indexPath) as? DataEntryTableViewCell {
            cell.inputTextfield.becomeFirstResponder()
        }
    }
    
    private func handleCellTextfieldInputChange(value: FieldType, userInput: String) {
        switch value {
        case .address: self.streamDetails.address = userInput
        case .name: self.streamDetails.name = userInput
        case .username: self.streamDetails.username = userInput
        case .password: self.streamDetails.password = userInput
        }
    }
}
