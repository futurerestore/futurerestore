/*!
 *  @header MPSImageConvolution.h
 *  @framework MetalPerformanceShaders
 *  @copyright Copyright (c) 2015 Apple Inc. All rights reserved.
 *
 *  @abstract MetalPerformanceShaders Convolution Filters
 */

#ifndef MPS_MSImageConvolution_h
#define MPS_MSImageConvolution_h

#include <MPSImage/MPSImageKernel.h>

/*!
 *  @class      MPSImageConvolution
 *  @discussion The MPSImageConvolution convolves an image with given filter of odd width and height.
 *              The center of the kernel aligns with the MPSImageConvolution.offset. That is, the position 
 *              of the top left corner of the area covered by the kernel is given by 
 *              MPSImageConvolution.offset - {kernel_width>>1, kernel_height>>1, 0}
 *
 *              Optimized cases include 3x3,5x5,7x7,9x9,11x11, 1xN and Nx1. If a convolution kernel 
 *              does not fall into one of these cases but is a rank-1 matrix (a.k.a. separable)
 *              then it will fall on an optimzied separable path. Other convolutions will execute with
 *              full MxN complexity.
 *
 *              If there are multiple channels in the source image, each channel is processed independently.
 *  
 *  @performance Separable convolution filters may perform better when done in two passes. A convolution filter
 *              is separable if the ratio of filter values between all rows is constant over the whole row. For
 *              example, this edge detection filter:
 *                  @code
 *                      -1      0       1
 *                      -2      0       2
 *                      -1      0       1
 *                  @endcode
 *              can be separated into the product of two vectors:
 *                  @code
 *                      1
 *                      2      x    [-1  0   1]
 *                      1
 *                  @endcode
 *              and consequently can be done as two, one-dimensional convolution passes back to back on the same image. 
 *              In this way, the number of multiplies (ignoring the fact that we could skip zeros here) is reduced from
 *              3*3=9 to 3+3 = 6. There are similar savings for addition. For large filters, the savings can be profound.
 *
 */

MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(9.0), tvos(9.0))
@interface  MPSImageConvolution : MPSUnaryImageKernel

/*! @property kernelHeight
 *  @abstract  The height of the filter window. Must be an odd number.
 */
@property (readonly, nonatomic)   NSUInteger  kernelHeight;

/*! @property kernelWidth
 *  @abstract  The width of the filter window. Must be an odd number.
 */
@property (readonly, nonatomic)   NSUInteger  kernelWidth;


/*! @property    bias
 *  @discussion  The bias is a value to be added to convolved pixel before it is converted back to the storage format.
 *               It can be used to convert negative values into a representable range for a unsigned MTLPixelFormat.
 *               For example, many edge detection filters produce results in the range [-k,k]. By scaling the filter
 *               weights by 0.5/k and adding 0.5, the results will be in range [0,1] suitable for use with unorm formats. 
 *               It can be used in combination with renormalization of the filter weights to do video ranging as part 
 *               of the convolution effect. It can also just be used to increase the brightness of the image.
 *
 *               Default value is 0.0f.
 */
@property (readwrite, nonatomic) float bias;

/*!
 *  @abstract  Initialize a convolution filter
 *  @param      device          The device the filter will run on
 *  @param      kernelWidth     the width of the kernel
 *  @param      kernelHeight    the height of the kernel
 *  @param      kernelWeights   A pointer to an array of kernelWidth * kernelHeight values to be used as the kernel.
 *                              These are in row major order.
 *  @return     A valid MPSImageConvolution object or nil, if failure.
 */

-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device
                           kernelWidth: (NSUInteger)kernelWidth
                          kernelHeight: (NSUInteger)kernelHeight
                               weights: (const float*__nonnull)kernelWeights     NS_DESIGNATED_INITIALIZER;


/*! @abstract NSSecureCoding compatability
 *  @discussion While the standard NSSecureCoding/NSCoding method
 *              -initWithCoder: should work, since the file can't
 *              know which device your data is allocated on, we
 *              have to guess and may guess incorrectly.  To avoid
 *              that problem, use initWithCoder:device instead.
 *  @param      aDecoder    The NSCoder subclass with your serialized MPSKernel
 *  @param      device      The MTLDevice on which to make the MPSKernel
 *  @return     A new MPSKernel object, or nil if failure.
 */
