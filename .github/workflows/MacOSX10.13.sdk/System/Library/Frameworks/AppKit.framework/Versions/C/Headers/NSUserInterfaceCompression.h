/*
 NSUserInterfaceCompression.h
 Application Kit
 Copyright (c) 2015-2016, Apple Inc.
 All rights reserved.
 */

#import <Foundation/NSObjCRuntime.h>
#import <Foundation/NSGeometry.h>
#import <Foundation/NSArray.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSUserInterfaceCompressionOptions : NSObject <NSCopying, NSCoding> {
    NSSet *_identifiers;
}

/*
 Returns an option with an empty set of identifiers. This represents the absence of any options.
 */
- (instancetype)init NS_DESIGNATED_INITIALIZER;

/*
 -initWithCoder: is a designated initializer
 */
- (instancetype)initWithCoder:(NSCoder *)coder NS_DESIGNATED_INITIALIZER;

/*
 Returns a singleton option with the given identifier.
*/
- (instancetype)initWithIdentifier:(NSString *)identifier NS_DESIGNATED_INITIALIZER;

/*
 Returns a singleton option which is the union of the given set of options.
 */
- (instancetype)initWithCompressionOptions:(NSSet<NSUserInterfaceCompressionOptions *> *)options NS_DESIGNATED_INITIALIZER;

/*
 Returns YES if the all the given options are present in the receiver's options.
 */
- (BOOL)containsOptions:(NSUserInterfaceCompressionOptions *)options;

/*
 Returns YES if at least one of the given options are present in the receiver's options.
 */
- (BOOL)intersectsOptions:(NSUserInterfaceCompressionOptions *)options;

/*
 Returns YES if the option is equivalent to the empty set option.
 */
@property (readonly, getter=isEmpty) BOOL empty;

/*
 Returns a new NSCompressibleUserInterfaceOptions object which is the union of the caller and the provided options.
 */
- (NSUserInterfaceCompressionOptions *)optionsByAddingOptions:(NSUserInterfaceCompressionOptions *)options;

/*
 Returns a new NSCompressibleUserInterfaceOptions object with the given options removed from the receiver's set of options.
 */
- (NSUserInterfaceCompressionOptions *)optionsByRemovingOptions:(NSUserInterfaceCompressionOptions *)options;

/*
 Option specifying views should hide their images if applicable.
 */
@property (class, readonly, copy) NSUserInterfaceCompressionOptions *hideImagesOption;

/*
 Option specifying views should hide their text if applicable.
 */
@property (class, readonly, copy) NSUserInterfaceCompressionOptions *hideTextOption;

/*
 Option specifying views should reduce their internal metrics when possible to reduce padding around contents. The system will handle this for system controls when available.
 */
@property (class, readonly, copy) NSUserInterfaceCompressionOptions *reduceMetricsOption;

/*
 Option specifying views will no longer maintain equal widths. This is handled by the system and no action is required by the views.
 */
@property (class, readonly, copy) NSUserInterfaceCompressionOptions *breakEqualWidthsOption;

/*
 Option which is the composite of all standard options provided by AppKit.
 */
@property (class, readonly, copy) NSUserInterfaceCompressionOptions *standardOptions;

@end

/*
 NSButton, NSSegmentedControl, NSSliderTouchBarItem currently conform to this protocol
 */
@protocol NSUserInterfaceCompression
@required
/*
 Called when the view should apply the provided compression options, if applicable.
 Options handled by the system are not included.
 Subclasses that do not recognize a provided option should provide that option in a call to super.
 In case of conflict where one or more options are mutually exclusive, they should be satisfied in the order given.
 Options that are not supported by the client can be left unhandled.
 */
- (void)compressWithPrioritizedCompressionOptions:(NSArray<NSUserInterfaceCompressionOptions *> *)prioritizedOptions;

/*
 Returns the minimum size alignment rect a view can achieve by applying the provided compression options.
 Options handled by the system are not included.
 Subclasses that do not recognize a provided option should provide that option in a call to super.
 In case of conflict where one or more options are mutually exclusive, they should be satisfied in the order given.
 */
- (NSSize)minimumSizeWithPrioritizedCompressionOptions:(NSArray<NSUserInterfaceCompressionOptions *> *)prioritizedOptions;

/*
 Returns the NSCompressibleUserInterfaceOptions currently applied to the view.
 Only options that have been applied and are actively being respected are included.
 */
@property (readonly, copy) NSUserInterfaceCompressionOptions *activeCompressionOptions;

@end

NS_ASSUME_NONNULL_END

