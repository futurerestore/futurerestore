/*
 NSCustomTouchBarItem.h
 Application Kit
 Copyright (c) 2015-2017, Apple Inc.
 All rights reserved.
*/

#import <AppKit/NSTouchBarItem.h>

NS_ASSUME_NONNULL_BEGIN

NS_CLASS_AVAILABLE_MAC(10_12_2)
@interface NSCustomTouchBarItem : NSTouchBarItem {
@private
    NSView *_view;
    NSViewController *_viewController;
    NSString *_customizationLabel;
    NSInteger _preferredPopoverTransposerPriority;
    Class _preferredPopoverTransposerClass;
    CGFloat _preferredZOrder;
#if !__OBJC2__
    void *_customTouchBarItemReserved[3] __unused;
#endif /* !__OBJC2__ */
}

/*
    A view to be displayed in the NSTouchBar in the location corresponding to this item.
    By default, the getter for this property will return this item's view controller's view. If this property is set explicitly, the view controller will be set to nil.
    This property is archived.
*/
@property (readwrite, strong) __kindof NSView *view;

/*
    A view controller whose view is to be displayed in the NSTouchBar in the location corresponding to this item.
    By default, this property is nil. When set, this item's view property will automatically return the view associated with this view controller.
    This property is archived.
*/
@property (readwrite, strong, nullable) __kindof NSViewController *viewController;

/*
    The localized string labelling this item during user customization. The default value is the empty string. This property is archived.
*/
@property (readwrite, copy, null_resettable) NSString *customizationLabel;

@end

NS_ASSUME_NONNULL_END
