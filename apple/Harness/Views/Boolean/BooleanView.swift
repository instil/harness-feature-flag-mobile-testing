//
//  Copyright 2022 Harness
//

import SwiftUI

struct BooleanView: View {
    var body: some View {
        TabView {
            BooleanTesterView()
                .tabItem {
                    Label("Test", systemImage: "testtube.2")
                }
            RealWorldBooleanUseView()
                .tabItem {
                    Label("Real World", systemImage: "globe.europe.africa.fill")
                }
            FlaggedBooleanView()
                .tabItem {
                    Label("Flagged View", systemImage: "flag.fill")
                }
        }
    }
}

struct BooleanView_Previews: PreviewProvider {
    static var previews: some View {
        BooleanView()
    }
}
