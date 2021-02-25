//
//  VNTypes.h
//  Vision
//
//  Copyright © 2017 Apple Inc. All rights reserved.
//

#ifndef VNTYPES_H
#define VNTYPES_H

#import <Foundation/Foundation.h>

#import <Vision/VNDefines.h>


typedef float VNConfidence;

typedef float VNAspectRatio;

typedef float VNDegrees;

typedef NS_ENUM(NSUInteger, VNImageCropAndScaleOption)
{
    VNImageCropAndScaleOptionCenterCrop = 0,  // scale image maintaining aspect ratio to fit on the short side and crop centered on the long side
    VNImageCropAndScaleOptionScaleFit = 1,    // scale to size required by algorithm
    VNImageCropAndScaleOptionScaleFill
};

/*!
	@brief Barcode symbologies that are supported by the Vision framework.
	
	@discussion The actual set of barcode symbologies that can actually be recognized by a specific version of the Vision framework should be determined by using the VNRequestNameSupportedBarcodeSymbologies request.
*/
typedef NSString *VNBarcodeSymbology NS_STRING_ENUM;

VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyAztec API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyCODE39 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyCODE39Checksum API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyCODE39FullASCII API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyCODE39FullASCIIChecksum API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyCODE93 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyCODE93i API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyCODE128 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyDataMatrix API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyEAN8 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyEAN13 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyI2OF5 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyI2OF5Checksum API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyITF14 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyPDF417 API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyQR API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));
VN_EXPORT VNBarcodeSymbology const VNBarcodeSymbologyUPCE API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0));


#endif /* VNTYPES_H */
