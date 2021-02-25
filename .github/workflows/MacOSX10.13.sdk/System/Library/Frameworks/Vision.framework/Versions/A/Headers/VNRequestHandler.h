//
//  VNRequestHandler.h
//  Vision
//
//  Copyright © 2017 Apple Inc. All rights reserved.
//
/*!
 @header There are two types of request handlers: VNSequenceRequestHandler and VNImageRequestHandler.
 
 TODO:  re-document this
 
 Each of them are for scheduling @link VNRequest @/link. But the type of request dictates which kind of request handler to use. Some VNRequests can be used on multiple classes of VNTrackingHandlers while other require a specific one. Please refer to the documentation of the specific request. The general distinction between the 3 is that requests that are specific to an image (or buffer) would use the VNImageRequestHandler. Face detection is an example for the VNImageRequestHandler. For requests that span over multiple image buffers the request handler will hold on to past request data to allow algorithms to work on multiple buffers in a sequence and that sequence can be dynamic. 
 
 The base class VNSequenceRequestHandler is to be used for requests that are not tied to an image like Face Recognition that is based on already detected faces. Requests can be scheduled synchronous where the performRequests call waits for all requests to finish or asynchronous via performRequestsAsynchronous. performRequestsAsynchronous returns immediately. The client can monitor the completion of the requests through the completionHandler specified in each VNRequest.
 
 */


#import <Foundation/Foundation.h>

#import <CoreVideo/CVPixelBuffer.h>

#import <CoreImage/CoreImage.h>

#import <ImageIO/ImageIO.h>

#import <Vision/VNDefines.h>
#import <Vision/VNRequest.h>



NS_ASSUME_NONNULL_BEGIN

// Options keys  passed into the VNImageRequestHandler creations or requests that take an auxilary image

typedef NSString * VNImageOption NS_STRING_ENUM;

/*!
 @const      VNImageOptionProperties
 @abstract   VNImageOptionProperties is the dictionary from CGImageSourceCopyPropertiesAtIndex. This contains metadata that can be used by some algorithms
 */

VN_EXPORT VNImageOption const VNImageOptionProperties API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));

/*!
 @brief VNImageOptionCameraIntrinsics  Specifies the camera intrinsics as an NSData or CFData representing a matrix_float3x3. See kCMSampleBufferAttachmentKey_CameraIntrinsicMatrix for details
 @discussion
     Camera intrinsic matrix is a CFData containing a matrix_float3x3, which is column-major. It has the following contents:
     fx	0	ox
     0	fy	oy
     0	0	1
     fx and fy are the focal length in pixels. For square pixels, they will have the same value.
     ox and oy are the coordinates of the principal point. The origin is the upper left of the frame.

 */
VN_EXPORT VNImageOption const VNImageOptionCameraIntrinsics API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));




/*!
 @brief Performs requests on a single image.
 @discussion The VNImageRequestHandler is created with an image that is used to be used for the requests a client might want to schedule. The VNImageRequestHandler retains, but never modifies, the image source for its entire lifetime. The client also must not modify the content of the image source once the VNImageRequestHandler is created otherwise the results are undefined.
             The VNImageRequestHandler can choose to also cache intermediate representation of the image or other request-specific information for the purposes of runtime performance.
 */
API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0))
@interface VNImageRequestHandler : NSObject

- (instancetype)init NS_UNAVAILABLE;

/*!
 @brief initWithCVPixelBuffer:options creates a VNImageRequestHandler to be used for performing requests against the image passed in as buffer.
 
 @param pixelBuffer A CVPixelBuffer containing the image to be used for performing the requests. The content of the buffer cannot be modified for the lifetime of the VNImageRequestHandler.
 
 */
- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer options:(NSDictionary<VNImageOption, id> *)options;

/*!
 @brief initWithCVPixelBuffer:options creates a VNImageRequestHandler to be used for performing requests against the image passed in as buffer.
 
 @param pixelBuffer A CVPixelBuffer containing the image to be used for performing the requests. The content of the buffer cannot be modified for the lifetime of the VNImageRequestHandler.
 @param orientation The orientation of the image/buffer based on the EXIF specification. For details see kCGImagePropertyOrientation. The value has to be an integer from 1 to 8. This superceeds every other orientation information.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics
 */
- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer orientation:(CGImagePropertyOrientation)orientation options:(NSDictionary<VNImageOption, id> *)options;

