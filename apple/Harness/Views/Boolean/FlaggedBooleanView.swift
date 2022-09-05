//
//  Copyright 2022 Harness
//

import SwiftUI

struct FlaggedBooleanView: View {
    var body: some View {
        Text("If you are seeing this, then the associated feature flag is true!").padding()
    }
}

struct FlaggedBooleanView_Previews: PreviewProvider {
    static var previews: some View {
        FlaggedBooleanView()
    }
}
