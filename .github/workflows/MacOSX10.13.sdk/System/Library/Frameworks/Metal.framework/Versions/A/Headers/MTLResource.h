//
//  MTLResource.h
//  Metal
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Metal/MTLDefines.h>

NS_ASSUME_NONNULL_BEGIN
/*!
 @enum MTLPurgeableOption
 @abstract Options for setPurgeable call.
 
 @constant MTLPurgeableStateNonVolatile
 The contents of this resource may not be discarded.

 @constant MTLPurgeableStateVolatile
 The contents of this resource may be discarded.

 @constant MTLPurgeableStateEmpty
 The contents of this are discarded.
 
 @constant MTLPurgeableStateKeepCurrent
 The purgeabelity state is not changed.
 */
typedef NS_ENUM(NSUInteger, MTLPurgeableState)
{
    MTLPurgeableStateKeepCurrent = 1,
    
    MTLPurgeableStateNonVolatile = 2,
    MTLPurgeableStateVolatile = 3,
    MTLPurgeableStateEmpty = 4,
} NS_ENUM_AVAILABLE(10_11, 8_0);

/*!
 @enum MTLCPUCacheMode
 @abstract Describes what CPU cache mode is used for the CPU's mapping of a texture resource.
 @constant MTLCPUCacheModeDefaultCache
 The default cache mode for the system.
 
 @constant MTLCPUCacheModeWriteCombined
 Write combined memory is optimized for resources that the CPU will write into, but never read.  On some implementations, writes may bypass caches avoiding cache pollution, and reads perform very poorly.
 
 @discussion
 Applications should only investigate changing the cache mode if writing to normally cached buffers is known to cause performance issues due to cache pollution, as write combined memory can have surprising performance pitfalls.  Another approach is to use non-temporal stores to normally cached memory (STNP on ARMv8, _mm_stream_* on x86_64).
 */
typedef NS_ENUM(NSUInteger, MTLCPUCacheMode)
{
    MTLCPUCacheModeDefaultCache = 0,
    MTLCPUCacheModeWriteCombined = 1,
} NS_ENUM_AVAILABLE(10_11, 8_0);

/*!
 @enum MTLStorageMode
 @abstract Describes location and CPU mapping of MTLTexture.
 @constant MTLStorageModeShared
 In this mode, CPU and device will nominally both use the same underlying memory when accessing the contents of the texture resource.
 However, coherency is only guaranteed at command buffer boundaries to minimize the required flushing of CPU and GPU caches.
 This is the default storage mode for iOS Textures.

 @constant MTLStorageModeManaged
 This mode relaxes the coherency requirements and requires that the developer make explicit requests to maintain
 coherency between a CPU and GPU version of the texture resource.  In order for CPU to access up to date GPU results,
 first, a blit synchronizations must be completed (see synchronize methods of MTLBlitCommandEncoder).
 Blit overhead is only incurred if GPU has modified the resource.
 This is the default storage mode for OS X Textures.

 @constant MTLStorageModePrivate
 This mode allows the texture resource data to be kept entirely to GPU (or driver) private memory that will never be accessed by the CPU directly, so no
 conherency of any kind must be maintained.
 
 @constant MTLStorageModeMemoryless
 This mode allows creation of resources that do not have a GPU or CPU memory backing, but do have on-chip storage for TBDR
 devices. The contents of the on-chip storage is undefined and does not persist, but its configuration is controlled by the
 MTLTexture descriptor. Textures created with MTLStorageModeMemoryless dont have an IOAccelResource at any point in their
 lifetime. The only way to populate such resource is to perform rendering operations on it. Blit operations are disallowed.
*/
typedef NS_ENUM(NSUInteger, MTLStorageMode)
{
    MTLStorageModeShared  = 0,
    MTLStorageModeManaged NS_ENUM_AVAILABLE(10_11, NA) = 1,
    MTLStorageModePrivate = 2,
    MTLStorageModeMemoryless NS_ENUM_AVAILABLE(NA, 10_0) = 3,
} NS_ENUM_AVAILABLE(10_11, 9_0);

/*!
 @enum MTLResourceOptions
 @abstract A set of optional arguments to influence the creation of a MTLResource (MTLTexture or MTLBuffer)

 @constant MTLResourceCPUCacheModeDefault
 The default CPU cache mode for the resource.
 Applications should only investigate changing the cache mode if writing to normally cached buffers is known to
 cause performance issues due to cache pollution, as write combined memory can have surprising performance pitfalls.
 Another approach is to use non-temporal stores to normally cached memory (STNP on ARMv8, _mm_stream_* on x86_64).
 
 @constant MTLResourceCPUCacheModeWriteCombined
 Write combined memory is optimized for resources that the CPU will write into, but never read.
 On some implementations, writes may bypass caches avoiding cache pollution, and reads perform very poorly.
 
 @constant MTLResourceStorageModeShared
 In this mode, CPU and device will nominally both use the same underlying memory when accessing the contents of the resource.
 However, coherency is only guaranteed at command buffer boundaries to minimize the required flushing of CPU and GPU caches.
 This is the default storage mode for iOS Textures.

 @constant MTLResourceStorageModeManaged
 This mode relaxes the coherency requirements and requires that the developer make explicit requests to maintain
 coherency between a CPU and GPU version of the resource.  Changes due to CPU stores outside of the Metal API must be
 indicated to Metal via MTLBuffer::didModifyRange:(NSRange)range.  In order for CPU to access up to date GPU results,
 first, a blit synchronizations must be completed (see synchronize methods of MTLBlitCommandEncoder).
 Blit overhead is only incurred if GPU has modified the resource.
 This storage mode is only defined for OS X.
 This is the default storage mode for OS X Textures.

 @constant MTLResourceStorageModePrivate
 This mode allows the data to be kept entirely to GPU (or driver) private memory that will never be accessed by the CPU directly, so no
 conherency of any kind must be maintained.

 @constant MTLResourceStorageModeMemoryless
 This mode allows creation of resources that do not have a GPU or CPU memory backing, but do have on-chip storage for TBDR
 devices. The contents of the on-chip storage is undefined and does not persist, but its configuration is controlled by the
 MTLTexture descriptor. Textures created with MTLStorageModeMemoryless dont have an IOAccelResource at any point in their
 lifetime. The only way to populate such resource is to perform rendering operations on it. Blit operations are disallowed.

 @discussion
 Note that resource options are a property of MTLTextureDescriptor (resourceOptions), so apply to texture creation.
 they are also passed directly into MTLBuffer creation methods.
*/