/*!
 @brief initWithCGImage:options creates a VNImageRequestHandler to be used for performing requests against the image passed in as a CGImageRef.
 
 @param image A CGImageRef containing the image to be used for performing the requests. The content of the image cannot be modified.
 
 */
- (instancetype)initWithCGImage:(CGImageRef)image options:(NSDictionary<VNImageOption, id> *)options;


/*!
 
 @brief initWithCGImage:options creates a VNImageRequestHandler to be used for performing requests against the image passed in as a CGImageRef.
 
 @param image A CGImageRef containing the image to be used for performing the requests. The content of the image cannot be modified.
 @param orientation The orientation of the image/buffer based on the EXIF specification. For details see kCGImagePropertyOrientation. The value has to be an integer from 1 to 8. This superceeds every other orientation information.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics

 */
- (instancetype)initWithCGImage:(CGImageRef)image orientation:(CGImagePropertyOrientation)orientation options:(NSDictionary<VNImageOption, id> *)options;


/*!
 @brief initWithCIImage:options creates a VNImageRequestHandler to be used for performing requests against the image passed in as a CIImage.
 
 @param image A CIImage containing the image to be used for performing the requests. The content of the image cannot be modified.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics
 
 
 @note:  Request results may not be accurate in simulator due to CI's inability to render certain pixel formats in the simulator. The orientation of the original image should be applied for instance by using imageByApplyingOrientation or use the initWithCIImage:options:orientation API.
 */
- (instancetype)initWithCIImage:(CIImage *)image options:(NSDictionary<VNImageOption, id> *)options;


/*!
 @brief initWithCIImage:options:orientation creates a VNImageRequestHandler to be used for performing requests against the image passed in as a CIImage.
 
 @param image A CIImage containing the image to be used for performing the requests. The content of the image cannot be modified.
 @param orientation The orientation of the image/buffer based on the EXIF specification. For details see kCGImagePropertyOrientation. The value has to be an integer from 1 to 8. This superceeds every other orientation information.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics

 
 @note:  Request results may not be accurate in simulator due to CI's inability to render certain pixel formats in the simulator
 */
- (instancetype)initWithCIImage:(CIImage *)image orientation:(CGImagePropertyOrientation)orientation options:(NSDictionary<VNImageOption, id> *)options;


/*!
 @brief initWithURL:options creates a VNImageRequestHandler to be used for performing requests against an image specified by it's URL
 
 @param imageURL A URL pointing at an image to be used for performing the requests. The image has to be in a format that is supported by ImageIO. The content of the image cannot be modified.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics
 
 @note:  Request results may not be accurate in simulator due to CI's inability to render certain pixel formats in the simulator
 */
- (instancetype)initWithURL:(NSURL *)imageURL options:(NSDictionary<VNImageOption, id> *)options;


/*!
 @brief initWithURL:options creates a VNImageRequestHandler to be used for performing requests against an image specified by it's URL
 
 @param imageURL A URL pointing at an image to be used for performing the requests. The image has to be in a format that is supported by ImageIO. The content of the image cannot be modified.
 @param orientation The orientation of the image/buffer based on the EXIF specification. For details see kCGImagePropertyOrientation. The value has to be an integer from 1 to 8. This superceeds every other orientation information.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics

 @note:  Request results may not be accurate in simulator due to CI's inability to render certain pixel formats in the simulator
 */
- (instancetype)initWithURL:(NSURL *)imageURL orientation:(CGImagePropertyOrientation)orientation options:(NSDictionary<VNImageOption, id> *)options;


/*!
 @brief initWithData:options creates a VNImageRequestHandler to be used for performing requests against an image contained in an NSData object.
 
 @param imageData An NSData object containing the content of the image to be used for performing the requests. See CIImage imageWithData for supported format. The content of the image cannot be modified.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics
 
 @note:  Request results may not be accurate in simulator due to CI's inability to render certain pixel formats in the simulator
 
 */
- (instancetype)initWithData:(NSData *)imageData options:(NSDictionary<VNImageOption, id> *)options;


/*!
 @brief initWithData:options creates a VNImageRequestHandler to be used for performing requests against an image contained in an NSData object.
 
 @param imageData An NSData object containing the content of the image to be used for performing the requests. See CIImage imageWithData for supported format. The content of the image cannot be modified.
 @param orientation The orientation of the image/buffer based on the EXIF specification. For details see kCGImagePropertyOrientation. The value has to be an integer from 1 to 8. This superceeds every other orientation information.
 @param options A dictionary with options specifying auxilary information for the buffer/image like VNImageOptionCameraIntrinsics

 @note:  Request results may not be accurate in simulator due to CI's inability to render certain pixel formats in the simulator
 
 */
