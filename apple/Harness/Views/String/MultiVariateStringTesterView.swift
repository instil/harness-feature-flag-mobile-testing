//
//  Copyright 2022 Harness
//

import SwiftUI

struct MultiVariateStringTesterView: View {
    @StateObject private var viewModel = MultiVariateStringTesterViewModel()

    var body: some View {
        Text(viewModel.stringFeatureFlag)
            .accessibilityIdentifier("FeatureFlagString")
            .padding()
            .multilineTextAlignment(.center)
    }
}

struct MultiVariateStringTesterView_Previews: PreviewProvider {
    static var previews: some View {
        MultiVariateStringTesterView()
    }
}
