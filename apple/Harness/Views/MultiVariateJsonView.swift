//
//  Copyright 2022 Harness
//

import SwiftUI

struct MultiVariateJsonView: View {
    @StateObject private var viewModel = MultiVariateJsonViewModel()

    var body: some View {
        VStack(alignment: .center) {
            Text("Loading...")
                .accessibilityLabel("Loading")
                .padding()
                .opacity(viewModel.isLoading ? 1 : 0)
            Text(viewModel.json)
                .padding()
                .opacity(viewModel.isLoading ? 0 : 1)
        }
    }
}

struct MultiVariateJsonView_Previews: PreviewProvider {
    static var previews: some View {
        MultiVariateJsonView()
    }
}
