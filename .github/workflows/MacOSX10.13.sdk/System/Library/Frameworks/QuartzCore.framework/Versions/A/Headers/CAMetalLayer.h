/* CoreAnimation - CAMetalLayer.h

   Copyright (c) 2013-2017, Apple Inc.
   All rights reserved. */

#import <QuartzCore/CALayer.h>
#import <Metal/MTLPixelFormat.h>
#import <Metal/MTLDrawable.h>

@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLDrawable;

@class CAMetalLayer;

NS_ASSUME_NONNULL_BEGIN

/* CAMetalDrawable represents a displayable buffer that vends an object
 * that conforms to the MTLTexture protocol that may be used to create
 * a render target for Metal.
 *
 * Note: CAMetalLayer maintains an internal pool of textures used for
 * display. In order for a texture to be re-used for a new CAMetalDrawable,
 * any prior CAMetalDrawable must have been deallocated and another
 * CAMetalDrawable presented. */

@protocol CAMetalDrawable <MTLDrawable>

/* This is an object that conforms to the MTLTexture protocol and will
 * typically be used to create an MTLRenderTargetDescriptor. */

@property(readonly) id <MTLTexture> texture;

/* This is the CAMetalLayer responsible for displaying the drawable */

@property(readonly) CAMetalLayer *layer;

@end

/* Note: The default value of the `opaque' property for CAMetalLayer
 * instances is true. */

CA_CLASS_AVAILABLE (10.11, 8.0, 9.0, 2.0)
@interface CAMetalLayer : CALayer
{
@private
  struct _CAMetalLayerPrivate *_priv;
}

/* This property determines which MTLDevice the MTLTexture objects for
 * the drawables will be created from.
 * On iOS this defaults to the device returned by MTLCreateSystemDefaultDevice().
 * On MacOS this defaults to nil and must be set explicitly before asking for
 * the first drawable. */

@property(nullable, retain) id <MTLDevice> device;

/* This property controls the pixel format of the MTLTexture objects.
 * The two supported values are MTLPixelFormatBGRA8Unorm and
 * MTLPixelFormatBGRA8Unorm_sRGB. */

@property MTLPixelFormat pixelFormat;

/* This property controls whether or not the returned drawables'
 * MTLTextures may only be used for framebuffer attachments (YES) or
 * whether they may also be used for texture sampling and pixel
 * read/write operations (NO). A value of YES allows CAMetalLayer to
 * allocate the MTLTexture objects in ways that are optimized for display
 * purposes that makes them unsuitable for sampling. The recommended
 * value for most applications is YES. */

@property BOOL framebufferOnly;

/* This property controls the pixel dimensions of the returned drawable
 * objects. The most typical value will be the layer size multiplied by
 * the layer contentsScale property. */

@property CGSize drawableSize;

/* Returns a drawable. This will return nil if the layer has an invalid
 * combination of drawable properties. */

- (nullable id <CAMetalDrawable>)nextDrawable;

/* When false (the default value) changes to the layer's render buffer
 * appear on-screen asynchronously to normal layer updates. When true,
 * changes to the MTL content are sent to the screen via the standard
 * CATransaction mechanisms. */

@property BOOL presentsWithTransaction;

/* The colorspace of the rendered frames. If nil, no colormatching occurs.
 * If non-nil, the rendered content will be colormatched to the colorspace of
 * the context containing this layer (typically the display's colorspace). */

@property (nullable) CGColorSpaceRef colorspace;


/* If any rendering context on the screen has this enabled, all content will be
 * clamped to its NSScreen’s maximumExtendedDynamicRangeColorComponentValue
 * rather than 1.0. The default is NO.  */

@property BOOL wantsExtendedDynamicRangeContent;

/* This property controls if this layer and its drawables will be synchronized
 * to the display's Vsync. The default value is YES. */
@property BOOL displaySyncEnabled CA_AVAILABLE_STARTING (10.13);

/* This property controls if nextDrawable is allowed to timeout and return nil if
 * the system does not have a free drawable available for longer than one second.
 * The default value is YES. */
@property BOOL allowsNextDrawableTimeout CA_AVAILABLE_STARTING (10.13, 11.0, 11.0, 4.0);

@end

NS_ASSUME_NONNULL_END
