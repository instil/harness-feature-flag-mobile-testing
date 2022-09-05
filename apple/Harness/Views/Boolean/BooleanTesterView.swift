//
//  Copyright 2022 Harness
//

import SwiftUI

struct BooleanTesterView: View {
    var body: some View {
        VStack(alignment: .leading) {
            Text("Boolean One").padding()
            Text("Boolean Two").padding()
            Text("Boolean Three").padding()
            Text("Boolean Four").padding()
            Text("Boolean Five").padding()
        }.padding()
    }
}

struct BooleanTesterView_Previews: PreviewProvider {
    static var previews: some View {
        BooleanTesterView()
    }
}