- (instancetype)initWithData:(NSData *)imageData orientation:(CGImagePropertyOrientation)orientation options:(NSDictionary<VNImageOption, id> *)options;


/*!
 @brief performRequests schedules one or more VNRequests to be performed. The function returns once all requests have been finished.
 
 @discussion The results of the VNRequests as well any possible errors of the individual requests are reported in the VNRequests results and error properties.
 
 @param requests An NSArray of VNRequests that are to be performed.
 @param error Returns an error that happened during scheduling of the requests. Check individual requests results and errors for their respective success and failures. This parameter is optional.
 @return Returns true if all requests were scheduled and performed. Check individual requests results and errors for their respective success and failures.
 
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests error:(NSError **)error;


@end





/*!
 @brief Performs requests on a sequence of images.
 @discussion The VNSequenceRequestHandler is created without any specific image source.  The -performRequests:on<ImageSource>:error: methods will retain the image source for no longer than the lifetime of the call.
             The VNSequenceRequestHandler can choose to also cache state information related to the previously-processed image sources.
 */
API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0))
@interface VNSequenceRequestHandler : NSObject


/*!
	@brief	Creates a new object.
*/
- (instancetype)init NS_DESIGNATED_INITIALIZER;

/*!
	@brief Perform requests on an image in a CVPixelBuffer.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	pixelBuffer		The CVPixelBuffer containing the image to be processed.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onCVPixelBuffer:(CVPixelBufferRef)pixelBuffer error:(NSError **)error;

/*!
	@brief Perform requests on an image in a CVPixelBuffer.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	pixelBuffer		The CVPixelBuffer containing the image to be processed.
	
	@param	orientation		The orientation of the image as it is captured in the pixel buffer.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onCVPixelBuffer:(CVPixelBufferRef)pixelBuffer orientation:(CGImagePropertyOrientation)orientation error:(NSError **)error;

/*!
	@brief Perform requests on an image in a CGImageRef.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	image			The CGImageRef containing the image to be processed.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onCGImage:(CGImageRef)image error:(NSError **)error;


/*!
	@brief Perform requests on an image in a CGImageRef.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	image			The CGImageRef containing the image to be processed.
	
	@param	orientation		The orientation of the image.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onCGImage:(CGImageRef)image orientation:(CGImagePropertyOrientation)orientation error:(NSError **)error;


/*!
	@brief Perform requests on an image in a CIImage.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	image			The CIImage containing the image to be processed.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onCIImage:(CIImage*)image error:(NSError **)error;


/*!
	@brief Perform requests on an image in a CIImage.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	image			The CIImage containing the image to be processed.
	
	@param	orientation		The orientation of the image.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onCIImage:(CIImage*)image orientation:(CGImagePropertyOrientation)orientation error:(NSError **)error;


/*!
	@brief Perform requests on an image referenced by an URL.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	imageURL		The URL of the image to be processed.  If this is not a file-based URL, the method will fail.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onImageURL:(NSURL*)imageURL error:(NSError **)error;


/*!
	@brief Perform requests on an image referenced by an URL.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	imageURL		The URL of the image to be processed.  If this is not a file-based URL, the method will fail.

	@param	orientation		The orientation of the image.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onImageURL:(NSURL*)imageURL orientation:(CGImagePropertyOrientation)orientation error:(NSError **)error;


/*!
	@brief Perform requests on an image with its source format in memory.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	imageData		The data representing the source format of the image to be processed.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onImageData:(NSData*)imageData error:(NSError **)error;


/*!
	@brief Perform requests on an image with its source format in memory.
	
	@param	requests		The VNRequests to be performed on the image.
	
	@param	imageData		The data representing the source format of the image to be processed.

	@param	orientation		The orientation of the image.
	
	@param	error			On input, a pointer to an error object. If an error occurs, this pointer is set to an actual error object containing the error information. You may specify NULL for this parameter if you do not want the error information.
 */
- (BOOL)performRequests:(NSArray<VNRequest *> *)requests onImageData:(NSData*)imageData orientation:(CGImagePropertyOrientation)orientation error:(NSError **)error;


@end


NS_ASSUME_NONNULL_END
