//
//  INSearchCallHistoryIntentResponse.h
//  Intents
//
//  Copyright (c) 2016-2017 Apple Inc. All rights reserved.
//

#import <Intents/INIntentResponse.h>

@class INCallRecord;

typedef NS_ENUM(NSInteger, INSearchCallHistoryIntentResponseCode) {
    INSearchCallHistoryIntentResponseCodeUnspecified = 0,
    INSearchCallHistoryIntentResponseCodeReady,
    INSearchCallHistoryIntentResponseCodeContinueInApp,
    INSearchCallHistoryIntentResponseCodeFailure,
    INSearchCallHistoryIntentResponseCodeFailureRequiringAppLaunch,
    INSearchCallHistoryIntentResponseCodeFailureAppConfigurationRequired,
    INSearchCallHistoryIntentResponseCodeInProgress API_AVAILABLE(ios(11.0), watchos(4.0), macosx(10.13)),
    INSearchCallHistoryIntentResponseCodeSuccess API_AVAILABLE(ios(11.0), watchos(4.0), macosx(10.13)),
} API_AVAILABLE(ios(10.0), watchos(3.2), macosx(10.12));

NS_ASSUME_NONNULL_BEGIN

API_AVAILABLE(ios(10.0), watchos(3.2), macosx(10.12))
@interface INSearchCallHistoryIntentResponse : INIntentResponse

- (id)init NS_UNAVAILABLE;

// The app extension has the option of capturing its private state as an NSUserActivity and returning it as the 'currentActivity'.
// If the the app is launched, an NSUserActivity will be passed in with the private state.  The NSUserActivity may also be used to query the app's UI extension (if provided) for a view controller representing the current intent handling state.
// In the case of app launch, the NSUserActivity will have its activityType set to the name of the intent. This intent object will also be available in the NSUserActivity.interaction property.
- (instancetype)initWithCode:(INSearchCallHistoryIntentResponseCode)code userActivity:(nullable NSUserActivity *)userActivity NS_DESIGNATED_INITIALIZER;

@property (readonly, NS_NONATOMIC_IOSONLY) INSearchCallHistoryIntentResponseCode code;

@property (readwrite, copy, nullable, NS_NONATOMIC_IOSONLY) NSArray<INCallRecord *> *callRecords API_AVAILABLE(ios(11.0), macosx(10.13));

@end

NS_ASSUME_NONNULL_END
