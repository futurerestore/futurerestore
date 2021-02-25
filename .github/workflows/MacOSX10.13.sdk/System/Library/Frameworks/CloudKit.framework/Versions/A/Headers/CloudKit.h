//
//  CloudKit.h
//  CloudKit
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <CloudKit/CKDefines.h>

#import <CloudKit/CKContainer.h>
#import <CloudKit/CKAsset.h>
#import <CloudKit/CKDatabase.h>
#import <CloudKit/CKError.h>
#import <CloudKit/CKLocationSortDescriptor.h>
#import <CloudKit/CKNotification.h>
#import <CloudKit/CKQuery.h>
#import <CloudKit/CKRecordZone.h>
#import <CloudKit/CKRecord.h>
#import <CloudKit/CKRecordID.h>
#import <CloudKit/CKRecordZoneID.h>
#import <CloudKit/CKReference.h>
#import <CloudKit/CKServerChangeToken.h>
#import <CloudKit/CKShare.h>
#import <CloudKit/CKShareMetadata.h>
#import <CloudKit/CKShareParticipant.h>
#import <CloudKit/CKSubscription.h>
#import <CloudKit/CKDiscoveredUserInfo.h>
#import <CloudKit/CKUserIdentity.h>
#import <CloudKit/CKUserIdentityLookupInfo.h>

#import <CloudKit/CKOperationGroup.h>
#import <CloudKit/CKOperation.h>
#import <CloudKit/CKDatabaseOperation.h>
#import <CloudKit/CKModifyRecordsOperation.h>
#import <CloudKit/CKFetchRecordsOperation.h>
#import <CloudKit/CKFetchRecordChangesOperation.h>
#import <CloudKit/CKFetchRecordZoneChangesOperation.h>
#import <CloudKit/CKQueryOperation.h>
#import <CloudKit/CKModifyBadgeOperation.h>
#import <CloudKit/CKFetchNotificationChangesOperation.h>
#import <CloudKit/CKMarkNotificationsReadOperation.h>
#import <CloudKit/CKFetchSubscriptionsOperation.h>
#import <CloudKit/CKModifySubscriptionsOperation.h>
#import <CloudKit/CKModifyRecordZonesOperation.h>
#import <CloudKit/CKFetchRecordZonesOperation.h>
#import <CloudKit/CKFetchWebAuthTokenOperation.h>
#import <CloudKit/CKDiscoverUserInfosOperation.h>
#import <CloudKit/CKDiscoverAllContactsOperation.h>
#import <CloudKit/CKDiscoverUserIdentitiesOperation.h>
#import <CloudKit/CKDiscoverAllUserIdentitiesOperation.h>
#import <CloudKit/CKFetchShareParticipantsOperation.h>
#import <CloudKit/CKAcceptSharesOperation.h>
#import <CloudKit/CKFetchShareMetadataOperation.h>
#import <CloudKit/CKFetchDatabaseChangesOperation.h>
