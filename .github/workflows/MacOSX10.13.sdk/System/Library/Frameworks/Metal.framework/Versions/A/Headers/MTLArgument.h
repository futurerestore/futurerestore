//
//  MTLArgument.h
//  Metal
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Metal/MTLDefines.h>
#import <Metal/MTLTexture.h>


NS_ASSUME_NONNULL_BEGIN
typedef NS_ENUM(NSUInteger, MTLDataType){
    
    MTLDataTypeNone = 0,
    
    MTLDataTypeStruct = 1,
    MTLDataTypeArray  = 2,
    
    MTLDataTypeFloat  = 3,
    MTLDataTypeFloat2 = 4,
    MTLDataTypeFloat3 = 5,
    MTLDataTypeFloat4 = 6,
    
    MTLDataTypeFloat2x2 = 7,
    MTLDataTypeFloat2x3 = 8,
    MTLDataTypeFloat2x4 = 9,
    
    MTLDataTypeFloat3x2 = 10,
    MTLDataTypeFloat3x3 = 11,
    MTLDataTypeFloat3x4 = 12,
    
    MTLDataTypeFloat4x2 = 13,
    MTLDataTypeFloat4x3 = 14,
    MTLDataTypeFloat4x4 = 15,
    
    MTLDataTypeHalf  = 16,
    MTLDataTypeHalf2 = 17,
    MTLDataTypeHalf3 = 18,
    MTLDataTypeHalf4 = 19,
    
    MTLDataTypeHalf2x2 = 20,
    MTLDataTypeHalf2x3 = 21,
    MTLDataTypeHalf2x4 = 22,
    
    MTLDataTypeHalf3x2 = 23,
    MTLDataTypeHalf3x3 = 24,
    MTLDataTypeHalf3x4 = 25,
    
    MTLDataTypeHalf4x2 = 26,
    MTLDataTypeHalf4x3 = 27,
    MTLDataTypeHalf4x4 = 28,
    
    MTLDataTypeInt  = 29,
    MTLDataTypeInt2 = 30,
    MTLDataTypeInt3 = 31,
    MTLDataTypeInt4 = 32,
    
    MTLDataTypeUInt  = 33,
    MTLDataTypeUInt2 = 34,
    MTLDataTypeUInt3 = 35,
    MTLDataTypeUInt4 = 36,
    
    MTLDataTypeShort  = 37,
    MTLDataTypeShort2 = 38,
    MTLDataTypeShort3 = 39,
    MTLDataTypeShort4 = 40,
    
    MTLDataTypeUShort = 41,
    MTLDataTypeUShort2 = 42,
    MTLDataTypeUShort3 = 43,
    MTLDataTypeUShort4 = 44,
    
    MTLDataTypeChar  = 45,
    MTLDataTypeChar2 = 46,
    MTLDataTypeChar3 = 47,
    MTLDataTypeChar4 = 48,
    
    MTLDataTypeUChar  = 49,
    MTLDataTypeUChar2 = 50,
    MTLDataTypeUChar3 = 51,
    MTLDataTypeUChar4 = 52,
    
    MTLDataTypeBool  = 53,
    MTLDataTypeBool2 = 54,
    MTLDataTypeBool3 = 55,
    MTLDataTypeBool4 = 56,

    
    MTLDataTypeTexture NS_ENUM_AVAILABLE(10_13, 11_0) = 58,
    MTLDataTypeSampler NS_ENUM_AVAILABLE(10_13, 11_0) = 59,
    MTLDataTypePointer NS_ENUM_AVAILABLE(10_13, 11_0) = 60,

} NS_ENUM_AVAILABLE(10_11, 8_0);

@class MTLArgument;

/*!
 @enum MTLArgumentType
 @abstract The type for an input to a MTLRenderPipelineState or a MTLComputePipelineState
 
 @constant MTLArgumentTypeBuffer
 This input is a MTLBuffer
 
 @constant MTLArgumentTypeThreadgroupMemory
 This input is a pointer to the threadgroup memory.
 
 @constant MTLArgumentTypeTexture
 This input is a MTLTexture.
 
 @constant MTLArgumentTypeSampler
 This input is a sampler.
*/
typedef NS_ENUM(NSUInteger, MTLArgumentType) {
    
    MTLArgumentTypeBuffer = 0,
    MTLArgumentTypeThreadgroupMemory= 1,
    MTLArgumentTypeTexture = 2,
    MTLArgumentTypeSampler = 3,


} NS_ENUM_AVAILABLE(10_11, 8_0);

