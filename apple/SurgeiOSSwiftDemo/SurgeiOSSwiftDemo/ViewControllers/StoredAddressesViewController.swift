//
//  AddressesViewController.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

class FullWidthVerticalLayout: UICollectionViewFlowLayout {
    override func prepare() {
        guard let cv = self.collectionView else { return }
        self.sectionInset = UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0)
        self.sectionInsetReference = .fromSafeArea
        self.minimumLineSpacing = 0.0
        self.minimumInteritemSpacing = 0.0
        let offsets = (self.sectionInset.left + self.sectionInset.right) + (cv.contentInset.left + cv.contentInset.right)
        self.itemSize = CGSize(width: cv.bounds.width - offsets , height: 45)
    }
}

protocol StoredAddressesViewControllerDelegate: class  {
    func selectedStream(_ stream: RSTPStream)
}

class StoredAddressesViewController: UIViewController {
    enum ViewingMode {
        case displaying, editing, searching
    }
    
    @IBOutlet weak var collectionView: UICollectionView! {
        didSet {
            collectionView.register(UINib(nibName: "AddressCollectionViewCell", bundle: nil), forCellWithReuseIdentifier: "AddressCollectionViewCell")
            collectionView.collectionViewLayout = FullWidthVerticalLayout()
            collectionView.dataSource = self
            collectionView.delegate = self
        }
    }
    @IBOutlet weak var toolbar: UIToolbar! {
        didSet {
            toolbar.tintColor = UIColor.primaryTint
        }
    }
    @IBOutlet weak var toolbarBottomConstraint: NSLayoutConstraint!
    
    var addresses: [RSTPStream] = [] {
        didSet {
            self.collectionView.reloadData()
        }
    }
    var viewState = ViewingMode.displaying {
        didSet {
            configureViewForState(viewState)
        }
    }
    var searchedAddresses: [RSTPStream] = []
    var toBeDeletedAddressesCache: [String] = []
    var cancelButton: UIBarButtonItem!
    var addButton: UIBarButtonItem!
    var editButton: UIBarButtonItem!
    
    weak var delegate: StoredAddressesViewControllerDelegate?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.title = "Stored Addresses"
        
