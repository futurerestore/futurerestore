//
//  MTLRenderPipeline.h
//  Metal
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <Metal/MTLDefines.h>
#import <Metal/MTLDevice.h>
#import <Metal/MTLRenderCommandEncoder.h>
#import <Metal/MTLRenderPass.h>
#import <Metal/MTLPixelFormat.h>
#import <Metal/MTLArgument.h>
#import <Metal/MTLFunctionConstantValues.h>
#import <Metal/MTLPipeline.h>


NS_ASSUME_NONNULL_BEGIN
@class MTLVertexDescriptor;

typedef NS_ENUM(NSUInteger, MTLBlendFactor) {
    MTLBlendFactorZero = 0,
    MTLBlendFactorOne = 1,
    MTLBlendFactorSourceColor = 2,
    MTLBlendFactorOneMinusSourceColor = 3,
    MTLBlendFactorSourceAlpha = 4,
    MTLBlendFactorOneMinusSourceAlpha = 5,
    MTLBlendFactorDestinationColor = 6,
    MTLBlendFactorOneMinusDestinationColor = 7,
    MTLBlendFactorDestinationAlpha = 8,
    MTLBlendFactorOneMinusDestinationAlpha = 9,
    MTLBlendFactorSourceAlphaSaturated = 10,
    MTLBlendFactorBlendColor = 11,
    MTLBlendFactorOneMinusBlendColor = 12,
    MTLBlendFactorBlendAlpha = 13,
    MTLBlendFactorOneMinusBlendAlpha = 14,
    MTLBlendFactorSource1Color              NS_AVAILABLE(10_12, NA) = 15,
    MTLBlendFactorOneMinusSource1Color      NS_AVAILABLE(10_12, NA) = 16,
    MTLBlendFactorSource1Alpha              NS_AVAILABLE(10_12, NA) = 17,
    MTLBlendFactorOneMinusSource1Alpha      NS_AVAILABLE(10_12, NA) = 18,
} NS_ENUM_AVAILABLE(10_11, 8_0);

typedef NS_ENUM(NSUInteger, MTLBlendOperation) {
    MTLBlendOperationAdd = 0,
    MTLBlendOperationSubtract = 1,
    MTLBlendOperationReverseSubtract = 2,
    MTLBlendOperationMin = 3,
    MTLBlendOperationMax = 4,
} NS_ENUM_AVAILABLE(10_11, 8_0);

typedef NS_OPTIONS(NSUInteger, MTLColorWriteMask) {
    MTLColorWriteMaskNone  = 0,
    MTLColorWriteMaskRed   = 0x1 << 3,
    MTLColorWriteMaskGreen = 0x1 << 2,
    MTLColorWriteMaskBlue  = 0x1 << 1,
    MTLColorWriteMaskAlpha = 0x1 << 0,
    MTLColorWriteMaskAll   = 0xf
} NS_ENUM_AVAILABLE(10_11, 8_0);

typedef NS_ENUM(NSUInteger, MTLPrimitiveTopologyClass) {
    MTLPrimitiveTopologyClassUnspecified = 0,
    MTLPrimitiveTopologyClassPoint = 1,
    MTLPrimitiveTopologyClassLine = 2,
    MTLPrimitiveTopologyClassTriangle = 3,
} NS_ENUM_AVAILABLE(10_11, NA);

typedef NS_ENUM(NSUInteger, MTLTessellationPartitionMode) {
    MTLTessellationPartitionModePow2 = 0,
    MTLTessellationPartitionModeInteger = 1,
    MTLTessellationPartitionModeFractionalOdd = 2,
    MTLTessellationPartitionModeFractionalEven = 3,
} NS_ENUM_AVAILABLE(10_12, 10_0);

typedef NS_ENUM(NSUInteger, MTLTessellationFactorStepFunction) {
    MTLTessellationFactorStepFunctionConstant = 0,
    MTLTessellationFactorStepFunctionPerPatch = 1,
    MTLTessellationFactorStepFunctionPerInstance = 2,
    MTLTessellationFactorStepFunctionPerPatchAndPerInstance = 3,
} NS_ENUM_AVAILABLE(10_12, 10_0);

typedef NS_ENUM(NSUInteger, MTLTessellationFactorFormat) {
    MTLTessellationFactorFormatHalf = 0,
} NS_ENUM_AVAILABLE(10_12, 10_0);

typedef NS_ENUM(NSUInteger, MTLTessellationControlPointIndexType) {
    MTLTessellationControlPointIndexTypeNone = 0,
    MTLTessellationControlPointIndexTypeUInt16 = 1,
    MTLTessellationControlPointIndexTypeUInt32 = 2,
} NS_ENUM_AVAILABLE(10_12, 10_0);

@class MTLRenderPipelineColorAttachmentDescriptorArray;

NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLRenderPipelineColorAttachmentDescriptor : NSObject <NSCopying>

/*! Pixel format.  Defaults to MTLPixelFormatInvalid */
@property (nonatomic) MTLPixelFormat pixelFormat;

/*! Enable blending.  Defaults to NO. */
@property (nonatomic, getter = isBlendingEnabled) BOOL blendingEnabled;

