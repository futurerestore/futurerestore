/*
	AppKitErrors.h
	Application Kit
	Copyright (c) 2004-2017, Apple Inc.
	All rights reserved.
*/

#import <Foundation/NSObject.h>


/* NSError codes for AppKit added errors in NSCocoaErrorDomain.  Foundation error codes can be found in <Foundation/FoundationErrors.h>; CoreData error codes in <CoreData/CoreDataErrors.h>.
*/
enum {
    NSTextReadInapplicableDocumentTypeError = 65806,		// NSAttributedString parsing error
    NSTextWriteInapplicableDocumentTypeError = 66062,		// NSAttributedString generating error

    // Inclusive error range definitions, for checking future error codes
    NSTextReadWriteErrorMinimum = 65792,
    NSTextReadWriteErrorMaximum = 66303,

    // Font error codes
    NSFontAssetDownloadError NS_ENUM_AVAILABLE_MAC(10_13) = 66304,  // Generic code when font asset download error occurs.
    NSFontErrorMinimum NS_ENUM_AVAILABLE_MAC(10_13) = 66304,
    NSFontErrorMaximum NS_ENUM_AVAILABLE_MAC(10_13) = 66335,
    
    // Service error codes
    NSServiceApplicationNotFoundError = 66560,			// The service provider could not be found.
    NSServiceApplicationLaunchFailedError = 66561,		// The service providing application could not be launched.  This will typically contain an underlying error with an LS error code (check MacErrors.h for their meanings).
    NSServiceRequestTimedOutError = 66562,			// The service providing application did not open its service listening port in time, or the app didn't respond to the request in time; see the Console log to figure out which (the errors are typically reported the same way to the user).
    NSServiceInvalidPasteboardDataError = 66563,		// The service providing app did not return a pasteboard with any of the promised types, or we couldn't write the data from the pasteboard to the object receiving the returned data.
    NSServiceMalformedServiceDictionaryError = 66564,		// The service dictionary did not contain the necessary keys.  Messages will typically be logged to the console giving more details.
    NSServiceMiscellaneousError = 66800,			// Other errors, representing programmatic mistakes in the service consuming application.  These show a generic error message to the user.
    
    // Inclusive service error range, for checking future error codes
    NSServiceErrorMinimum = 66560,
    NSServiceErrorMaximum = 66817,
    
    
    // Sharing Service Error    
    NSSharingServiceNotConfiguredError = 67072,                // The service is not configured in the Preferences
    
    // Inclusive service error range, for checking future error codes
    NSSharingServiceErrorMinimum = 67072,
    NSSharingServiceErrorMaximum = 67327
};



