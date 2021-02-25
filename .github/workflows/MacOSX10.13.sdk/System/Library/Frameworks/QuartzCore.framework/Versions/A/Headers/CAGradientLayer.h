/* CoreAnimation - CAGradientLayer.h

   Copyright (c) 2008-2017, Apple Inc.
   All rights reserved. */

/* The gradient layer draws a color gradient over its background color,
 * filling the shape of the layer (i.e. including rounded corners). */

#import <QuartzCore/CALayer.h>
#import <Foundation/NSArray.h>

NS_ASSUME_NONNULL_BEGIN

CA_CLASS_AVAILABLE (10.6, 3.0, 9.0, 2.0)
@interface CAGradientLayer : CALayer

/* The array of CGColorRef objects defining the color of each gradient
 * stop. Defaults to nil. Animatable. */

@property(nullable, copy) NSArray *colors;

/* An optional array of NSNumber objects defining the location of each
 * gradient stop as a value in the range [0,1]. The values must be
 * monotonically increasing. If a nil array is given, the stops are
 * assumed to spread uniformly across the [0,1] range. When rendered,
 * the colors are mapped to the output colorspace before being
 * interpolated. Defaults to nil. Animatable. */

@property(nullable, copy) NSArray<NSNumber *> *locations;

/* The start and end points of the gradient when drawn into the layer's
 * coordinate space. The start point corresponds to the first gradient
 * stop, the end point to the last gradient stop. Both points are
 * defined in a unit coordinate space that is then mapped to the
 * layer's bounds rectangle when drawn. (I.e. [0,0] is the bottom-left
 * corner of the layer, [1,1] is the top-right corner.) The default values
 * are [.5,0] and [.5,1] respectively. Both are animatable. */

@property CGPoint startPoint;
@property CGPoint endPoint;

/* The kind of gradient that will be drawn. Currently the only allowed
 * value is `axial' (the default value). */

@property(copy) NSString *type;

@end

/** `type' values. **/

CA_EXTERN NSString * const kCAGradientLayerAxial
    CA_AVAILABLE_STARTING (10.6, 3.0, 9.0, 2.0);

NS_ASSUME_NONNULL_END
