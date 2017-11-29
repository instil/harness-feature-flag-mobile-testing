// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "AppDelegate.h"

#import "AddressesTableViewController.h"
#import "PlaybackViewController.h"

@interface AppDelegate () <UISplitViewControllerDelegate>
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window.tintColor = [UIColor whiteColor];
    [[UINavigationBar appearance] setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    
    if (@available(iOS 11.0, *)) {
        [[UITextField appearanceWhenContainedInInstancesOfClasses:@[[UISearchBar class]]] setDefaultTextAttributes:@{NSForegroundColorAttributeName:[UIColor darkGrayColor]}];
        [UITextField appearanceWhenContainedInInstancesOfClasses:@[[UISearchBar class]]].tintColor = [UIColor darkGrayColor];
    }
    
    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {
        UISplitViewController *svc = (UISplitViewController *)self.window.rootViewController;
        svc.delegate = self;
        [self checkSplitViewControllerRequiresHomeButton:svc forDisplayMode:svc.displayMode];
        UINavigationController *rightNav = (UINavigationController *)svc.viewControllers.lastObject;
        PlaybackViewController *playbackController = (PlaybackViewController *)rightNav.viewControllers.firstObject;
        UINavigationController *leftNav = (UINavigationController *)svc.viewControllers.firstObject;
        AddressesTableViewController *addressesController = (AddressesTableViewController *)leftNav.viewControllers.firstObject;
        addressesController.playbackManager = playbackController.playbackManager;
    }
    return YES;
}

- (void)checkSplitViewControllerRequiresHomeButton:(UISplitViewController *)svc forDisplayMode:(UISplitViewControllerDisplayMode)displayMode {
    UINavigationController *rightNav = (UINavigationController *)svc.viewControllers.lastObject;
    PlaybackViewController *playbackController = (PlaybackViewController *)rightNav.viewControllers.firstObject;
    UIBarButtonItem *button = (displayMode == UISplitViewControllerDisplayModePrimaryHidden ?
                               svc.displayModeButtonItem :
                               nil);
    playbackController.navigationItem.leftBarButtonItem = button;
}

#pragma mark - UISplitViewController

- (void)splitViewController:(UISplitViewController *)svc willChangeToDisplayMode:(UISplitViewControllerDisplayMode)displayMode {
    [self checkSplitViewControllerRequiresHomeButton:svc forDisplayMode:displayMode];
}

@end
