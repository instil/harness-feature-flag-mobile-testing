//
//  Copyright 2022 Harness
//

import XCTest

final class HarnessPerformanceTests: XCTestCase {
    typealias MetricAlloc = @convention(c) (XCTMetric.Type, Selector) -> NSObject
    typealias MetricInitWithProcessName = @convention(c) (NSObject, Selector, String) -> XCTMetric

    let app = XCUIApplication()

    var memoryMetric: XCTMetric = XCTMemoryMetric()
    var cpuMetric: XCTMetric = XCTCPUMetric()
    var clockMetric: XCTClockMetric = XCTClockMetric()
    var storageMetric: XCTStorageMetric = XCTStorageMetric()
    var measureOptions: XCTMeasureOptions = XCTMeasureOptions()

    override func setUpWithError() throws {
        continueAfterFailure = false

        app.launch()
        
        measureOptions = XCTMeasureOptions()
        measureOptions.iterationCount = 10
        
        if #available(iOS 14, *) {
            memoryMetric = self.initWithProcessName(for: XCTMemoryMetric.self, processName: "XCMetrics")
            cpuMetric = self.initWithProcessName(for: XCTCPUMetric.self, processName: "XCMetrics")
        } else {
            memoryMetric = XCTMemoryMetric(application: app)
            cpuMetric = XCTCPUMetric(application: app)
        }
        
        clockMetric = XCTClockMetric()
        storageMetric = XCTStorageMetric(application: app)
    }

    override func tearDownWithError() throws { }
    
    func testPerformanceNavigatingToBooleanTabWithRealSdk() {
        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let booleanButton = app.buttons["BooleanButton"]
            booleanButton.tap()
            
            let booleanOne = app.staticTexts["Boolean One"]
            let hidden = NSPredicate(format: "isHittable == 0")

            expectation(for: hidden, evaluatedWith: booleanOne, handler: nil)
            waitForExpectations(timeout: 5, handler: nil)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    func testPerformanceNavigatingToBooleanTabWithMockSdk() {
        enableMockSdkService()
        
        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let booleanButton = app.buttons["BooleanButton"]
            booleanButton.tap()
            
            let booleanOne = app.staticTexts["Boolean One"]
            XCTAssert(booleanOne.exists)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    func testPerformanceNavigatingToStringTabWithRealSdk() {
        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let stringButton = app.buttons["StringButton"]
            stringButton.tap()
            
            let featureFlagString = app.staticTexts["FeatureFlagString"]
            
            waitForElementExists(element: featureFlagString)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    func testPerformanceNavigatingToStringTabWithMockSdk() {
        enableMockSdkService()

        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let stringButton = app.buttons["StringButton"]
            stringButton.tap()
            
            let featureFlagString = app.staticTexts["FeatureFlagString"]
            
            waitForElementExists(element: featureFlagString)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    func testPerformanceNavigatingToYoutubeTabWithRealSdk() {
        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let stringButton = app.buttons["StringButton"]
            stringButton.tap()
            
            let youTubeTab = app.buttons["Video"]
            youTubeTab.tap()
            
            let youTubePlayer = getWebViewElement()
            waitForElementExists(element: youTubePlayer)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    func testPerformanceNavigatingToYoutubeTabWithMockSdk() {
        enableMockSdkService()

        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let stringButton = app.buttons["StringButton"]
            stringButton.tap()
            
            let youTubeTab = app.buttons["Video"]
            youTubeTab.tap()

            let youTubePlayer = getWebViewElement()

            waitForElementExists(element: youTubePlayer)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    func testPerformanceNavigatingToWebViewTabWithRealSdk() {
        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let stringButton = app.buttons["StringButton"]
            stringButton.tap()
            
            let webViewTab = app.buttons["Web"]
            webViewTab.tap()
            
            let webView = getWebViewElement()
            waitForElementExists(element: webView)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    func testPerformanceNavigatingToWebViewTabWithMockSdk() {
        enableMockSdkService()

        measure(metrics: [memoryMetric, cpuMetric, clockMetric, storageMetric], options: measureOptions) {
            let stringButton = app.buttons["StringButton"]
            stringButton.tap()
            
            let webViewTab = app.buttons["Web"]
            webViewTab.tap()

            let invalidUrl = app.staticTexts["URL is not valid, cannot load web view!"]
            waitForElementExists(element: invalidUrl)

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }

    func testGranularPerformanceNavigatingToBooleanTab() {
        measureMetrics([XCTPerformanceMetric.wallClockTime], automaticallyStartMeasuring: false) {
            startMeasuring()

            let booleanButton = app.buttons["BooleanButton"]
            booleanButton.tap()

            let booleanOne = app.staticTexts["Boolean One"]
            XCTAssert(booleanOne.exists)

            stopMeasuring()

            let backButton = app.buttons["Back"]
            backButton.tap()
        }
    }
    
    private func initWithProcessName(for type: XCTMetric.Type, processName: String) -> XCTMetric {
        guard type is XCTMemoryMetric.Type ||
              type is XCTCPUMetric.Type ||
              type is XCTStorageMetric.Type else {
            fatalError("Tried to create a metric that is not CPU, Storage or Memory")
        }
        let allocSelector = NSSelectorFromString("alloc")
        let allocIMP = method_getImplementation(class_getClassMethod(type.self, allocSelector)!)
        let allocMethod = unsafeBitCast(allocIMP, to: MetricAlloc.self)
        let result = allocMethod(type.self, allocSelector)
        
        let initSelector = NSSelectorFromString("initWithProcessName:")
        let methodIMP = result.method(for: initSelector)
        let initMethod = unsafeBitCast(methodIMP, to: MetricInitWithProcessName.self)
        return initMethod(result, initSelector, processName)
    }
    
    private func enableMockSdkService() {
        let harnessSdkToggle = app.switches["useSdkSwitch"]

        harnessSdkToggle.tap()

        XCTAssert(harnessSdkToggle.value as? String == "0")
    }
    
    private func waitForElementExists(element: XCUIElement) {
        let exists = NSPredicate(format: "exists == 1")

        expectation(for: exists, evaluatedWith: element, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
    }
    
    private func getWebViewElement() -> XCUIElement {
        let webViewQuery = app.descendants(matching: .webView)
        return webViewQuery.element(boundBy: 0)
    }
}
