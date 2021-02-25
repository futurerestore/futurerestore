/*
    NSPropertyDescription.h
    Core Data
    Copyright (c) 2004-2017, Apple Inc.
    All rights reserved.
*/

#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>

NS_ASSUME_NONNULL_BEGIN

@class NSEntityDescription;
@class NSData;
@class NSPredicate;

// Properties describe values within a managed object. There are different types of properties, each of them represented by a subclass which encapsulates the specific property behavior.
API_AVAILABLE(macosx(10.4),ios(3.0))
@interface NSPropertyDescription : NSObject <NSCoding, NSCopying> {
#if (!__OBJC2__)
@private
	NSString *_versionHashModifier;
	id _underlyingProperty2;
	NSData *_versionHash;
    NSEntityDescription *_entity;
    NSString *_name;
    NSArray *_validationPredicates;
    NSArray *_validationWarnings;
    __strong void *_extraIvars;
    NSMutableDictionary *_userInfo;
    struct __propertyDescriptionFlags {
        unsigned int _isReadOnly:1;
        unsigned int _isTransient:1;
        unsigned int _isOptional:1;
        unsigned int _isIndexed:1;
        unsigned int _skipValidation:1;
        unsigned int _isIndexedBySpotlight:1;
        unsigned int _isStoredInExternalRecord:1;
		unsigned int _extraIvarsAreInDataBlob:1;
        unsigned int _isOrdered:1;
        
        unsigned int _hasMaxValueInExtraIvars:1;
        unsigned int _hasMinValueInExtraIvars:1;
        unsigned int _storeBinaryDataExternally:1;
        unsigned int _preserveValueOnDelete:1;
        unsigned int _reservedAttributeFlagOne:1;

        unsigned int _reservedPropertyDescription:18;
    } _propertyDescriptionFlags;    
	long _entitysReferenceIDForProperty;
#endif
}

@property (nonatomic, readonly, assign) NSEntityDescription *entity;

@property (nonatomic, copy) NSString *name;

// The optional flag specifies whether a property's value can be nil or not (before an object can be persisted).
@property (getter=isOptional) BOOL optional;

// The transient flag specifies whether a property's value is persisted or ignored when an object is persisted - transient properties are still managed for undo/redo, validation, etc.
@property (getter=isTransient) BOOL transient;

// Instead of individual methods to set/get parameters like length, min and max values, formats, etc., there is a list of predicates evaluated against the managed objects and corresponding error messages (which can be localized).
@property (readonly, strong) NSArray<NSPredicate *> *validationPredicates;
@property (readonly, strong) NSArray *validationWarnings;
- (void)setValidationPredicates:(nullable NSArray<NSPredicate *> *)validationPredicates withValidationWarnings:(nullable NSArray<NSString *> *)validationWarnings;

@property (nullable, nonatomic, strong) NSDictionary *userInfo;

/* Returns a boolean value indicating if the property is important for searching.  NSPersistentStores can optionally utilize this information upon store creation for operations like defining indexes.
*/
@property (getter=isIndexed) BOOL indexed API_DEPRECATED( "Use NSEntityDescription.indexes instead", macosx(10.5,10.13),ios(3.0,11.0),tvos(10.0, 11.0),watchos(3.0, 4.0));

/* Returns the version hash for the property.  The version hash is used to uniquely identify a property based on its configuration.  The version hash uses only values which affect the persistence of data and the user-defined versionHashModifier value.  (The values which affect persistence are the name of the property, the flags for isOptional, isTransient, and isReadOnly).  This value is stored as part of the version information in the metadata for stores, as well as a definition of a property involved in an NSPropertyMapping.
*/
@property (readonly, copy) NSData *versionHash API_AVAILABLE(macosx(10.5),ios(3.0));

/* Returns/sets the version hash modifier for the property.  This value is included in the version hash for the property, allowing developers to mark/denote a property as being a different "version" than another, even if all of the values which affects persistence are equal.  (Such a difference is important in cases where the design of a property is unchanged, but the format or content of data has changed.)
*/
@property (nullable, copy) NSString *versionHashModifier API_AVAILABLE(macosx(10.5),ios(3.0));

/* Returns a boolean value indicating if the property should be indexed by Spotlight.
*/
@property (getter=isIndexedBySpotlight) BOOL indexedBySpotlight API_AVAILABLE(macosx(10.6),ios(3.0));

/* Returns a boolean value indicating if the property data should be written out to the external record file.
*/
@property (getter=isStoredInExternalRecord) BOOL storedInExternalRecord API_DEPRECATED("Spotlight integration is deprecated. Use CoreSpotlight integration instead.", macosx(10.6,10.13),ios(3.0,11.0));

@property (nullable, copy) NSString *renamingIdentifier API_AVAILABLE(macosx(10.6),ios(3.0));

@end

NS_ASSUME_NONNULL_END