/*!
 @enum MTLArgumentAccess
*/
typedef NS_ENUM(NSUInteger, MTLArgumentAccess) {
    
    MTLArgumentAccessReadOnly   = 0,
    MTLArgumentAccessReadWrite  = 1,
    MTLArgumentAccessWriteOnly  = 2,
    
} NS_ENUM_AVAILABLE(10_11, 8_0);

@class MTLStructType;
@class MTLArrayType;
@class MTLTextureReferenceType;
@class MTLPointerType;

NS_CLASS_AVAILABLE(10_13, 11_0)
@interface MTLType : NSObject
@property (readonly) MTLDataType dataType;
@end

NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLStructMember : NSObject
 
@property (readonly) NSString *name;
@property (readonly) NSUInteger offset;
@property (readonly) MTLDataType dataType;

- (nullable MTLStructType *)structType;
- (nullable MTLArrayType *)arrayType;

- (nullable MTLTextureReferenceType *)textureReferenceType  NS_AVAILABLE(10_13, 11_0);
- (nullable MTLPointerType *)pointerType  NS_AVAILABLE(10_13, 11_0);

@property (readonly) NSUInteger argumentIndex NS_AVAILABLE(10_13, 11_0);


@end
 
NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLStructType  : MTLType

@property (readonly) NSArray <MTLStructMember *> *members;

- (nullable MTLStructMember *)memberByName:(NSString *)name;

@end

NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLArrayType  : MTLType

@property (readonly) MTLDataType elementType;

@property (readonly) NSUInteger arrayLength;
@property (readonly) NSUInteger stride;
@property (readonly) NSUInteger argumentIndexStride NS_AVAILABLE(10_13, 11_0);
- (nullable MTLStructType *)elementStructType;
- (nullable MTLArrayType *)elementArrayType;
- (nullable MTLTextureReferenceType *)elementTextureReferenceType  NS_AVAILABLE(10_13, 11_0);
- (nullable MTLPointerType *)elementPointerType  NS_AVAILABLE(10_13, 11_0);


@end

NS_CLASS_AVAILABLE(10_13, 11_0)
@interface MTLPointerType : MTLType

@property (readonly) MTLDataType elementType;           // MTLDataTypeFloat, MTLDataTypeFloat4, MTLDataTypeStruct, ...
@property (readonly) MTLArgumentAccess access;
@property (readonly) NSUInteger alignment;              // min alignment for the element data
@property (readonly) NSUInteger dataSize;               // sizeof(T) for T *argName

@property (readonly) BOOL elementIsArgumentBuffer NS_AVAILABLE(10_13, 11_0);

- (nullable MTLStructType *)elementStructType  NS_AVAILABLE(10_13, 11_0);
- (nullable MTLArrayType *)elementArrayType  NS_AVAILABLE(10_13, 11_0);

@end

NS_CLASS_AVAILABLE(10_13, 11_0)
@interface MTLTextureReferenceType : MTLType

@property (readonly) MTLDataType textureDataType; // half, float, int, or uint.
@property (readonly) MTLTextureType textureType;  // texture1D, texture2D...
@property (readonly) MTLArgumentAccess access;    // read, write, read-write
@property (readonly) BOOL isDepthTexture;         // true for depth textures


@end

/*!
 MTLArgument
*/
NS_CLASS_AVAILABLE(10_11, 8_0)
@interface MTLArgument : NSObject

@property (readonly) NSString *name;
@property (readonly) MTLArgumentType type;
@property (readonly) MTLArgumentAccess access;
@property (readonly) NSUInteger index;


@property (readonly, getter=isActive) BOOL active;

// for buffer arguments
@property (readonly) NSUInteger      bufferAlignment;		// min alignment of starting offset in the buffer
@property (readonly) NSUInteger      bufferDataSize; 		// sizeof(T) for T *argName
@property (readonly) MTLDataType     bufferDataType; 		// MTLDataTypeFloat, MTLDataTypeFloat4, MTLDataTypeStruct, ...
@property (readonly) MTLStructType  *bufferStructType;
@property (readonly, nullable) MTLPointerType *bufferPointerType NS_AVAILABLE(10_13, 11_0);


// for threadgroup memory arguments
@property (readonly) NSUInteger     threadgroupMemoryAlignment;
@property (readonly) NSUInteger     threadgroupMemoryDataSize; // sizeof(T) for T *argName

// for texture arguments
@property (readonly) MTLTextureType textureType; // texture1D, texture2D...
@property (readonly) MTLDataType    textureDataType; // half, float, int, or uint.
@property (readonly) BOOL           isDepthTexture NS_AVAILABLE(10_12, 10_0); // true for depth textures
@property (readonly) NSUInteger     arrayLength NS_AVAILABLE(10_13, 10_0);

@end

NS_ASSUME_NONNULL_END
