//
//  CKFetchRecordChangesOperation.h
//  CloudKit
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <CloudKit/CKDatabaseOperation.h>
#import <CloudKit/CKServerChangeToken.h>

@class CKRecord, CKRecordID, CKRecordZoneID;

/* Use CKFetchRecordZoneChangesOperation instead of this class.
 Any serverChangeTokens saved from a CKFetchRecordChangesOperation are usable as a serverRecordZoneChangeToken in CKFetchRecordZoneChangesOperation */

NS_ASSUME_NONNULL_BEGIN
API_DEPRECATED_WITH_REPLACEMENT("CKFetchRecordZoneChangesOperation", macos(10.10, 10.12), ios(8.0, 10.0), tvos(9.0, 10.0), watchos(3.0, 3.0))
@interface CKFetchRecordChangesOperation : CKDatabaseOperation

/* This operation will fetch records changes in the given record zone.

   If a change anchor from a previous CKFetchRecordChangesOperation is passed in, only the records that have changed
    since that anchor will be fetched.

   If this is your first fetch or if you wish to re-fetch all records, pass nil for the change anchor.

   Change anchors are opaque tokens and clients should not infer any behavior based on their content. */
- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithRecordZoneID:(CKRecordZoneID *)recordZoneID previousServerChangeToken:(nullable CKServerChangeToken *)previousServerChangeToken;

@property (nonatomic, copy, nullable) CKRecordZoneID *recordZoneID;
@property (nonatomic, copy, nullable) CKServerChangeToken *previousServerChangeToken;

@property (nonatomic, assign) NSUInteger resultsLimit;

/* Declares which user-defined keys should be fetched and added to the resulting CKRecords.  If nil, declares the entire record should be downloaded. If set to an empty array, declares that no user fields should be downloaded.  Defaults to nil. */
@property (nonatomic, copy, nullable) NSArray<NSString *> *desiredKeys;

@property (nonatomic, copy, nullable) void (^recordChangedBlock)(CKRecord *record);
@property (nonatomic, copy, nullable) void (^recordWithIDWasDeletedBlock)(CKRecordID *recordID);

/* Will be set before fetchRecordChangesCompletionBlock is called. If moreComing is true then the server wasn't able to return all the changes in this response.
    Another CKFetchRecordChangesOperation operation should be run with the updated serverChangeToken token from this operation. */
@property (nonatomic, readonly) BOOL moreComing;

/* Clients are responsible for saving the change token at the end of the operation and passing it in to the next call to CKFetchRecordChangesOperation.
   Note that a fetch can fail partway. If that happens, an updated change token may be returned in the completion
    block so that already fetched records don't need to be re-downloaded on a subsequent operation.
   The clientChangeTokenData from the most recent CKModifyRecordsOperation is also returned, or nil if none was provided.
   If the server returns a CKErrorChangeTokenExpired error, the previousServerChangeToken value was too old and the client should toss its local cache and 
    re-fetch the changes in this record zone starting with a nil previousServerChangeToken. */
@property (nonatomic, copy, nullable) void (^fetchRecordChangesCompletionBlock)(CKServerChangeToken * _Nullable serverChangeToken, NSData * _Nullable clientChangeTokenData, NSError * _Nullable operationError);

@end
NS_ASSUME_NONNULL_END