#define MTLResourceCPUCacheModeShift 0
#define MTLResourceCPUCacheModeMask  (0xfUL << MTLResourceCPUCacheModeShift)
#define MTLResourceStorageModeShift  4
#define MTLResourceStorageModeMask   (0xfUL << MTLResourceStorageModeShift)

#define MTLResourceHazardTrackingModeShift  8
#define MTLResourceHazardTrackingModeMask   (0x1UL << MTLResourceHazardTrackingModeShift)

typedef NS_OPTIONS(NSUInteger, MTLResourceOptions)
{
    MTLResourceCPUCacheModeDefaultCache  = MTLCPUCacheModeDefaultCache  << MTLResourceCPUCacheModeShift,
    MTLResourceCPUCacheModeWriteCombined = MTLCPUCacheModeWriteCombined << MTLResourceCPUCacheModeShift,

    MTLResourceStorageModeShared     NS_ENUM_AVAILABLE(10_11, 9_0)  = MTLStorageModeShared     << MTLResourceStorageModeShift,
    MTLResourceStorageModeManaged    NS_ENUM_AVAILABLE(10_11, NA)   = MTLStorageModeManaged    << MTLResourceStorageModeShift,
    MTLResourceStorageModePrivate    NS_ENUM_AVAILABLE(10_11, 9_0)  = MTLStorageModePrivate    << MTLResourceStorageModeShift,
    MTLResourceStorageModeMemoryless NS_ENUM_AVAILABLE(NA,    10_0) = MTLStorageModeMemoryless << MTLResourceStorageModeShift,

    MTLResourceHazardTrackingModeUntracked NS_ENUM_AVAILABLE(10_13, 10_0) = 0x1UL << MTLResourceHazardTrackingModeShift,

    // Deprecated spellings
    MTLResourceOptionCPUCacheModeDefault       = MTLResourceCPUCacheModeDefaultCache,
    MTLResourceOptionCPUCacheModeWriteCombined = MTLResourceCPUCacheModeWriteCombined,
} NS_ENUM_AVAILABLE(10_11, 8_0);

@protocol MTLDevice;

@protocol MTLHeap;

/*!
 @protocol MTLResource
 @abstract Common APIs available for MTLBuffer and MTLTexture instances
 */
NS_AVAILABLE(10_11, 8_0)
@protocol MTLResource <NSObject>

/*!
 @property label
 @abstract A string to help identify this object.
 */
@property (nullable, copy, atomic) NSString *label;

/*!
 @property device
 @abstract The device this resource was created against.  This resource can only be used with this device.
 */
@property (readonly) id <MTLDevice> device;

/*!
 @property cpuCacheMode
 @abstract The cache mode used for the CPU mapping for this resource
 */
@property (readonly) MTLCPUCacheMode cpuCacheMode;

/*!
 @property storageMode
 @abstract The resource storage mode used for the CPU mapping for this resource
 */
@property (readonly) MTLStorageMode storageMode NS_AVAILABLE(10_11, 9_0);

/*!
 @method setPurgeableState
 @abstract Set (or query) the purgeability state of a resource
 @discussion Synchronously set the purgeability state of a resource and return what the prior (or current) state is.
 FIXME: If the device is keeping a cached copy of the resource, both the shared copy and cached copy are made purgeable.  Any access to the resource by either the CPU or device will be undefined.
 */
- (MTLPurgeableState)setPurgeableState:(MTLPurgeableState)state;

/*!
 @property heap
 @abstract The heap from which this resouce was created.
 @discussion Nil when this resource is not backed by a heap.
 */
@property (readonly, nullable) id <MTLHeap> heap NS_AVAILABLE(10_13, 10_0);

/*!
 @property allocatedSize
 @abstrace The size in bytes occupied by this resource
*/
@property (readonly) NSUInteger allocatedSize NS_AVAILABLE(10_13, 11_0);

/*!
 @method makeAliasable
 @abstract Allow future heap sub-allocations to alias against this resource's memory.
 @discussion It is illegal to call this method on a non heap-based resource. 
 It is also illegal to call this method on texture views created from heap-based textures.
 The debug layer will raise an exception. Calling this method on textures sub-allocated
 from Buffers backed by heap memory has no effect.
 Once a resource is made aliasable, the decision cannot be reverted.
 */
-(void) makeAliasable NS_AVAILABLE(10_13, 10_0);

/*!
 @method isAliasable
 @abstract Returns whether future heap sub-allocations may alias against this resource's memory.
 @return YES if <st>makeAliasable</st> was previously successfully called on this resource. NO otherwise.
 If resource is sub-allocated from other resource created on the heap, isAliasable returns 
 aliasing state of that base resource. Also returns NO when storage mode is memoryless.
 */
-(BOOL) isAliasable NS_AVAILABLE(10_13, 10_0);

@end


NS_ASSUME_NONNULL_END
