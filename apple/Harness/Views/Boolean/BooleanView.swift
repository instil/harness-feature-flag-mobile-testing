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
                .accessibilityIdentifier("BooleanTesterTab")
            RealWorldBooleanUseView()
                .tabItem {
                    Label("Real World", systemImage: "globe.europe.africa.fill")
                }
                .accessibilityIdentifier("BooleanRealWorldTab")
            FlaggedBooleanView()
                .tabItem {
                    Label("Flagged View", systemImage: "flag.fill")
                }
                .accessibilityIdentifier("BooleanFeatureFlagTab")
        }
    }
}

struct BooleanView_Previews: PreviewProvider {
    static var previews: some View {
        BooleanView()
    }
}
