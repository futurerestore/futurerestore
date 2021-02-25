//
//  MLDictionaryConstraint.h
//  CoreML
//
//  Copyright © 2017 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreML/MLFeatureType.h>

NS_ASSUME_NONNULL_BEGIN

/*!
 * MLDictionaryConstraint
 *
 * Constraint describing expected NSDictionary properties
 */
API_AVAILABLE(macos(10.13), ios(11.0), watchos(4.0), tvos(11.0))
@interface MLDictionaryConstraint : NSObject

/// Required key type, described as MLFeatureType
@property (readonly, nonatomic) MLFeatureType keyType;

@end

NS_ASSUME_NONNULL_END