-(nullable instancetype) initWithCoder:(NSCoder * __nonnull)aDecoder
                                device:(nonnull id <MTLDevice>) device NS_DESIGNATED_INITIALIZER
                MPS_AVAILABLE_STARTING(macos(10.13), ios(11.0), tvos(11.0));

@end


/*!
 *  @class      MPSImageLaplacian
 *  @discussion The MPSImageLaplacian is an optimized variant of the MPSImageConvolution filter provided primarily for ease of use.
 *              This filter uses an optimized convolution filter with a 3 x 3 kernel with the following weights:
 *                  [ 0  1  0
 *                    1 -4  1
 *                    0  1  0 ]
 *
 *              The optimized convolution filter used by MPSImageLaplacian can also be used by creating a MPSImageConvolution
 *              object with kernelWidth = 3, kernelHeight = 3 and weights as specified above.
 */
MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(10.0), tvos(10.0))
@interface  MPSImageLaplacian : MPSUnaryImageKernel

/*! @property    bias
 *  @discussion  The bias is a value to be added to convolved pixel before it is converted back to the storage format.
 *               It can be used to convert negative values into a representable range for a unsigned MTLPixelFormat.
 *               For example, many edge detection filters produce results in the range [-k,k]. By scaling the filter
 *               weights by 0.5/k and adding 0.5, the results will be in range [0,1] suitable for use with unorm formats.
 *               It can be used in combination with renormalization of the filter weights to do video ranging as part
 *               of the convolution effect. It can also just be used to increase the brightness of the image.
 *
 *               Default value is 0.0f.
 */
@property (readwrite, nonatomic) float bias;

@end


/*!
 *  @class      MPSImageBox
 *  @discussion The MPSImageBox convolves an image with given filter of odd width and height. The kernel elements
 *              all have equal weight, achieving a blur effect. (Each result is the unweighted average of the
 *              surrounding pixels.) This allows for much faster algorithms, espcially for larger blur radii.
 *              The box height and width must be odd numbers. The box blur is a separable filter. The implementation 
 *              is aware of this and will act accordingly to give best performance for multi-dimensional blurs.
 */

MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(9.0), tvos(9.0))
@interface  MPSImageBox : MPSUnaryImageKernel


/*! @property kernelHeight
 *  @abstract  The height of the filter window.
 */
@property (readonly, nonatomic)   NSUInteger  kernelHeight;

/*! @property kernelWidth
 *  @abstract  The width of the filter window.
 */
@property (readonly, nonatomic)   NSUInteger  kernelWidth;

/*! @abstract   Initialize a filter for a particular kernel size and device
 *  @param      device  The device the filter will run on
 *  @param      kernelWidth  the width of the kernel.  Must be an odd number.
 *  @param      kernelHeight the height of the kernel. Must be an odd number.
 *  @return     A valid object or nil, if failure.
 */

-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device
                           kernelWidth: (NSUInteger)kernelWidth
                          kernelHeight: (NSUInteger)kernelHeight        NS_DESIGNATED_INITIALIZER;

/*! @abstract NSSecureCoding compatability
 *  @discussion While the standard NSSecureCoding/NSCoding method
 *              -initWithCoder: should work, since the file can't
 *              know which device your data is allocated on, we
 *              have to guess and may guess incorrectly.  To avoid
 *              that problem, use initWithCoder:device instead.
 *  @param      aDecoder    The NSCoder subclass with your serialized MPSKernel
 *  @param      device      The MTLDevice on which to make the MPSKernel
 *  @return     A new MPSKernel object, or nil if failure.
 */
-(nullable instancetype) initWithCoder:(NSCoder * __nonnull)aDecoder
                                device:(nonnull id <MTLDevice>) device NS_DESIGNATED_INITIALIZER
                MPS_AVAILABLE_STARTING(macos(10.13), ios(11.0), tvos(11.0));

