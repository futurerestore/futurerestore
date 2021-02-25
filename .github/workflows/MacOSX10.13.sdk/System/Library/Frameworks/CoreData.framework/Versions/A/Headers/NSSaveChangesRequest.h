/*
    NSSaveChangesRequest.h
    Core Data
    Copyright (c) 2004-2017, Apple Inc.
    All rights reserved.
*/

#import <Foundation/NSArray.h>
#import <Foundation/NSSet.h>
#import <CoreData/NSPersistentStoreRequest.h>

NS_ASSUME_NONNULL_BEGIN

@class NSPersistentStoreRequest;
@class NSManagedObject;

API_AVAILABLE(macosx(10.7),ios(5.0))
@interface NSSaveChangesRequest : NSPersistentStoreRequest {
#if (!__OBJC2__)
@private
    NSSet *_insertedObjects;
    NSSet *_updatedObjects;
    NSSet *_deletedObjects;
    NSSet* _optimisticallyLockedObjects;
	uintptr_t _flags;
	void* _reserved1;
#endif
}

// Default initializer.
- (instancetype)initWithInsertedObjects:(nullable NSSet<NSManagedObject *> *)insertedObjects updatedObjects:(nullable NSSet<NSManagedObject *> *)updatedObjects deletedObjects:(nullable NSSet<NSManagedObject *> *)deletedObjects lockedObjects:(nullable NSSet<NSManagedObject *> *)lockedObjects;

// Objects that were inserted into the calling context.
@property (nullable, readonly, strong) NSSet<__kindof NSManagedObject *> *insertedObjects;
// Objects that were modified in the calling context.
@property (nullable, readonly, strong) NSSet<__kindof NSManagedObject *> *updatedObjects;
// Objects that were deleted from the calling context.
@property (nullable, readonly, strong) NSSet<__kindof NSManagedObject *> *deletedObjects;
// Objects that were flagged for optimistic locking on the calling context via detectConflictsForObject:.
@property (nullable, readonly, strong) NSSet<__kindof NSManagedObject *> *lockedObjects;

@end

NS_ASSUME_NONNULL_END
