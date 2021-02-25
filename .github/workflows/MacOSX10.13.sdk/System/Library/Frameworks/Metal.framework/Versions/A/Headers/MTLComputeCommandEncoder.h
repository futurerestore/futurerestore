//
//  MTLComputeCommandEncoder.h
//  Metal
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Metal/MTLDefines.h>
#import <Metal/MTLTypes.h>
#import <Metal/MTLCommandEncoder.h>
#import <Metal/MTLTexture.h>
#import <Metal/MTLCommandBuffer.h>
#import <Metal/MTLFence.h>

NS_ASSUME_NONNULL_BEGIN
@protocol MTLFunction;
@protocol MTLBuffer;
@protocol MTLSamplerState;
@protocol MTLTexture;
@protocol MTLComputePipelineState;

typedef struct {
    uint32_t threadgroupsPerGrid[3];
} MTLDispatchThreadgroupsIndirectArguments;

/*!
 @protocol MTLComputeCommandEncoder
 @abstract A command encoder that writes data parallel compute commands.
 */
NS_AVAILABLE(10_11, 8_0)
@protocol MTLComputeCommandEncoder <MTLCommandEncoder>

/*!
 @method setComputePipelineState:
 @abstract Set the compute pipeline state that will be used.
 */
- (void)setComputePipelineState:(id <MTLComputePipelineState>)state;

/*!
 @method setBytes:length:atIndex:
 @brief Set the data (by copy) for a given buffer binding point.  This will remove any existing MTLBuffer from the binding point.
 */
- (void)setBytes:(const void *)bytes length:(NSUInteger)length atIndex:(NSUInteger)index NS_AVAILABLE(10_11, 8_3);

/*!
 @method setBuffer:offset:atIndex:
 @brief Set a global buffer for all compute kernels at the given bind point index.
 */
- (void)setBuffer:(nullable id <MTLBuffer>)buffer offset:(NSUInteger)offset atIndex:(NSUInteger)index;

/*!
 @method setBufferOffset:atIndex:
 @brief Set the offset within the current global buffer for all compute kernels at the given bind point index.
 */
- (void)setBufferOffset:(NSUInteger)offset atIndex:(NSUInteger)index NS_AVAILABLE(10_11, 8_3);

/*!
 @method setBuffers:offsets:withRange:
 @brief Set an array of global buffers for all compute kernels with the given bind point range.
 */
- (void)setBuffers:(const id <MTLBuffer> __nullable [__nonnull])buffers offsets:(const NSUInteger [__nonnull])offsets withRange:(NSRange)range;

/*!
 @method setTexture:atIndex:
 @brief Set a global texture for all compute kernels at the given bind point index.
 */
- (void)setTexture:(nullable id <MTLTexture>)texture atIndex:(NSUInteger)index;

/*!
 @method setTextures:withRange:
 @brief Set an array of global textures for all compute kernels with the given bind point range.
 */
- (void)setTextures:(const id <MTLTexture> __nullable [__nonnull])textures withRange:(NSRange)range;

/*!
 @method setSamplerState:atIndex:
 @brief Set a global sampler for all compute kernels at the given bind point index.
 */
- (void)setSamplerState:(nullable id <MTLSamplerState>)sampler atIndex:(NSUInteger)index;

/*!
 @method setSamplers:withRange:
 @brief Set an array of global samplers for all compute kernels with the given bind point range.
 */
- (void)setSamplerStates:(const id <MTLSamplerState> __nullable [__nonnull])samplers withRange:(NSRange)range;

/*!
 @method setSamplerState:lodMinClamp:lodMaxClamp:atIndex:
 @brief Set a global sampler for all compute kernels at the given bind point index.
 */
- (void)setSamplerState:(nullable id <MTLSamplerState>)sampler lodMinClamp:(float)lodMinClamp lodMaxClamp:(float)lodMaxClamp atIndex:(NSUInteger)index;

/*!
 @method setSamplers:lodMinClamps:lodMaxClamps:withRange:
 @brief Set an array of global samplers for all compute kernels with the given bind point range.
 */
- (void)setSamplerStates:(const id <MTLSamplerState> __nullable [__nonnull])samplers lodMinClamps:(const float [__nonnull])lodMinClamps lodMaxClamps:(const float [__nonnull])lodMaxClamps withRange:(NSRange)range;


/*!
 @method setThreadgroupMemoryLength:atIndex:
 @brief Set the threadgroup memory byte length at the binding point specified by the index. This applies to all compute kernels.
 */
- (void)setThreadgroupMemoryLength:(NSUInteger)length atIndex:(NSUInteger)index;




/*
 @method setStageInRegion:region:
 @brief Set the region of the stage_in attributes to apply the compute kernel.
*/
- (void)setStageInRegion:(MTLRegion)region NS_AVAILABLE(10_12, 10_0);

