/*
	NSPersistentDocument.h
	Application Kit
	Copyright (c) 2004-2017, Apple Inc.
	All rights reserved.
 */


#import <Foundation/NSDictionary.h>
#import <AppKit/NSDocument.h>

NS_ASSUME_NONNULL_BEGIN

@class NSManagedObjectModel, NSManagedObjectContext;

@interface NSPersistentDocument : NSDocument {
@private
    NSManagedObjectModel *_managedObjectModel;
    NSManagedObjectContext *_managedObjectContext;
    id _store;
    uintptr_t _pDocFlags;
    id _relatedRequestURLs;
    void *_reserved3 __unused;
    void *_reserved4 __unused;
}

// Persistent documents always have a managed context (and a persistent store coordinator through that context).
@property (nullable, strong) NSManagedObjectContext *managedObjectContext;

// By default the framework will create a merged model of all models found in the main bundle. Subclasses can return a specific model to use for creating persistent stores.
@property (nullable, readonly, strong) NSManagedObjectModel *managedObjectModel;


// Configures the persistent store coordinator with the appropriate stores. Subclasses can create the store to save to or load from (invoked from within the other NSDocument methods to read/write files), which gives developers the ability to load/save from/to different persistent store types (default type is XML).
- (BOOL)configurePersistentStoreCoordinatorForURL:(NSURL *)url ofType:(NSString *)fileType modelConfiguration:(nullable NSString *)configuration storeOptions:(nullable NSDictionary<NSString *, id> *)storeOptions error:(NSError **)error NS_AVAILABLE_MAC(10_5);


- (NSString *)persistentStoreTypeForFileType:(NSString *)fileType;  // Returns the name identifying the store type to save to for the given fileType. The default implementation of this method returns information derived from the application's Info.plist. If no store type information is in the Info.plist, the default implementation returns NSXMLStoreType. See NSPersistentStoreCoordinator.h for store type information.

- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName forSaveOperation:(NSSaveOperationType)saveOperation originalContentsURL:(nullable NSURL *)absoluteOriginalContentsURL error:(NSError **)error; // Overridden to save the document's managed objects referenced by the managed object context. If this is the first time the document is being saved, simply pass nil as the originalContentsURL.

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)error; // Overridden to load the document's managed objects through the managed object context.

- (BOOL)revertToContentsOfURL:(NSURL *)inAbsoluteURL ofType:(NSString *)inTypeName error:(NSError **)outError; //Overridden to clean up the managedObjectContext and controllers during a revert.

@end

@interface NSPersistentDocument (NSDeprecated)

// This method is deprecated. Please use -configurePersistentStoreCoordinatorForURL:ofType:modelConfiguration:storeOptions:error: instead
- (BOOL)configurePersistentStoreCoordinatorForURL:(null_unspecified NSURL *)url ofType:(null_unspecified NSString *)fileType error:(NSError **)error NS_DEPRECATED_MAC(10_4, 10_5);

@end

NS_ASSUME_NONNULL_END


