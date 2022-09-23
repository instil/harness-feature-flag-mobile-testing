//
//  Copyright 2022 Harness
//

import SwiftUI
import ff_ios_client_sdk
  
struct BooleanTesterView: View {
    @StateObject private var viewModel = BooleanTesterViewModel()
    
    var body: some View {
        VStack(alignment: .leading) {
            Text("Boolean One")
                .accessibilityIdentifier("BooleanOne")
                .padding()
                .opacity(viewModel.booleanOne ? 1 : 0)
            Text("Boolean Two")
                .accessibilityIdentifier("BooleanTwo")
                .padding()
            Text("Boolean Three")
                .accessibilityIdentifier("BooleanThree")
                .padding()
            Text("Boolean Four")
                .accessibilityIdentifier("BooleanFour")
                .padding()
            Text("Boolean Five")
                .accessibilityIdentifier("BooleanFive")
                .padding()
        }.padding()
    }
}

struct BooleanTesterView_Previews: PreviewProvider {
    static var previews: some View {
        BooleanTesterView()
    }
}
