//
//  Copyright 2022 Harness
//

import XCTest

class HarnessLaunchPerfomanceTests: XCTestCase {
    let app = XCUIApplication()

    override func setUpWithError() throws {
        continueAfterFailure = false
    }

    func testLaunchPerformance() throws {
        measure(metrics: [XCTApplicationLaunchMetric()]) {
            app.launch()
        }
    }
    
    func testLaunchPerformanceUntilResponsive() throws {
        guard #available(iOS 14, *) else { return }

        measure(metrics: [XCTApplicationLaunchMetric(waitUntilResponsive: true)]) {
          app.launch()
          app.activate()
        }
    }
}
