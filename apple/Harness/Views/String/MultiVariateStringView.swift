//
//  Copyright 2022 Harness
//

import SwiftUI

struct MultiVariateStringView: View {
    var body: some View {
        TabView {
            MultiVariateStringTesterView()
                .tabItem {
                    Label("Test", systemImage: "testtube.2")
                }
            MultiVariateStringYouTubeView()
                .tabItem {
                    Label("Video", systemImage: "video.fill")
                }
            MultiVariateStringWebView()
                .tabItem {
                    Label("Web", systemImage: "network")
                }
        }
    }
}

struct MultiVariateStringView_Previews: PreviewProvider {
    static var previews: some View {
        MultiVariateStringView()
    }
}
