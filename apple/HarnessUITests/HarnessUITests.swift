//
//  Copyright 2022 Harness
//

import XCTest

final class HarnessUITests: XCTestCase {
    let app = XCUIApplication()

    override func setUpWithError() throws {
        continueAfterFailure = false

        app.launch()
    }

    override func tearDownWithError() throws { }

    func testMainViewNavigationLinksExists() throws {
        let harnessSdkToggle = app.switches["useSdkSwitch"]
        let booleanButton = app.buttons["BooleanButton"]
        let stringButton = app.buttons["StringButton"]
        let intButton = app.buttons["IntButton"]
        let jsonButton = app.buttons["JsonButton"]
        let sleepButton = app.buttons["SleepButton"]
        let closeButton = app.buttons["CloseButton"]
        
        XCTAssert(harnessSdkToggle.exists)
        XCTAssert(harnessSdkToggle.value as? String == "1")
        XCTAssert(booleanButton.exists)
        XCTAssert(stringButton.exists)
        XCTAssert(intButton.exists)
        XCTAssert(jsonButton.exists)
        XCTAssert(sleepButton.exists)
        XCTAssert(closeButton.exists)
    }
    
    func testRealHarnessSdkToggle() throws {
        let harnessSdkToggle = app.switches["useSdkSwitch"]

        harnessSdkToggle.tap()

        XCTAssert(harnessSdkToggle.value as? String == "0")

        harnessSdkToggle.tap()

        XCTAssert(harnessSdkToggle.value as? String == "1")
    }
    
    func testBooleanNavigationLinkNavigatesToBooleanTesterView() throws {
        let harnessSdkToggle = app.switches["useSdkSwitch"]
        harnessSdkToggle.tap()
        XCTAssert(harnessSdkToggle.value as? String == "0")
        
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
    
    func testBooleanOneNotExistsForRealSdk() throws {
        let harnessSdkToggle = app.switches["useSdkSwitch"]
        XCTAssert(harnessSdkToggle.value as? String == "1")
        
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
    
    func testStringNavigationLinkNavigatesToStringTesterView() throws {
        let stringButton = app.buttons["StringButton"]
        stringButton.tap()
        
        let featureFlagString = app.staticTexts["FeatureFlagString"]
        
        let exists = NSPredicate(format: "exists == 1")

        expectation(for: exists, evaluatedWith: featureFlagString, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
    }
    
    func testStringTabsWork() throws {
        let stringButton = app.buttons["StringButton"]
        stringButton.tap()
        
        let stringTesterTab = app.buttons["Test"]
        let stringYouTubeTab = app.buttons["Video"]
        let stringWebViewTab = app.buttons["Web"]
        
        XCTAssert(stringTesterTab.exists)
        XCTAssert(stringYouTubeTab.exists)
        XCTAssert(stringWebViewTab.exists)
        
        let exists = NSPredicate(format: "exists == 1")
        
        let featureFlagString = app.staticTexts["FeatureFlagString"]
        expectation(for: exists, evaluatedWith: featureFlagString, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
        
        stringYouTubeTab.tap()

        let youTubePlayerQuery = app.descendants(matching: .webView)
        let youTubePlayer = youTubePlayerQuery.element(boundBy: 0)
        expectation(for: exists, evaluatedWith: youTubePlayer, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
        
        stringWebViewTab.tap()
    
        let webViewQuery = app.descendants(matching: .webView)
        let webView = webViewQuery.element(boundBy: 0)
        expectation(for: exists, evaluatedWith: webView, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
        
        stringTesterTab.tap()
        expectation(for: exists, evaluatedWith: featureFlagString, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
    }
    
    func testIntNavigationLinkNavigatesToIntTesterView() throws {
        let intButton = app.buttons["IntButton"]
        intButton.tap()
        
        let helloWorld = app.staticTexts["Hello, World!"]
        let exists = NSPredicate(format: "exists == 1")

        expectation(for: exists, evaluatedWith: helloWorld, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
    }
    
    func testJsonNavigationLinkNavigatesToJsonTesterView() throws {
        let jsonButton = app.buttons["JsonButton"]
        jsonButton.tap()
        
        let helloWorld = app.staticTexts["Hello, World!"]
        
        let exists = NSPredicate(format: "exists == 1")

        expectation(for: exists, evaluatedWith: helloWorld, handler: nil)
        waitForExpectations(timeout: 5, handler: nil)
    }
    
    func testSleepButtonSuspendsApp() throws {
        let sleepButton = app.buttons["SleepButton"]
        sleepButton.tap()

        XCTAssert(app.exists)
        XCTAssert(!sleepButton.isHittable)
    }
    
    func testCloseButtonClosesApp() throws {
        let closeButton = app.buttons["CloseButton"]
        closeButton.tap()
        
        XCTAssert(!app.exists)
    }
}
