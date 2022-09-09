//
//  Copyright 2022 Harness
//

import SwiftUI

struct MainView: View {
    var body: some View {
        NavigationView {
            VStack {
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
            }
        }
    }
}

struct MainView_Previews: PreviewProvider {
    static var previews: some View {
        MainView()
    }
}
