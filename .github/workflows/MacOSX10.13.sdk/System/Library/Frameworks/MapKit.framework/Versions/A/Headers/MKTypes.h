//
//  MKTypes.h
//  MapKit
//
//  Copyright (c) 2009-2014, Apple Inc. All rights reserved.
//

#import <MapKit/MKFoundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MKMapType) {
    MKMapTypeStandard = 0,
    MKMapTypeSatellite,
    MKMapTypeHybrid,
    MKMapTypeSatelliteFlyover NS_ENUM_AVAILABLE(10_11, 9_0),
    MKMapTypeHybridFlyover NS_ENUM_AVAILABLE(10_11, 9_0),
    MKMapTypeMutedStandard NS_ENUM_AVAILABLE(10_13, 11_0) __TVOS_AVAILABLE(11_0),
} NS_ENUM_AVAILABLE(10_9, 3_0) __TVOS_AVAILABLE(9_2) __WATCHOS_PROHIBITED;

MK_EXTERN NSString *MKErrorDomain __TVOS_AVAILABLE(9_2);

typedef NS_ENUM(NSUInteger, MKErrorCode) {
    MKErrorUnknown = 1,
    MKErrorServerFailure,
    MKErrorLoadingThrottled,
    MKErrorPlacemarkNotFound,
    MKErrorDirectionsNotFound NS_ENUM_AVAILABLE(10_9, 7_0)
} NS_ENUM_AVAILABLE(10_9, 3_0) __TVOS_AVAILABLE(9_2) __WATCHOS_PROHIBITED;

typedef NS_ENUM(NSInteger, MKFeatureVisibility) {
    MKFeatureVisibilityAdaptive,
    MKFeatureVisibilityHidden,
    MKFeatureVisibilityVisible
} NS_AVAILABLE_IOS(11_0) __TVOS_AVAILABLE(11_0) __WATCHOS_PROHIBITED;

NS_ASSUME_NONNULL_END