/* You must use initWithDevice:kernelWidth:kernelHeight: instead. */
-(nonnull instancetype) initWithDevice:(nonnull id<MTLDevice>)device    NS_UNAVAILABLE;
@end

/*!
 *  @class      MPSImageTent
 *  @discussion The box filter, while fast, may yield square-ish looking blur effects. However, multiple
 *              passes of the box filter tend to smooth out with each additional pass. For example, two 3-wide
 *              box blurs produces the same effective convolution as a 5-wide tent blur:
 *              @code
 *                      1   1   1
 *                          1   1   1
 *                      +       1   1   1
 *                      =================
 *                      1   2   3   2   1
 *              @endcode
 *              Addition passes tend to approximate a gaussian line shape.
 *
 *              The MPSImageTent convolves an image with a tent filter. These form a tent shape with incrementally
 *              increasing sides, for example:
 *
 *                  1   2   3   2   1
 *
 *
 *                  1   2   1
 *                  2   4   2
 *                  1   2   1
 *
 *              Like the box filter, this arrangement allows for much faster algorithms, espcially for for larger blur
 *              radii but with a more pleasing appearance.
 *
 *              The tent blur is a separable filter. The implementation is aware of this and will act accordingly
 *              to give best performance for multi-dimensional blurs.
 */
MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(9.0), tvos(9.0))
@interface MPSImageTent : MPSImageBox

@end

/*!
 *  @class      MPSImageGaussianBlur
 *  @discussion The MPSImageGaussianBlur convolves an image with gaussian of given sigma in both x and y direction.
 *
 *                  The MPSImageGaussianBlur utilizes a very fast algorith that typically runs at approximately
 *                  1/2 of copy speeds. Notably, it is faster than either the tent or box blur except perhaps
 *                  for very large filter windows. Mathematically, it is an approximate gaussian. Some
 *                  non-gaussian behavior may be detectable with advanced analytical methods such as FFT.  
 *                  If a analytically clean gaussian filter is required, please use the MPSImageConvolution 
 *                  filter instead with an appropriate set of weights. The MPSImageGaussianBlur is intended
 *                  to be suitable for all common image processing needs demanding ~10 bits of precision or
 *                  less.
 */
MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(9.0), tvos(9.0))
@interface  MPSImageGaussianBlur : MPSUnaryImageKernel

/*! @abstract   Initialize a gaussian blur filter for a particular sigma and device
 *  @param      device  The device the filter will run on
 *  @param      sigma   The standard deviation of gaussian blur filter. 
 *                      Gaussian weight, centered at 0, at integer grid i is given as 
 *                            w(i) = 1/sqrt(2*pi*sigma) * exp(-i^2/2*sigma^2)
 *                      If we take cut off at 1% of w(0) (max weight) beyond which weights
 *                      are considered 0, we have 
 *                              ceil (sqrt(-log(0.01)*2)*sigma) ~ ceil(3.7*sigma) 
 *                      as rough estimate of filter width
 *  @return     A valid object or nil, if failure.
 */

-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device
                                 sigma: (float)sigma                   NS_DESIGNATED_INITIALIZER;

/*! @abstract NSSecureCoding compatability
 *  @discussion While the standard NSSecureCoding/NSCoding method
 *              -initWithCoder: should work, since the file can't
 *              know which device your data is allocated on, we
 *              have to guess and may guess incorrectly.  To avoid
 *              that problem, use initWithCoder:device instead.
 *  @param      aDecoder    The NSCoder subclass with your serialized MPSKernel
 *  @param      device      The MTLDevice on which to make the MPSKernel
 *  @return     A new MPSKernel object, or nil if failure.
 */
-(nullable instancetype) initWithCoder:(NSCoder * __nonnull)aDecoder
                                device:(nonnull id <MTLDevice>) device NS_DESIGNATED_INITIALIZER
                                MPS_AVAILABLE_STARTING(macos(10.13), ios(11.0), tvos(11.0));


/* You must use initWithDevice:sigma: instead. */
-(nonnull instancetype) initWithDevice:(nonnull id<MTLDevice>)device    NS_UNAVAILABLE;

/*! @property sigma
 *  @abstract Read-only sigma value with which filter was created
 */
@property (readonly, nonatomic) float sigma;

