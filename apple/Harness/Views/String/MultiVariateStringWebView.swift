//
//  Copyright 2022 Harness
//

import SwiftUI
import WebKit

struct MultiVariateStringWebView: View {
    @StateObject private var viewModel = MultiVariateStringWebViewModel()

    var body: some View {
        if let unwrappedUrl = viewModel.url {
            WebView(request: URLRequest(url: unwrappedUrl))
                .accessibilityIdentifier("WebView")
        } else if viewModel.isLoading {
            Text("Loading...")
        } else {
            Text("URL is not valid, cannot load web view!")
        }
    }
}

struct MultiVariateStringWebView_Previews: PreviewProvider {
    static var previews: some View {
        MultiVariateStringWebView()
    }
}
