//
//  Copyright 2022 Harness
//

import SwiftUI
import YouTubePlayerKit

struct MultiVariateStringYouTubeView: View {
    @StateObject private var viewModel = MultiVariateStringYouTubeViewModel()
    
    var body: some View {
        ZStack {
            YouTubePlayerView(YouTubePlayer(stringLiteral: viewModel.youtubeUrl))
                .frame(height: 220)
                .opacity(viewModel.youtubeUrlIsValid ? 1 : 0)
                .accessibilityIdentifier("YouTubePlayer")
            Text("Loading...")
                .opacity(viewModel.isLoading ? 1 : 0)
            Text("Error! The URL received from Harness is not a valid YouTube URL!")
                .opacity(viewModel.youtubeUrlIsValid || viewModel.isLoading ? 0 : 1)
        }.padding()
    }
}

struct MultiVariateStringYouTubeView_Previews: PreviewProvider {
    static var previews: some View {
        MultiVariateStringYouTubeView()
    }
}
