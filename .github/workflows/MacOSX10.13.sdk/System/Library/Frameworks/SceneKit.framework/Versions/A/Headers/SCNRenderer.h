//
//  SCNRenderer.h
//
//  Copyright (c) 2012-2017 Apple Inc. All rights reserved.
//

#import <SceneKit/SCNSceneRenderer.h>
#import <SceneKit/SCNTechnique.h>

NS_ASSUME_NONNULL_BEGIN

@class NSImage;
@protocol MTLDevice;
@protocol MTLCommandQueue;
@protocol MTLRenderCommandEncoder;
@class MTLRenderPassDescriptor;

/*! @class SCNRenderer
	@abstract SCNRenderer lets you use the SceneKit renderer in an OpenGL context or Metal render pass descriptor of your own.
 */
__WATCHOS_PROHIBITED
@interface SCNRenderer : NSObject <SCNSceneRenderer, SCNTechniqueSupport>

/*! 
 @method rendererWithContext:options:
 @abstract Creates a new renderer object.
 @param context The context to render into.
 @param options An optional dictionary for future extensions.
 */
+ (instancetype)rendererWithContext:(nullable CGLContextObj)context options:(nullable NSDictionary *)options;

/*!
 @method rendererWithDevice:options:
 @abstract Creates a new renderer object that renders using Metal.
 @param device The metal device to use. Pass nil to let SceneKit choose a default device.
 @param options An optional dictionary for future extensions.
 */
+ (instancetype)rendererWithDevice:(nullable id <MTLDevice>)device options:(nullable NSDictionary *)options API_AVAILABLE(macos(10.11), ios(9.0));

/*! 
 @property scene
 @abstract Specifies the scene of the receiver
 */
@property(nonatomic, retain, nullable) SCNScene *scene;

/*!
 @method renderAtTime:viewport:commandBuffer:passDescriptor:
 @abstract updates and renders the receiver's scene at the specified time (system time) viewport, Metal command buffer and pass descriptor.
 @discussion Use this method to render using Metal.
 */
- (void)renderAtTime:(CFTimeInterval)time viewport:(CGRect)viewport commandBuffer:(id <MTLCommandBuffer>)commandBuffer passDescriptor:(MTLRenderPassDescriptor *)renderPassDescriptor API_AVAILABLE(macos(10.11), ios(9.0));

/*!
 @method renderAtTime:
 @abstract updates and renders the receiver's scene at the specified time (system time).
 @discussion This method only work if the receiver was allocated with an OpenGL context. Use renderAtTime:withEncoder:pass:commandQueue: to render with Metal.
 */
- (void)renderAtTime:(CFTimeInterval)time API_AVAILABLE(macos(10.10));

/*!
 @method updateAtTime:
 @abstract updates the receiver's scene at the specified time (system time).
 */
- (void)updateAtTime:(CFTimeInterval)time API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0), watchos(4.0));

/*!
 @method renderWithViewport:viewport:commandBuffer:passDescriptor:
 @abstract renders the receiver's scene with the specified viewport, Metal command buffer and pass descriptor.
 @discussion Use this method to render using Metal. This method doesn't update the scene's animations, physics, particles etc... It's up to you to call "updateAtTime:" to update the scene.
 */
- (void)renderWithViewport:(CGRect)viewport commandBuffer:(id <MTLCommandBuffer>)commandBuffer passDescriptor:(MTLRenderPassDescriptor *)renderPassDescriptor API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0), watchos(4.0));

/*!
 @property nextFrameTime
 @abstract Returns the time at which the next update should happen. If infinite no update needs to be scheduled yet. If the current frame time, a continuous animation is running and an update should be scheduled after a "natural" delay.
 */
@property(nonatomic, readonly) CFTimeInterval nextFrameTime API_AVAILABLE(macos(10.10));

/*!
 @method snapshotAtTime:withSize:antialiasingMode:
 @abstract renders the receiver's scene at the specified time (system time) into an image.
 */
- (NSImage *)snapshotAtTime:(CFTimeInterval)time withSize:(CGSize)size antialiasingMode:(SCNAntialiasingMode)antialiasingMode API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0));

/*!
 @method updateProbes:atTime:
 @abstract Update the specified probes by computing their incoming irradiance in the receiver's scene at the specified time.
 @param lightProbes An array of nodes that must have a light probe attached.
 @param time The time used to render the scene when computing the light probes irradiance.
 @discussion Light probes are only supported with Metal. This method is observable using NSProgress.
 */
- (void)updateProbes:(NSArray<SCNNode*> *)lightProbes atTime:(CFTimeInterval)time API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0));


// Deprecated
/*!
 @method render
 @abstract renders the receiver's scene at the current system time.
 @discussion This method only work if the receiver was allocated with an OpenGL context and it is deprecated (use renderAtTime: instead). Use renderAtTime:withEncoder:pass:commandQueue: to render with Metal.
 */
- (void)render API_DEPRECATED_WITH_REPLACEMENT("-renderAtTime:withEncoder:pass:commandQueue:", macos(10.8, 10.11), ios(8.0, 9.0)) API_UNAVAILABLE(watchos, tvos);

@end

NS_ASSUME_NONNULL_END
