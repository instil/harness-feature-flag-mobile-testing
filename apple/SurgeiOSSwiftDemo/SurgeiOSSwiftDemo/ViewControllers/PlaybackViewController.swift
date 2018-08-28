//
//  PlaybackViewController.swift
//  SurgeiOSSwiftDemo
//
//  Created by Stevey Brown on 21/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

import UIKit

/// Custom Layout for playback collection view
class PlaybackLayout: UICollectionViewFlowLayout {
    override func prepare() {
        super.prepare()
        self.sectionInset = UIEdgeInsets.zero
        self.sectionInsetReference = .fromSafeArea
        self.minimumLineSpacing = 0.0
        self.minimumInteritemSpacing = 0.0

        guard let collectionView = self.collectionView else { return }
        
        if collectionView.numberOfItems(inSection: 0) == 1 {
            self.itemSize = CGSize(width: collectionView.bounds.width - (self.sectionInset.left + self.sectionInset.right) , height: collectionView.bounds.height)
        } else if collectionView.numberOfItems(inSection: 0) == 2 {
            self.itemSize = CGSize(width: collectionView.bounds.width - (self.sectionInset.left + self.sectionInset.right), height: collectionView.bounds.height / 2)
        } else {
            self.itemSize = CGSize(width: collectionView.bounds.width / 2, height: collectionView.bounds.height / 2)
        }
    }
}

class PlaybackViewController: UIViewController {
    @IBOutlet var collectionView: UICollectionView! {
        didSet {
            collectionView.register(UINib(nibName: "StreamCollectionViewCell", bundle: nil), forCellWithReuseIdentifier: "StreamCollectionViewCell")
            collectionView.dataSource = self
            collectionView.collectionViewLayout = PlaybackLayout()
        }
    }
    
    private var manager = PlaybackManager()
    
    //MARK: View lifecycle
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.title = "Surge"
        self.view.backgroundColor = .black
        
        let addButton = UIBarButtonItem(barButtonSystemItem: .add, target: self, action: #selector(PlaybackViewController.addStream))
        self.navigationItem.rightBarButtonItem = addButton
        
        // Populate with two sample streams
        #if DEBUG
        let address1 = RtspStream(address: "rtsp://184.72.239.149/vod/mp4:BigBuckBunny_175k.mov")
        let address2 = RtspStream(address: "rtsp://webcam01.bigskyresort.com:554/axis-media/media.amp")
        
        manager.startStreamWithAddress(address1)
        manager.startStreamWithAddress(address2)
        #endif
    }
    
    //MARK: Segue
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if let vc = (segue.destination as? UINavigationController)?.topViewController as? StoredAddressesViewController {
            vc.delegate = self
        }
    }
    
    //MARK: Actions
    @objc func addStream() {
        self.performSegue(withIdentifier: "addStreamAddress", sender: self)
    }
    
    func removeStream(_ stream: PlaybackStream) {
        self.manager.removeStream(stream)
        self.collectionView.reloadSections(IndexSet(integer: 0))
    }
}

//MARK: Collection view data source
extension PlaybackViewController: UICollectionViewDataSource {    
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        if self.manager.streams.count == 0 {
            collectionView.configureEmptyDataSourceMessage("No streams selected, \n tap + to add a new stream.", render: true)
        } else {
            collectionView.configureEmptyDataSourceMessage(render: false)
        }
        return self.manager.streams.count
    }
    
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "StreamCollectionViewCell", for: indexPath) as! StreamCollectionViewCell
        cell.configureCellWithStream(manager.streams[indexPath.row], withRemoveCallback: removeStream)
        cell.contentView.isUserInteractionEnabled = false
        return cell
    }
}

// MARK: Stored addresses view controller delegate
extension PlaybackViewController: StoredAddressesViewControllerDelegate {
    func selectedStream(_ stream: RtspStream) {
        manager.startStreamWithAddress(stream)
        self.collectionView.reloadSections(IndexSet(integer: 0))
    }
}
