//
//  CKContainer.h
//  CloudKit
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CloudKit/CKDefines.h>
#import <CloudKit/CKDatabase.h>

NS_ASSUME_NONNULL_BEGIN

@class CKDatabase, CKOperation, CKRecordID, CKUserIdentity, CKShareParticipant, CKDiscoveredUserInfo, CKShare, CKShareMetadata;

// This constant represents the current user's ID for zone ID
CK_EXTERN NSString * const CKCurrentUserDefaultName API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));

CK_EXTERN NSString * const CKOwnerDefaultName API_DEPRECATED_WITH_REPLACEMENT("CKCurrentUserDefaultName", macos(10.10, 10.12), ios(8.0, 10.0), tvos(9.0, 10.0), watchos(3.0, 3.0));

/* A CKContainer, and its CKDatabases, are the main entry points into the CloudKit framework.

 Several methods in CloudKit accept completion handlers to indicate when they're completed.
 All CKOperation subclasses include progress and completion blocks to report significant events in their lifecycles.
 Each of these handlers and blocks is invoked on a non-main serial queue.  The receiver is responsible for handling the message on a different queue or thread if it is required. */

API_AVAILABLE(macos(10.10), ios(8.0), watchos(3.0))
@interface CKContainer : NSObject

- (instancetype)init NS_UNAVAILABLE;

/* These calls return a CKContainer initialized with a containerIdentifier of
  [NSString stringWithFormat:@"iCloud.%@", applicationIdentifier], where application identifier
   is the calling process' application-identifier (com.apple.application-identifier on OS X) entitlement.
   If the application is in production mode (aka,
   com.apple.developer.icloud-container-environment
   is set to Production in your entitlements plist, and you have no override in
   com.apple.developer.icloud-container-development-container-identifiers
   ), then the production environment is used. */
+ (CKContainer *)defaultContainer;
+ (CKContainer *)containerWithIdentifier:(NSString *)containerIdentifier;

@property (nonatomic, readonly, nullable) NSString *containerIdentifier;

- (void)addOperation:(CKOperation *)operation;

@end

@interface CKContainer (Database)

/* Database properties:
 Records in a public database
 - By default are world readable, owner writable.
 - Can be locked down by Roles, a process done in the Developer Portal, a web interface.  Roles are not present in the client API.
 - Are visible to the application developer via the Developer Portal.
 - Do not contribute to the owner's iCloud account storage quota.
 Records in a private database
 - By default are only owner readable and owner writable.
 - Are not visible to the application developer via the Developer Portal.
 - Are counted towards the owner's iCloud account storage quota.
 Records in a shared database
 - Are available to share participants based on the permissions of the enclosing CKShare
 - Are not visible to the application developer via the Developer Portal.
 - Are counted towards the originating owner's iCloud account storage quota.
 */
@property (nonatomic, readonly) CKDatabase *privateCloudDatabase;
@property (nonatomic, readonly) CKDatabase *publicCloudDatabase;
@property (nonatomic, readonly) CKDatabase *sharedCloudDatabase API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));

/* Convenience method, will return a database that's pointer-equal to one of the above properties */
- (CKDatabase *)databaseWithDatabaseScope:(CKDatabaseScope)databaseScope API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));

@end

typedef NS_ENUM(NSInteger, CKAccountStatus) {
    /* An error occurred when getting the account status, consult the corresponding NSError */
    CKAccountStatusCouldNotDetermine                   = 0,
    /* The iCloud account credentials are available for this application */
    CKAccountStatusAvailable                           = 1,
    /* Parental Controls / Device Management has denied access to iCloud account credentials */
    CKAccountStatusRestricted                          = 2,
    /* No iCloud account is logged in on this device */
    CKAccountStatusNoAccount                           = 3,
} API_AVAILABLE(macos(10.10), ios(8.0), watchos(3.0));

/* This local notification is posted when there has been any change to the logged in 
   iCloud account. On receipt, an updated account status should be obtained by calling
   accountStatusWithCompletionHandler:
 */
CK_EXTERN NSString * const CKAccountChangedNotification API_AVAILABLE(macos(10.11), ios(9.0), watchos(3.0));

@interface CKContainer (AccountStatus)

- (void)accountStatusWithCompletionHandler:(void (^)(CKAccountStatus accountStatus, NSError * _Nullable error))completionHandler;

@end

typedef NS_OPTIONS(NSUInteger, CKApplicationPermissions) {
    /* Allows the user's record in CloudKit to be discoverable via the user's email address */
    CKApplicationPermissionUserDiscoverability         = 1 << 0,
} API_AVAILABLE(macos(10.10), ios(8.0), watchos(3.0));

typedef NS_ENUM(NSInteger, CKApplicationPermissionStatus) {
    /* The user has not made a decision for this application permission. */
    CKApplicationPermissionStatusInitialState          = 0,
    /* An error occurred when getting or setting the application permission status, consult the corresponding NSError */
    CKApplicationPermissionStatusCouldNotComplete      = 1,
    /* The user has denied this application permission */
    CKApplicationPermissionStatusDenied                = 2,
    /* The user has granted this application permission */
    CKApplicationPermissionStatusGranted               = 3,
} API_AVAILABLE(macos(10.10), ios(8.0), watchos(3.0));