@end

/*!
 *  @class      MPSImageSobel
 *  @discussion The MPSImageSobel implements the Sobel filter.
 *              When the color model (e.g. RGB, two-channel, grayscale, etc.) of source 
 *              and destination textures match, the filter is applied to each channel 
 *              separately. If the destination is monochrome (single channel) but source 
 *              multichannel, the pixel values are converted to grayscale before applying Sobel
 *              operator using the linear gray color transform vector (v).
 *
 *                  Luminance = v[0] * pixel.x + v[1] * pixel.y + v[2] * pixel.z;
 */
MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(9.0), tvos(9.0))
@interface  MPSImageSobel : MPSUnaryImageKernel

/*! @abstract   Initialize a Sobel filter on a given device using the default color 
 *              transform. Default: BT.601/JPEG {0.299f, 0.587f, 0.114f}
 *
 *              For non-default conversion matrices, use -initWithDevice:linearGrayColorTransform:
 *
 *  @param      device  The device the filter will run on
 *  @return     A valid object or nil, if failure.
 */
-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device;

/*! @abstract   Initialize a Sobel filter on a given device with a non-default color transform
 *  @param      device          The device the filter will run on
 *  @param      transform       Array of three floats describing the rgb to gray scale color transform.
 *                @code
 *                          Luminance = transform[0] * pixel.x +
 *                                      transform[1] * pixel.y +
 *                                      transform[2] * pixel.z;
 *                @endcode
 *  @return     A valid object or nil, if failure.
 */
-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device
              linearGrayColorTransform: (const float * __nonnull) transform      NS_DESIGNATED_INITIALIZER;

/*! @abstract NSSecureCoding compatability
 *  @discussion While the standard NSSecureCoding/NSCoding method
 *              -initWithCoder: should work, since the file can't
 *              know which device your data is allocated on, we
 *              have to guess and may guess incorrectly.  To avoid
 *              that problem, use initWithCoder:device instead.
 *  @param      aDecoder    The NSCoder subclass with your serialized MPSKernel
 *  @param      device      The MTLDevice on which to make the MPSKernel
 *  @return     A new MPSKernel object, or nil if failure.
 */
-(nullable instancetype) initWithCoder:(NSCoder * __nonnull)aDecoder
                                device:(nonnull id <MTLDevice>) device NS_DESIGNATED_INITIALIZER
                MPS_AVAILABLE_STARTING(macos(10.13), ios(11.0), tvos(11.0));


/*! @property    colorTransform
 *  @discussion  Returns a pointer to the array of three floats used to convert RGBA, RGB or RG images
 *               to the destination format when the destination is monochrome.
 */
@property (readonly, nonatomic, nonnull) const float* colorTransform;

@end  /* MPSImageSobel */



/*!
 *  @class      MPSImagePyramid
 *  @discussion The MPSImagePyramid is a base class for creating different kinds of pyramid images
 *
 *              Currently supported pyramid-types are:
 *              @ref MPSImageGaussianPyramid
 *
 *              The Gaussian image pyramid kernel is enqueued as a in-place operation using
 *              @ref MPSUnaryImageKernel::encodeToCommandBuffer:inPlaceTexture:fallbackCopyAllocator:
 *              and all mipmap levels after level=1, present in the provided image are filled using
 *              the provided filtering kernel. The fallbackCopyAllocator parameter is not used.
 *
 *              The Gaussian image pyramid filter ignores @ref clipRect and @ref offset and fills
 *              the entire mipmap levels.
 *
 *  @note       Make sure your texture type is compatible with mipmapping and supports texture views
 *                  (see @ref MTLTextureUsagePixelFormatView).
 *  @note       Recall the size of the nth mipmap level:
 *              @code
 *                  w_n = max(1, floor(w_0 / 2^n))
 *                  h_n = max(1, floor(h_0 / 2^n)),
 *              @endcode
 *              where w_0, h_0 are the zeroth level width and height. ie the image dimensions themselves.
 */

MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(10.0), tvos(10.0))
@interface  MPSImagePyramid : MPSUnaryImageKernel

