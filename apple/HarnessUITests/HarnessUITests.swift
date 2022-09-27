//
//  Copyright 2022 Harness
//

import XCTest

final class HarnessUITests: XCTestCase {
    typealias MetricAlloc = @convention(c) (XCTMetric.Type, Selector) -> NSObject
    typealias MetricInitWithProcessName = @convention(c) (NSObject, Selector, String) -> XCTMetric
    
    let app = XCUIApplication()

    override func setUpWithError() throws {
        continueAfterFailure = false

        app.launch()
    }

    override func tearDownWithError() throws { }

    func testBooleanNavigationLinkExists() throws {
        let booleanButton = app.buttons["BooleanButton"]
        
        XCTAssert(booleanButton.exists)
    }
    
    func testBooleanNavigationLinkNavigatesToBooleanTesterView() throws {
        let booleanButton = app.buttons["BooleanButton"]
        booleanButton.tap()
        
        let booleanOne = app.staticTexts["Boolean One"]
        let booleanTwo = app.staticTexts["Boolean Two"]
        let booleanThree = app.staticTexts["Boolean Three"]
        let booleanFour = app.staticTexts["Boolean Four"]
        let booleanFive = app.staticTexts["Boolean Five"]
        
        XCTAssert(booleanOne.exists)
        XCTAssert(booleanTwo.exists)
        XCTAssert(booleanThree.exists)
        XCTAssert(booleanFour.exists)
        XCTAssert(booleanFive.exists)
    }
    
    func testBooleanOneNotExists() throws {
        let booleanButton = app.buttons["BooleanButton"]
        booleanButton.tap()
        
        let booleanOne = app.staticTexts["Boolean One"]
        
        let hidden = NSPredicate(format: "isHittable == 0")

        expectation(for: hidden, evaluatedWith: booleanOne, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
    }
    
    func testBooleanTabsWork() throws {
        let booleanButton = app.buttons["BooleanButton"]
        booleanButton.tap()
        
        let booleanTesterTab = app.buttons["Test"]
        let booleanRealWorldTab = app.buttons["Real World"]
        let booleanFlagTab = app.buttons["Flagged View"]
        
        XCTAssert(booleanTesterTab.exists)
        XCTAssert(booleanRealWorldTab.exists)
        XCTAssert(booleanFlagTab.exists)
        
        let booleanOne = app.staticTexts["Boolean One"]
        XCTAssert(booleanOne.exists)
        
        booleanRealWorldTab.tap()
        
        let realWorldImage = app.images["InstilOffice"]
        XCTAssert(realWorldImage.exists)
        
        booleanFlagTab.tap()
        
        let flaggedText = app.staticTexts["BooleanFeatureFlagText"]
        XCTAssert(flaggedText.exists)
        
        booleanTesterTab.tap()
    }
    
    func testPerformanceNavigatingToBooleanTab() {
        let measureOptions = XCTMeasureOptions()
        measureOptions.iterationCount = 10
        
        let memoryMetric: XCTMetric
        let cpuMetric: XCTMetric
        if #available(iOS 14, *) {
            memoryMetric = self.initWithProcessName(for: XCTMemoryMetric.self, processName: "XCMetrics")
            cpuMetric = self.initWithProcessName(for: XCTCPUMetric.self, processName: "XCMetrics")
        } else {
            memoryMetric = XCTMemoryMetric(application: app)
            cpuMetric = XCTCPUMetric(application: app)
        }
        
        let clockMetric = XCTClockMetric()
        let storageMetric = XCTStorageMetric(application: app)

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

    func testGranularPerformanceNavigatingToBooleanTAb() {
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
            fatalError("CPU, Storage and Memory metric implements initWithProcessName:")
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
}
