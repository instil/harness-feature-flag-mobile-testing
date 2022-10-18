//
//  Copyright 2022 Harness
//

import SwiftUI

struct RealWorldBooleanUseView: View {
    @StateObject private var viewModel = RealWorldBooleanUseViewModel()

    var body: some View {
        VStack(alignment: .leading) {
            Spacer()
            HStack {
                Spacer()
                Text("Loading...")
                    .accessibilityLabel("Loading")
                    .padding()
                    .opacity(viewModel.isLoading ? 1 : 0)
                Spacer()
            }
            Spacer()
            Image("InstilOffice")
                .resizable()
                .scaledToFit()
                .accessibilityIdentifier("BooleanImage")
                .opacity(viewModel.boolean ? 1 : 0)
            Spacer()
        }
    }
}

struct RealWorldBooleanUseView_Previews: PreviewProvider {
    static var previews: some View {
        RealWorldBooleanUseView()
    }
}
