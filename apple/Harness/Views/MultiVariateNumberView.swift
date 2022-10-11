//
//  Copyright 2022 Harness
//

import SwiftUI

struct MultiVariateNumberView: View {
    @StateObject private var viewModel = MultiVariateNumberViewModel()

    var body: some View {
        VStack(alignment: .center) {
            Text("Loading...")
                .accessibilityLabel("Loading")
                .padding()
                .opacity(viewModel.isLoading ? 1 : 0)
            Text("\(viewModel.number ?? 0)!")
                .padding()
                .opacity(viewModel.number == nil ? 0 : 1)
        }
    }
}

struct MultiVariateNumberView_Previews: PreviewProvider {
    static var previews: some View {
        MultiVariateNumberView()
    }
}