/*
 @method dispatchThreadgroups:threadsPerThreadgroup:
 @abstract Enqueue a compute function dispatch.
 */
- (void)dispatchThreadgroups:(MTLSize)threadgroupsPerGrid threadsPerThreadgroup:(MTLSize)threadsPerThreadgroup;

/*
 @method dispatchThreadgroupsWithIndirectBuffer:indirectBufferOffset:threadsPerThreadgroup:
 @abstract Enqueue a compute function dispatch using an indirect buffer for threadgroupsPerGrid see MTLDispatchThreadgroupsIndirectArguments.
 @param indirectBuffer A buffer object that the device will read dispatchThreadgroups arguments from, see MTLDispatchThreadgroupsIndirectArguments.
 @param indirectBufferOffset Byte offset within @a indirectBuffer to read arguments from.  @a indirectBufferOffset must be a multiple of 4.
 */
- (void)dispatchThreadgroupsWithIndirectBuffer:(id <MTLBuffer>)indirectBuffer indirectBufferOffset:(NSUInteger)indirectBufferOffset threadsPerThreadgroup:(MTLSize)threadsPerThreadgroup NS_AVAILABLE(10_11, 9_0);

/*
 @method dispatchThreads:threadsPerThreadgroup:
 @abstract Enqueue a compute function dispatch.
 */
- (void)dispatchThreads:(MTLSize)threadsPerGrid threadsPerThreadgroup:(MTLSize)threadsPerThreadgroup NS_AVAILABLE(10_13, 11_0);

/*!
 @method updateFence:
 @abstract Update the event to capture all GPU work so far enqueued by this encoder.
 @discussion The event is updated at kernel submission to maintain global order and prevent deadlock.
 Drivers may delay fence updates until the end of the encoder. Drivers may also wait on fences at the beginning of an encoder. It is therefore illegal to wait on a fence after it has been updated in the same encoder.
 */
- (void)updateFence:(id <MTLFence>)fence NS_AVAILABLE(10_13, 10_0);

/*!
 @method waitForFence:
 @abstract Prevent further GPU work until the event is reached.
 @discussion The event is evaluated at kernel submision to maintain global order and prevent deadlock.
 Drivers may delay fence updates until the end of the encoder. Drivers may also wait on fences at the beginning of an encoder. It is therefore illegal to wait on a fence after it has been updated in the same encoder.
 */
- (void)waitForFence:(id <MTLFence>)fence NS_AVAILABLE(10_13, 10_0);


/*!
 * @method useResource:usage:
 * @abstract Declare that a resource may be accessed by the command encoder through an argument buffer
 * @discussion This method does not protect against data hazards; these hazards must be addressed using an MTLFence. This method must be called before encoding any dispatch commands which may access the resource through an argument buffer.
 */
- (void)useResource:(id <MTLResource>)resource usage:(MTLResourceUsage)usage NS_AVAILABLE(10_13, 11_0);

/*!
 * @method useResources:count:usage:
 * @abstract Declare that an array of resources may be accessed through an argument buffer by the command encoder
 * @discussion This method does not protect against data hazards; these hazards must be addressed using an MTLFence. This method must be called before encoding any dispatch commands which may access the resources through an argument buffer.
 */
- (void)useResources:(const id <MTLResource> __nonnull[__nonnull])resources count:(NSUInteger)count usage:(MTLResourceUsage)usage NS_AVAILABLE(10_13, 11_0);

/*!
 * @method useHeap:
 * @abstract Declare that the resources allocated from a heap may be accessed by the render pass through an argument buffer
 * @discussion This method does not protect against data hazards; these hazards must be addressed using an MTLFence. This method must be called before encoding any dispatch commands which may access the resources allocated from the heap through an argument buffer. This method may cause all of the color attachments allocated from the heap to become decompressed. Therefore, it is recommended that the useResource:usage: or useResources:count:usage: methods be used for color attachments instead, with a minimal (i.e. read-only) usage.
 */
- (void)useHeap:(id <MTLHeap>)heap NS_AVAILABLE(10_13, 11_0);

/*!
 * @method useHeaps:count:
 * @abstract Declare that the resources allocated from an array of heaps may be accessed by the render pass through an argument buffer
 * @discussion This method does not protect against data hazards; these hazards must be addressed using an MTLFence. This method must be called before encoding any dispatch commands which may access the resources allocated from the heaps through an argument buffer. This method may cause all of the color attachments allocated from the heaps to become decompressed. Therefore, it is recommended that the useResource:usage: or useResources:count:usage: methods be used for color attachments instead, with a minimal (i.e. read-only) usage.
 */
- (void)useHeaps:(const id <MTLHeap> __nonnull[__nonnull])heaps count:(NSUInteger)count NS_AVAILABLE(10_13, 11_0);

@end
NS_ASSUME_NONNULL_END
