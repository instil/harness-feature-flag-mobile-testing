//
//  Copyright 2022 Harness
//

import SwiftUI

struct BooleanTesterView: View {
    var body: some View {
        VStack(alignment: .leading) {
            Text("Boolean One")
                .accessibilityIdentifier("BooleanOne")
                .padding()
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
