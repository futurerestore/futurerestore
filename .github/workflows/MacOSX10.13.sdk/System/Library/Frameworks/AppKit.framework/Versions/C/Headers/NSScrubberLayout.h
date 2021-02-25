/*
 NSScrubberLayout.h
 Application Kit
 Copyright (c) 2016-2017, Apple Inc.
 All rights reserved.
 */

#import <Foundation/NSGeometry.h>

NS_ASSUME_NONNULL_BEGIN

@class NSScrubber, NSScrubberFlowLayout, NSIndexSet;
@protocol NSScrubberDelegate;

#pragma mark - Layout Attributes

/*!
 @class NSScrubberLayoutAttributes
 @abstract @c NSScrubberLayoutAttributes describes the layout of a single @c NSScrubber item.
 @discussion @c NSScrubberLayout objects transact in terms of @c NSScrubberLayoutAttributes. @c NSScrubberLayoutAttributes can be subclassed if a layout object wants to include more layout information than the base implementation provides. Subclasses of @c NSScrubberLayoutAttributes must implement @c isEqual:, @c hash, and the @c NSCopying protocol.
 */
NS_CLASS_AVAILABLE_MAC(10_12_2)
@interface NSScrubberLayoutAttributes : NSObject <NSCopying> {
@private
    NSInteger _itemIndex;
    NSRect _frame;
    CGFloat _alpha;
    unsigned int _escapes:1;
    unsigned int _reservedLayoutAttributeFlags:31 __unused;
#ifndef __OBJC2__
    id _reserved[2] __unused;
#endif
}

@property NSInteger itemIndex;
@property NSRect frame;
@property CGFloat alpha;

+ (instancetype)layoutAttributesForItemAtIndex:(NSInteger)index;

@end

#pragma mark - Base Layout Object

/*!
 @class NSScrubberLayout
 @abstract @c NSScrubberLayout is an abstract class that describes the layout of items within a @c NSScrubber control.
 */
NS_CLASS_AVAILABLE_MAC(10_12_2)
@interface NSScrubberLayout : NSObject <NSCoding> {
@private
    __weak id _private;
    unsigned int _dirty:1;
    unsigned int _reservedFlags:31 __unused;
}

#pragma mark Base Implementation
/* 
 The following properties and methods are provided by the base implementation
 */

/// Specifies a class for describing layout attributes. By default, this is @c NSScrubberLayoutAttributes, but subclasses may override this method to use a custom subclass of @c NSScrubberLayoutAttributes.
@property (class, readonly) Class layoutAttributesClass;

/// The NSScrubber control that this layout is assigned to, or @c nil if the receiver is not assigned to a scrubber.
@property (weak, nullable, readonly) NSScrubber *scrubber;

/// The currently visible rectangle, in the coordinate space of the scrubber content. Returns @c NSZeroRect if the receiver is not assigned to a scrubber.
@property (readonly) NSRect visibleRect;

- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithCoder:(NSCoder *)coder NS_DESIGNATED_INITIALIZER;

/// Signals that layout has been invalidated and the NSScrubber should run a fresh layout pass. Subclasses may define more granular invalidation methods suitable for their own data structures, but those methods should always call up to -invalidateLayout.
- (void)invalidateLayout NS_REQUIRES_SUPER;

#pragma mark Subclassing Hooks
/* 
 The following methods and properties should be implemented by subclasses.
 */

/// Following any invalidation in layout, @c NSScrubber will call @c prepareLayout on its layout object prior to requesting any other layout information. Subclasses should use this method to perform upfront calculations and caching. The base implementation of this method does nothing.
- (void)prepareLayout;

/// Returns the content size for all elements within the scrubber. The base implementation returns @c NSZeroSize.
@property (readonly) NSSize scrubberContentSize;

/// Returns the layout attributes for a single item within the scrubber. The base implementation returns @c nil.
- (nullable __kindof NSScrubberLayoutAttributes *)layoutAttributesForItemAtIndex:(NSInteger)index;

/// Returns the set of layout attributes for all items within the provided rectangle. The base implementation returns an empty set.
- (NSSet<__kindof NSScrubberLayoutAttributes *> *)layoutAttributesForItemsInRect:(NSRect)rect;

/// If @c YES, the scrubber will invalidate its layout when the selection changes. The default value is @c NO. Subclasses should return @c YES if the selection index affects the item layout.
@property (readonly) BOOL shouldInvalidateLayoutForSelectionChange;

/// If @c YES, the scrubber will invalidate its layout when an item is highlighted. The default value is @c NO. Subclasses should return @c YES if the highlight state affects the item layout.
@property (readonly) BOOL shouldInvalidateLayoutForHighlightChange;

/// If @c YES, the scrubber will invalidate its layout in response to a change in the visible region. The default value is @c NO. Subclasses which rely on the size or origin of the visible region should return @c YES.
- (BOOL)shouldInvalidateLayoutForChangeFromVisibleRect:(NSRect)fromVisibleRect toVisibleRect:(NSRect)toVisibleRect;

/// If @c YES, the layout object will automatically have its inputs and outputs mirrored in right-to-left interfaces. The default value is @c YES. Subclasses that wish to handle RTL layout manually should return @c NO.
@property (readonly) BOOL automaticallyMirrorsInRightToLeftLayout;

@end

#pragma mark - Flow Layout

@protocol NSScrubberFlowLayoutDelegate <NSScrubberDelegate>
@optional
- (NSSize)scrubber:(NSScrubber *)scrubber layout:(NSScrubberFlowLayout *)layout sizeForItemAtIndex:(NSInteger)itemIndex;
@end

/*!
 @class NSScrubberFlowLayout
 @abstract @c NSScrubberFlowLayout is a concrete layout object that arranges items end-to-end in a linear strip. It supports a fixed inter-item spacing and both fixed- and variable-sized items.
 @discussion If the associated scrubber's @c delegate conforms to @c NSScrubberFlowLayoutDelegate, and it implements the @c scrubber:layout:sizeForItemAtIndex: method, @c NSScrubberFlowLayout will obtain the item size from the delegate. If the delegate does not implement that method, or if the method returns @c NSZeroSize, it will fall back to using the layout's @c itemSize property. By default, NSScrubberFlowLayout does not invalidate its layout on selection change, highlight change, or visible rectangle change.
 */
NS_CLASS_AVAILABLE_MAC(10_12_2)
@interface NSScrubberFlowLayout : NSScrubberLayout {
@private
    id _support;
    CGFloat _itemSpacing;
    NSSize _itemSize;
}

/// The amount of horizontal spacing between items in points. The default value is 0.0.
@property CGFloat itemSpacing;

/// The frame size for each item, if not provided by the scrubber's delegate. The default value is { 50.0, 30.0 }.
@property NSSize itemSize;

- (void)invalidateLayoutForItemsAtIndexes:(NSIndexSet *)invalidItemIndexes;

@end

#pragma mark - Proportional Layout

/*!
 @class NSScrubberProportionalLayout
 @abstract @c NSScrubberProportionalLayout is a concrete layout object that sizes each item to some fraction of the scrubber's visible size.
 */
NS_CLASS_AVAILABLE_MAC(10_12_2)
@interface NSScrubberProportionalLayout : NSScrubberLayout {
@private
    NSInteger _numberOfVisibleItems;
}

/// The number of items that should fit within the scrubber's viewport at once.
@property NSInteger numberOfVisibleItems;

- (instancetype)initWithNumberOfVisibleItems:(NSInteger)numberOfVisibleItems NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
