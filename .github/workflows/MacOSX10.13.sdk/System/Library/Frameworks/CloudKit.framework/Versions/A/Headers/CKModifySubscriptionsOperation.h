//
//  CKModifySubscriptionOperation.h
//  CloudKit
//
//  Copyright (c) 2014 Apple Inc. All rights reserved.
//

#import <CloudKit/CKDatabaseOperation.h>

@class CKSubscription;

NS_ASSUME_NONNULL_BEGIN
API_AVAILABLE(macos(10.10), ios(8.0)) __WATCHOS_PROHIBITED
@interface CKModifySubscriptionsOperation : CKDatabaseOperation

- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithSubscriptionsToSave:(nullable NSArray<CKSubscription *> *)subscriptionsToSave subscriptionIDsToDelete:(nullable NSArray<NSString *> *)subscriptionIDsToDelete;

@property (nonatomic, copy, nullable) NSArray<CKSubscription *> *subscriptionsToSave;
@property (nonatomic, copy, nullable) NSArray<NSString *> *subscriptionIDsToDelete;

/*  This block is called when the operation completes.
    The [NSOperation completionBlock] will also be called if both are set.
    If the error is CKErrorPartialFailure, the error's userInfo dictionary contains
    a dictionary of subscriptionIDs to errors keyed off of CKPartialErrorsByItemIDKey.
*/
@property (nonatomic, copy, nullable) void (^modifySubscriptionsCompletionBlock)(NSArray<CKSubscription *> * _Nullable savedSubscriptions, NSArray<NSString *> * _Nullable deletedSubscriptionIDs, NSError * _Nullable operationError);

@end
NS_ASSUME_NONNULL_END