typedef void(^CKApplicationPermissionBlock)(CKApplicationPermissionStatus applicationPermissionStatus, NSError * _Nullable error);

@interface CKContainer (ApplicationPermission)

- (void)statusForApplicationPermission:(CKApplicationPermissions)applicationPermission completionHandler:(CKApplicationPermissionBlock)completionHandler;
- (void)requestApplicationPermission:(CKApplicationPermissions)applicationPermission completionHandler:(CKApplicationPermissionBlock)completionHandler;

@end

@interface CKContainer (UserRecords)

/* If there is no iCloud account configured, or if access is restricted, a CKErrorNotAuthenticated error will be returned. 
   This work is treated as having NSQualityOfServiceUserInitiated quality of service.
 */
- (void)fetchUserRecordIDWithCompletionHandler:(void (^)(CKRecordID * _Nullable recordID, NSError * _Nullable error))completionHandler;

/* This fetches all user records that match an entry in the user's address book.
 CKDiscoverAllContactsOperation and CKDiscoverUserIdentityOperation are the more configurable,
 CKOperation-based alternatives to these methods */
- (void)discoverAllIdentitiesWithCompletionHandler:(void (^)(NSArray<CKUserIdentity *> * _Nullable userIdentities, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), watchos(3.0)) API_UNAVAILABLE(tvos);
- (void)discoverUserIdentityWithEmailAddress:(NSString *)email completionHandler:(void (^)(CKUserIdentity * _Nullable userInfo, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));
- (void)discoverUserIdentityWithPhoneNumber:(NSString *)phoneNumber completionHandler:(void (^)(CKUserIdentity * _Nullable userInfo, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));
- (void)discoverUserIdentityWithUserRecordID:(CKRecordID *)userRecordID completionHandler:(void (^)(CKUserIdentity * _Nullable userInfo, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));

- (void)discoverAllContactUserInfosWithCompletionHandler:(void (^)(NSArray<CKDiscoveredUserInfo *> * _Nullable userInfos, NSError * _Nullable error))completionHandler API_DEPRECATED("Use -[CKContainer discoverAllIdentitiesWithCompletionHandler:]", macos(10.10, 10.12), ios(8.0, 10.0), watchos(3.0, 3.0));
- (void)discoverUserInfoWithEmailAddress:(NSString *)email completionHandler:(void (^)(CKDiscoveredUserInfo * _Nullable userInfo, NSError * _Nullable error))completionHandler API_DEPRECATED("Use -[CKContainer discoverUserIdentityWithEmailAddress:completionHandler:]", macos(10.10, 10.12), ios(8.0, 10.0), tvos(9.0, 10.0), watchos(3.0, 3.0));
- (void)discoverUserInfoWithUserRecordID:(CKRecordID *)userRecordID completionHandler:(void (^)(CKDiscoveredUserInfo * _Nullable userInfo, NSError * _Nullable error))completionHandler API_DEPRECATED("Use -[CKContainer discoverUserIdentityWithUserRecordID:completionHandler:]", macos(10.10, 10.12), ios(8.0, 10.0), tvos(9.0, 10.0), watchos(3.0, 3.0));

@end

@interface CKContainer (Sharing)

/* Fetches share participants matching the provided info .
 CKFetchShareParticipantsOperation is the more configurable, CKOperation-based alternative to these methods. */
- (void)fetchShareParticipantWithEmailAddress:(NSString *)emailAddress completionHandler:(void (^)(CKShareParticipant * _Nullable shareParticipant, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));
- (void)fetchShareParticipantWithPhoneNumber:(NSString *)phoneNumber completionHandler:(void (^)(CKShareParticipant * _Nullable shareParticipant, NSError *_Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));
- (void)fetchShareParticipantWithUserRecordID:(CKRecordID *)userRecordID completionHandler:(void (^)(CKShareParticipant *_Nullable shareParticipant, NSError *_Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));

- (void)fetchShareMetadataWithURL:(NSURL *)url completionHandler:(void (^)(CKShareMetadata *_Nullable metadata, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));
- (void)acceptShareMetadata:(CKShareMetadata *)metadata completionHandler:(void (^)(CKShare *_Nullable acceptedShare, NSError *_Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(10.0), tvos(10.0), watchos(3.0));

@end

@interface CKContainer (CKLongLivedOperations)
/* 
 Long lived CKOperations returned by this call must be started on an operation queue.
 Remember to set the callback blocks before starting the operation.
 If an operation has already completed against the server, and is subsequently resumed, that operation will replay all of
  its callbacks from the start of the operation, but the request will not be re-sent to the server.
 If a long lived operation is cancelled or finishes completely it is no longer returned by these calls.
 */
- (void)fetchAllLongLivedOperationIDsWithCompletionHandler:(void (^)(NSArray<NSString *> * _Nullable outstandingOperationIDs, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(9.3), tvos(9.2), watchos(3.0));
- (void)fetchLongLivedOperationWithID:(NSString *)operationID completionHandler:(void (^)(CKOperation * _Nullable outstandingOperation, NSError * _Nullable error))completionHandler API_AVAILABLE(macos(10.12), ios(9.3), tvos(9.2), watchos(3.0));
@end
NS_ASSUME_NONNULL_END