/*! @abstract   Initialize a downwards 5-tap image pyramid with the default filter kernel and device
 *  @param      device  The device the filter will run on
 *
 *  @discussion The filter kernel is the outer product of w = [ 1/16,  1/4,  3/8,  1/4,  1/16 ]^T, with itself
 *
 *  @return     A valid object or nil, if failure.
 */

-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device;


/*! @abstract   Initialize a downwards 5-tap image pyramid with a central weight parameter and device
 *  @param      device  The device the filter will run on
 *  @param      centerWeight Defines form of the filter-kernel  through the outer product ww^T, where
 *              w = [ (1/4 - a/2),  1/4,  a,  1/4,  (1/4 - a/2) ]^T and 'a' is centerWeight.
 *
 *  @return     A valid object or nil, if failure.
 */

-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device
                          centerWeight: (float) centerWeight;


/*! @abstract   Initialize a downwards n-tap pyramid with a custom filter kernel and device
 *  @param      device  The device the filter will run on
 *  @param      kernelWidth The width of the filtering kernel. See @ref MPSImageConvolution.
 *  @param      kernelHeight    The height of the filtering kernel. See @ref MPSImageConvolution.
 *  @param      kernelWeights   A pointer to an array of kernelWidth * kernelHeight values to be
 *                              used as the kernel.
 *                              These are in row major order. See @ref MPSImageConvolution.
 *
 *  @return     A valid object or nil, if failure.
 */

-(nonnull instancetype) initWithDevice: (nonnull id <MTLDevice>) device
                           kernelWidth: (NSUInteger)kernelWidth
                          kernelHeight: (NSUInteger)kernelHeight
                               weights: (const float*__nonnull)kernelWeights NS_DESIGNATED_INITIALIZER;


/*! @property kernelHeight
 *  @abstract  The height of the filter window. Must be an odd number.
 */
@property (readonly, nonatomic)   NSUInteger  kernelHeight;

/*! @property kernelWidth
 *  @abstract  The width of the filter window. Must be an odd number.
 */
@property (readonly, nonatomic)   NSUInteger  kernelWidth;

/*! @abstract NSSecureCoding compatability
 *  @discussion See @ref MPSKernel#initWithCoder.
 *  @param      aDecoder    The NSCoder subclass with your serialized MPSCNNPooling
 *  @param      device      The MTLDevice on which to make the MPSCNNPooling
 *  @return     A new MPSCNNPooling object, or nil if failure.
 */
-(nullable instancetype) initWithCoder:(NSCoder * __nonnull)aDecoder
                                device:(nonnull id <MTLDevice>) device NS_DESIGNATED_INITIALIZER
                        MPS_AVAILABLE_STARTING(macos(10.13), ios(11.0), tvos(11.0));

@end

/*!
 *  @class      MPSImageGaussianPyramid
 *  @discussion The Gaussian image pyramid is constructed as follows:
 *              First the zeroth level mipmap of the input image is filtered with the specified
 *              convolution kernel.
 *              The default the convolution filter kernel is
 *              @code
 *                  k = w w^T, where w = [ 1/16,  1/4,  3/8,  1/4,  1/16 ]^T,
 *              @endcode
 *              but the user may also tweak this kernel with a @ref centerWeight parameter: 'a':
 *              @code
 *                  k = w w^T, where w = [ (1/4 - a/2),  1/4,  a,  1/4,  (1/4 - a/2) ]^T
 *              @endcode
 *              or the user can provide a completely custom kernel. After this the image is downsampled by
 *              removing all odd rows and columns, which defines the next level in the Gaussian image pyramid.
 *              This procedure is continued until every mipmap level present in the image texture are
 *              filled with the pyramid levels.
 *
 *              In case of the Gaussian pyramid the user must run the operation in-place using:
 *              @ref MPSUnaryImageKernel::encodeToCommandBuffer:inPlaceTexture:fallbackCopyAllocator:,
 *              where the fallback allocator is ignored.
 */

MPS_CLASS_AVAILABLE_STARTING( macos(10.13), ios(10.0), tvos(10.0))
@interface  MPSImageGaussianPyramid : MPSImagePyramid
@end


#endif    /* MPS_MSImageConvolution_h */
