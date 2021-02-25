/* CoreAnimation - CAScrollLayer.h

   Copyright (c) 2006-2017, Apple Inc.
   All rights reserved. */

#import <QuartzCore/CALayer.h>

NS_ASSUME_NONNULL_BEGIN

CA_CLASS_AVAILABLE (10.5, 2.0, 9.0, 2.0)
@interface CAScrollLayer : CALayer

/* Changes the origin of the layer to point 'p'. */

- (void)scrollToPoint:(CGPoint)p;

/* Scroll the contents of the layer to ensure that rect 'r' is visible. */

- (void)scrollToRect:(CGRect)r;

/* Defines the axes in which the layer may be scrolled. Possible values
 * are `none', `vertically', `horizontally' or `both' (the default). */

@property(copy) NSString *scrollMode;

@end

@interface CALayer (CALayerScrolling)

/* These methods search for the closest ancestor CAScrollLayer of the *
 * receiver, and then call either -scrollToPoint: or -scrollToRect: on
 * that layer with the specified geometry converted from the coordinate
 * space of the receiver to that of the found scroll layer. */

- (void)scrollPoint:(CGPoint)p;

- (void)scrollRectToVisible:(CGRect)r;

/* Returns the visible region of the receiver, in its own coordinate
 * space. The visible region is the area not clipped by the containing
 * scroll layer. */

@property(readonly) CGRect visibleRect;

@end

/* `scrollMode' values. */

CA_EXTERN NSString * const kCAScrollNone
    CA_AVAILABLE_STARTING (10.5, 2.0, 9.0, 2.0);
CA_EXTERN NSString * const kCAScrollVertically
    CA_AVAILABLE_STARTING (10.5, 2.0, 9.0, 2.0);
CA_EXTERN NSString * const kCAScrollHorizontally
    CA_AVAILABLE_STARTING (10.5, 2.0, 9.0, 2.0);
CA_EXTERN NSString * const kCAScrollBoth
    CA_AVAILABLE_STARTING (10.5, 2.0, 9.0, 2.0);

NS_ASSUME_NONNULL_END
