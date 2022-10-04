//
//  Copyright 2022 Harness
//

import SwiftUI

struct MainView: View {
    @StateObject private var viewModel = MainViewModel()

    var body: some View {
        NavigationView {
            VStack {
                Toggle("Using Harness SDK", isOn: $viewModel.useRealService)
                    .frame(width: 220)
                    .padding(.top, 50)
                Spacer()
                NavigationLink(destination: BooleanView()) {
                    Image(systemName: "switch.2")
                    Text("Booleans")
                }.accessibilityIdentifier("BooleanButton")
                    .padding()
                NavigationLink(destination: MultiVariateStringView()) {
                    Image(systemName: "quote.opening")
                    Text("Multi-variate Strings")
                }.accessibilityIdentifier("StringButton")
                    .padding()
                NavigationLink(destination: MultiVariateNumberView()) {
                    Image(systemName: "number")
                    Text("Multi-variate Integers")
                }.accessibilityIdentifier("IntButton")
                    .padding()
                NavigationLink(destination: MultiVariateJsonView()) {
                    Image(systemName: "doc")
                    Text("Multi-variate JSON")
                }.accessibilityIdentifier("JsonButton")
                    .padding()
                Spacer()
                Button("\(Image(systemName: "powersleep")) Suspend App", action: viewModel.suspendApp)
                Button("\(Image(systemName: "exclamationmark.triangle.fill")) Close App", action: viewModel.closeApp)
                    .foregroundColor(.red)
                    .padding()
            }
        }
    }
}

struct MainView_Previews: PreviewProvider {
    static var previews: some View {
        MainView()
    }
}