        cancelButton = UIBarButtonItem(barButtonSystemItem: .cancel, target: self, action: #selector(StoredAddressesViewController.dismissViewAction))
        addButton = UIBarButtonItem(barButtonSystemItem: .add, target: self, action: #selector(StoredAddressesViewController.addAddressAction))
        editButton = UIBarButtonItem(title: "Edit", style: .plain, target: self, action: #selector(StoredAddressesViewController.toggleToolbarAction))

        self.navigationItem.leftBarButtonItem = cancelButton
        self.navigationItem.rightBarButtonItems = [addButton, editButton]
        
        hideToolbar()
        setUpSearchController()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        self.addresses = RSTPStream.savedStreams()
    }
    
    private func setUpSearchController() {
        let searchController = UISearchController(searchResultsController: nil)
        if let textfield = searchController.searchBar.value(forKey: "searchField") as? UITextField {
            textfield.tintColor = .black
            if let backgroundview = textfield.subviews.first {
                backgroundview.backgroundColor = UIColor.white
                backgroundview.layer.cornerRadius = 10;
                backgroundview.clipsToBounds = true;
            }
        }
        
        searchController.searchBar.tintColor = .white
        searchController.searchResultsUpdater = self
        searchController.dimsBackgroundDuringPresentation = false
        navigationItem.searchController = searchController
        navigationItem.hidesSearchBarWhenScrolling = false
        self.definesPresentationContext = true
    }
    
    private func configureViewForState(_ state: ViewingMode) {
        switch state {
        case .editing:
            self.editButton.title = "Cancel"
            showToolbar()
            self.navigationItem.leftBarButtonItem = nil
            self.navigationItem.rightBarButtonItems = [editButton]
        case .displaying:
            self.editButton.title = "Edit"
            hideToolbar()
            self.navigationItem.leftBarButtonItem = cancelButton
            self.navigationItem.rightBarButtonItems = [addButton, editButton]
        case .searching:
            break
        }
    }
    
    private func showToolbar() {
        self.toolbarBottomConstraint.constant = 0
        UIView.animate(withDuration: 0.5) {
            self.view.layoutIfNeeded()
        }
    }
    
    private func hideToolbar() {
        let frame = self.toolbar.frame
        self.toolbarBottomConstraint.constant = -(frame.height + (UIApplication.shared.keyWindow?.safeAreaInsets.bottom)!)
        UIView.animate(withDuration: 0.5) {
            self.view.layoutIfNeeded()
        }
    }
    
    //MARK: Actions
    
    @objc func dismissViewAction() {
        self.dismiss(animated: true, completion: nil)
    }
    
    @objc func addAddressAction() {
        self.performSegue(withIdentifier: "addAddress", sender: self)
    }
    
    @IBAction func deleteAddressesAction(_ sender: UIBarButtonItem) {
        self.addresses = self.addresses.filter { !toBeDeletedAddressesCache.contains($0.address) }
        self.viewState = .displaying
        self.collectionView.reloadData()
        
        // Update storage on disk
        RSTPStream.saveStreams(streams: self.addresses)
    }
    
    @objc func toggleToolbarAction() {
        if self.viewState == .displaying {
            self.viewState = .editing
        } else {
            self.viewState = .displaying
        }
        self.toBeDeletedAddressesCache.removeAll()
        self.collectionView.reloadData()
    }
}

//MARK: Search Controller delegate
extension StoredAddressesViewController: UISearchResultsUpdating {
    func updateSearchResults(for searchController: UISearchController) {
        if searchController.isActive {
            viewState = .searching
            guard let searchTerm = searchController.searchBar.text?.lowercased() else { return }
            
            if searchTerm.count == 0 {
                self.searchedAddresses = self.addresses
            } else {
                self.searchedAddresses = self.addresses.filter {
                    $0.address.lowercased().range(of: searchTerm) != nil ||
                    $0.name?.lowercased().range(of: searchTerm) != nil
                }
            }
        } else {
            // Cancel button was selected on searchController
            viewState = .displaying
        }
        self.collectionView.reloadData()
    }
}

//MARK: Collection view datasource
extension StoredAddressesViewController: UICollectionViewDataSource {
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        let itemsForSectionCount = viewState == .searching ? self.searchedAddresses.count : self.addresses.count

        if viewState == .searching && itemsForSectionCount == 0 {
            collectionView.configureEmptyDataSourceMessage("No matching streams", render: true)
        } else if viewState == .displaying && itemsForSectionCount == 0 {
            collectionView.configureEmptyDataSourceMessage("No streams saved, \n tap + to create a new stream", render: true)
        } else {
            collectionView.configureEmptyDataSourceMessage(render: false)
        }
        
        return itemsForSectionCount
    }
    
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "AddressCollectionViewCell", for: indexPath) as! AddressCollectionViewCell
        let streams = viewState == .searching ? self.searchedAddresses : self.addresses
        let stream = streams[indexPath.row]
        
        cell.titleLabel.text = stream.address
        cell.subtitleLabel.text = stream.name ?? ""
        
        // Check if we are in edit mode, if so we determine if the cell
        // has already been toggled to be removed and style accordingly
        if viewState == .editing {
            UIView.animate(withDuration: 0.3) { cell.selectionButtonViewContainer.isHidden = false }
            if toBeDeletedAddressesCache.contains(stream.address) {
                cell.toggleEditMode(true, withDeletionToggled: true)
            } else {
                cell.toggleEditMode(true, withDeletionToggled: false)
            }
        } else {
            // Default by always hiding the delete toggle
            cell.toggleEditMode(false)
        }
        
        return cell
    }
}

//MARK: Collection view delegate
extension StoredAddressesViewController: UICollectionViewDelegate {
    func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        if viewState == .editing {
            guard let cell = collectionView.cellForItem(at: indexPath) as? AddressCollectionViewCell else { return }
            let stream = self.addresses[indexPath.row]
            // Determine if the cell being tapped has already been added to the delete cache
            if toBeDeletedAddressesCache.contains(stream.address) {
                if let index = toBeDeletedAddressesCache.index(of: stream.address) {
                    toBeDeletedAddressesCache.remove(at: index)
                    cell.toggleEditMode(true, withDeletionToggled: false)
                }
            } else {
                toBeDeletedAddressesCache.append(stream.address)
                cell.toggleEditMode(true, withDeletionToggled: true)
            }
        } else {
            let selectedStream = self.addresses[indexPath.row]
            self.delegate?.selectedStream(selectedStream)
            self.dismissViewAction()
        }
    }
}