/*! Defaults to MTLBlendFactorOne */
@property (nonatomic) MTLBlendFactor sourceRGBBlendFactor;

/*! Defaults to MTLBlendFactorZero */
@property (nonatomic) MTLBlendFactor destinationRGBBlendFactor;

/*! Defaults to MTLBlendOperationAdd */
@property (nonatomic) MTLBlendOperation rgbBlendOperation;

/*! Defaults to MTLBlendFactorOne */
@property (nonatomic) MTLBlendFactor sourceAlphaBlendFactor;

/*! Defaults to MTLBlendFactorZero */
@property (nonatomic) MTLBlendFactor destinationAlphaBlendFactor;

/*! Defaults to MTLBlendOperationAdd */
@property (nonatomic) MTLBlendOperation alphaBlendOperation;

/* Other Options */

/*! Defaults to MTLColorWriteMaskAll */
@property (nonatomic) MTLColorWriteMask writeMask;

@end


NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLRenderPipelineReflection : NSObject

@property (nullable, readonly) NSArray <MTLArgument *> *vertexArguments;
@property (nullable, readonly) NSArray <MTLArgument *> *fragmentArguments;
@end

NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLRenderPipelineDescriptor : NSObject <NSCopying>

@property (nullable, copy, nonatomic) NSString *label;

@property (nullable, readwrite, nonatomic, strong) id <MTLFunction> vertexFunction;
@property (nullable, readwrite, nonatomic, strong) id <MTLFunction> fragmentFunction;

@property (nullable, copy, nonatomic) MTLVertexDescriptor *vertexDescriptor;

/* Rasterization and visibility state */
@property (readwrite, nonatomic) NSUInteger sampleCount; //DEPRECATED - aliases rasterSampleCount property
@property (readwrite, nonatomic) NSUInteger rasterSampleCount;
@property (readwrite, nonatomic, getter = isAlphaToCoverageEnabled) BOOL alphaToCoverageEnabled;
@property (readwrite, nonatomic, getter = isAlphaToOneEnabled) BOOL alphaToOneEnabled;
@property (readwrite, nonatomic, getter = isRasterizationEnabled) BOOL rasterizationEnabled;



@property (readonly) MTLRenderPipelineColorAttachmentDescriptorArray *colorAttachments;

@property (nonatomic) MTLPixelFormat depthAttachmentPixelFormat;
@property (nonatomic) MTLPixelFormat stencilAttachmentPixelFormat;

@property (readwrite, nonatomic) MTLPrimitiveTopologyClass inputPrimitiveTopology NS_AVAILABLE(10_11, NA);

@property (readwrite, nonatomic) MTLTessellationPartitionMode tessellationPartitionMode NS_AVAILABLE(10_12, 10_0);
@property (readwrite, nonatomic) NSUInteger maxTessellationFactor NS_AVAILABLE(10_12, 10_0);
@property (readwrite, nonatomic, getter = isTessellationFactorScaleEnabled) BOOL tessellationFactorScaleEnabled NS_AVAILABLE(10_12, 10_0);
@property (readwrite, nonatomic) MTLTessellationFactorFormat tessellationFactorFormat NS_AVAILABLE(10_12, 10_0);
@property (readwrite, nonatomic) MTLTessellationControlPointIndexType tessellationControlPointIndexType NS_AVAILABLE(10_12, 10_0);
@property (readwrite, nonatomic) MTLTessellationFactorStepFunction tessellationFactorStepFunction NS_AVAILABLE(10_12, 10_0);
@property (readwrite, nonatomic) MTLWinding tessellationOutputWindingOrder NS_AVAILABLE(10_12, 10_0);

@property (readonly) MTLPipelineBufferDescriptorArray *vertexBuffers NS_AVAILABLE(10_13, 11_0);
@property (readonly) MTLPipelineBufferDescriptorArray *fragmentBuffers NS_AVAILABLE(10_13, 11_0);

/*!
 @method reset
 @abstract Restore all pipeline descriptor properties to their default values.
 */
- (void)reset;

@end

/*!
 @protocol MTLRenderPipelineState
 @abstract MTLRenderPipelineState represents a compiled render pipeline
 
 @discussion MTLRenderPipelineState is a compiled render pipeline and can be set on a MTLRenderCommandEncoder.
 */
NS_AVAILABLE(10_11, 8_0)
@protocol MTLRenderPipelineState <NSObject>

@property (nullable, readonly) NSString *label;
@property (readonly) id <MTLDevice> device;




@end

NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLRenderPipelineColorAttachmentDescriptorArray : NSObject

/* Individual attachment state access */
- (MTLRenderPipelineColorAttachmentDescriptor *)objectAtIndexedSubscript:(NSUInteger)attachmentIndex;

/* This always uses 'copy' semantics.  It is safe to set the attachment state at any legal index to nil, which resets that attachment descriptor state to default vaules. */
- (void)setObject:(nullable MTLRenderPipelineColorAttachmentDescriptor *)attachment atIndexedSubscript:(NSUInteger)attachmentIndex;

@end



NS_ASSUME_NONNULL_END
