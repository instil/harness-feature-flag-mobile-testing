//
//  Copyright 2022 Harness
//

import SwiftUI

struct FlaggedBooleanView: View {
    @StateObject private var viewModel = RealWorldBooleanUseViewModel()

    var body: some View {
        VStack(alignment: .center) {
            Text("Loading...")
                .accessibilityLabel("Loading")
                .padding()
                .opacity(viewModel.isLoading ? 1 : 0)
            Text("If you are seeing this, then the associated feature flag is true!")
                .accessibilityIdentifier("BooleanFeatureFlagText")
                .padding()
                .opacity(viewModel.boolean ? 1 : 0)
        }
    }
}

struct FlaggedBooleanView_Previews: PreviewProvider {
    static var previews: some View {
        FlaggedBooleanView()
    }
}
