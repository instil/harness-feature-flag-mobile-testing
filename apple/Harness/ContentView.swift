//
//  Copyright 2022 Harness
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack(alignment: .leading) {
            Text("If you can see this, the boolean feature flag is true").padding()
            Text("The string multi-variate contains the value: ").padding()
            Text("The int multi-variate contains the value: ").padding()
            Text("The JSON multi-variate contains the value: ").padding()
        }.padding()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
