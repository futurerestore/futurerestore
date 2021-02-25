//
//  CKOperation.h
//  CloudKit
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CloudKit/CKDefines.h>

@class CKContainer, CKOperationConfiguration, CKOperationGroup;

NS_ASSUME_NONNULL_BEGIN
API_AVAILABLE(macos(10.10), ios(8.0), watchos(3.0))
@interface CKOperation : NSOperation

- (instancetype)init NS_DESIGNATED_INITIALIZER;

/*! @abstract This defines per-operation configuration settings.
 *
 *  @discussion See the CKOperationConfiguration class description for info on how this configuration composes with CKOperationGroup.defaultConfiguration
 */
@property (nonatomic, copy, null_resettable) CKOperationConfiguration *configuration API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0), watchos(4.0));

/*! @abstract The group this operation is associated with
 */
@property (nonatomic, strong, nullable) CKOperationGroup *group API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0), watchos(4.0));

/*! @abstract This is an identifier unique to this CKOperation.
 *
 *  @discussion This value is chosen by the system, and will be unique to this instance of a CKOperation.  This identifier will be sent to Apple's servers, and can be used to identify any server-side logging associated with this operation.
 */
@property (nonatomic, readonly, copy) NSString *operationID API_AVAILABLE(macos(10.12), ios(9.3), tvos(9.2), watchos(3.0));

/*
 This callback is called after a long lived operation has begun running and is persisted. Once this callback is called the operation will continue running even if the current process exits.
 */
@property (nonatomic, strong, nullable) void (^longLivedOperationWasPersistedBlock)(void) API_AVAILABLE(macos(10.12), ios(9.3), tvos(9.2), watchos(3.0));

@end

/* An operation configuration is a set of properties that describes how your operation should behave.  All properties have a default value.  When determining what properties to apply to an operation, we consult the operation's configuration property, as well as the operation->group->defaultConfiguration property.  We combine them following these rules:
   Group Default Configuration Value | Operation Configuration Value |        Value Applied To Operation
  -----------------------------------+-------------------------------+-----------------------------------------
             default value           |         default value         |                  default value
             default value           |         explicit value        |       operation.configuration explicit value
             explicit value          |         default value         | operation.group.defaultConfiguration explicit value
             explicit value          |         explicit value        |       operation.configuration explicit value
 
   For example:
   CKOperationGroup -> defaultConfiguration -> allowsCellularAccess explicitly set to NO
 + CKOperation -> configuration -> allowsCellularAccess has default value of YES
 = disallow cellular access
 
   CKOperationGroup -> defaultConfiguration -> allowsCellularAccess explicitly set to NO
 + CKOperation -> configuration -> allowsCellularAccess explicitly set to YES
 = allow cellular access
 */
API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0), watchos(4.0))
@interface CKOperationConfiguration : NSObject

/* If no container is set, [CKContainer defaultContainer] is used */
@property (nonatomic, strong, nullable) CKContainer *container;

/*
 CKOperations behave differently depending on how you set qualityOfService:

 NSQualityOfServiceUserInteractive
    Network requests are non-discretionary
 NSQualityOfServiceUserInitiated
    Network requests are non-discretionary
 NSQualityOfServiceUtility
    The system marks your network requests as discretionary when the user is not using your app
 NSQualityOfServiceBackground
    Network requests are discretionary

 CKOperations have a default qualityOfService of NSQualityOfServiceUtility.

 */
@property (nonatomic, assign) NSQualityOfService qualityOfService;


/* Defaults to YES */
@property (nonatomic, assign) BOOL allowsCellularAccess;

/* 
   Long lived operations will continue running even if your process exits. If your process remains alive for the lifetime of the long lived operation its behavior is the same as a regular operation.

   Long lived operations can be fetched and replayed from the container via the fetchAllLongLivedOperations: and fetchLongLivedOperationsWithIDs: APIs.

   Long lived operations persist until their -[NSOperation completionBlock] returns or until the operation is cancelled. 
   Long lived operations may be garbage collected 24 hours after they finish running if no client has replayed them.
 
   The default value for longLived is NO. Changing the value of longLived on an already started operation or on an outstanding long lived operation fetched from CKContainer has no effect.
 */
@property (nonatomic, assign, getter=isLongLived) BOOL longLived;

/* 
   If non-zero, overrides the timeout interval for any network requests issued by this operation.
   See NSURLSessionConfiguration.timeoutIntervalForRequest 
   The default value is 60.
*/
@property (nonatomic, assign) NSTimeInterval timeoutIntervalForRequest;

/*
   If non-zero, overrides the timeout interval for any network resources retrieved by this operation.
   See NSURLSessionConfiguration.timeoutIntervalForResource
   The default value is -1.
 */
@property (nonatomic, assign) NSTimeInterval timeoutIntervalForResource;


@end

#pragma mark - Deprecated CKOperation

/* These deprecated properties now read and write from the CKOperation's configuration */
@interface CKOperation (CKOperationDeprecated)
@property (nonatomic, strong, nullable) CKContainer *container          API_DEPRECATED("Use CKOperationConfiguration", macos(10.10, 10.13), ios(8.0, 11.0), tvos(9.0, 11.0), watchos(3.0, 4.0));
@property (nonatomic, assign) BOOL allowsCellularAccess                 API_DEPRECATED("Use CKOperationConfiguration", macos(10.10, 10.13), ios(8.0, 11.0), tvos(9.0, 11.0), watchos(3.0, 4.0));
@property (nonatomic, assign, getter=isLongLived) BOOL longLived        API_DEPRECATED("Use CKOperationConfiguration", macos(10.12, 10.13), ios(9.3, 11.0), tvos(9.2, 11.0), watchos(3.0, 4.0));
@property (nonatomic, assign) NSTimeInterval timeoutIntervalForRequest  API_DEPRECATED("Use CKOperationConfiguration", macos(10.12, 10.13), ios(10.0, 11.0), tvos(10.0, 11.0), watchos(3.0, 4.0));
@property (nonatomic, assign) NSTimeInterval timeoutIntervalForResource API_DEPRECATED("Use CKOperationConfiguration", macos(10.12, 10.13), ios(10.0, 11.0), tvos(10.0, 11.0), watchos(3.0, 4.0));
@end
NS_ASSUME_NONNULL_END

