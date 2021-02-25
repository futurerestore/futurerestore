/*	NSAffineTransform.h
        Copyright (c) 1997-2017, Apple Inc. All rights reserved.
*/

#import <Foundation/NSObject.h>
#import <Foundation/NSGeometry.h>
#import <CoreGraphics/CGAffineTransform.h>

NS_ASSUME_NONNULL_BEGIN

typedef struct {
    CGFloat m11, m12, m21, m22;
    CGFloat tX, tY;
} NSAffineTransformStruct;

@interface NSAffineTransform : NSObject <NSCopying, NSSecureCoding> {
    @private
    NSAffineTransformStruct _transformStruct;
}

// Creation
+ (NSAffineTransform *)transform;

// Initialization
- (instancetype)initWithTransform:(NSAffineTransform *)transform;
- (instancetype)init NS_DESIGNATED_INITIALIZER;

// Translating
- (void)translateXBy:(CGFloat)deltaX yBy:(CGFloat)deltaY;

// Rotating
- (void)rotateByDegrees:(CGFloat)angle;
- (void)rotateByRadians:(CGFloat)angle;

// Scaling
- (void)scaleBy:(CGFloat)scale;
- (void)scaleXBy:(CGFloat)scaleX yBy:(CGFloat)scaleY;

// Inverting
- (void)invert;

// Transforming with transform
- (void)appendTransform:(NSAffineTransform *)transform;
- (void)prependTransform:(NSAffineTransform *)transform;

// Transforming points and sizes
- (NSPoint)transformPoint:(NSPoint)aPoint;
- (NSSize)transformSize:(NSSize)aSize;

// Transform Struct
@property NSAffineTransformStruct transformStruct;

@end

NS_ASSUME_NONNULL_END
