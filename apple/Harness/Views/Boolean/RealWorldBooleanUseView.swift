//
//  Copyright 2022 Harness
//

import SwiftUI

struct RealWorldBooleanUseView: View {
    var body: some View {
        Image("InstilOffice")
            .resizable()
            .scaledToFit()
            .accessibilityIdentifier("BooleanImage")
    }
}

struct RealWorldBooleanUseView_Previews: PreviewProvider {
    static var previews: some View {
        RealWorldBooleanUseView()
    }
}
