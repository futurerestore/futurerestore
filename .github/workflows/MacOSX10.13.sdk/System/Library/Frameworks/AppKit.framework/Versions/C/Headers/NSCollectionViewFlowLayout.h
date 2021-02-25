/*
    NSCollectionViewFlowLayout.h
    Application Kit
    Copyright (c) 2015-2017, Apple Inc.
    All rights reserved.
*/

#import <AppKit/NSCollectionView.h>
#import <AppKit/NSCollectionViewLayout.h>

typedef NS_ENUM(NSInteger, NSCollectionViewScrollDirection) {
    NSCollectionViewScrollDirectionVertical,
    NSCollectionViewScrollDirectionHorizontal
} NS_ENUM_AVAILABLE_MAC(10_11);

NS_ASSUME_NONNULL_BEGIN

APPKIT_EXTERN NSCollectionViewSupplementaryElementKind const NSCollectionElementKindSectionHeader NS_AVAILABLE_MAC(10_11);
APPKIT_EXTERN NSCollectionViewSupplementaryElementKind const NSCollectionElementKindSectionFooter NS_AVAILABLE_MAC(10_11);

NS_CLASS_AVAILABLE_MAC(10_11)
@interface NSCollectionViewFlowLayoutInvalidationContext : NSCollectionViewLayoutInvalidationContext
#ifndef __OBJC2__
{
@private
    struct {
        unsigned int invalidateDelegateMetrics:1;
        unsigned int invalidateAttributes:1;
    } _flowLayoutInvalidationFlags;
}
#endif

@property BOOL invalidateFlowLayoutDelegateMetrics; // if set to NO, flow layout will not requery the collection view delegate for size information etc.
@property BOOL invalidateFlowLayoutAttributes; // if set to NO, flow layout will keep all layout information, effectively not invalidating - useful for a subclass which invalidates only a piece of itself

@end

@protocol NSCollectionViewDelegateFlowLayout <NSCollectionViewDelegate>
@optional

- (NSSize)collectionView:(NSCollectionView *)collectionView layout:(NSCollectionViewLayout*)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath;
- (NSEdgeInsets)collectionView:(NSCollectionView *)collectionView layout:(NSCollectionViewLayout*)collectionViewLayout insetForSectionAtIndex:(NSInteger)section;
- (CGFloat)collectionView:(NSCollectionView *)collectionView layout:(NSCollectionViewLayout*)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section;
- (CGFloat)collectionView:(NSCollectionView *)collectionView layout:(NSCollectionViewLayout*)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section;
- (NSSize)collectionView:(NSCollectionView *)collectionView layout:(NSCollectionViewLayout*)collectionViewLayout referenceSizeForHeaderInSection:(NSInteger)section;
- (NSSize)collectionView:(NSCollectionView *)collectionView layout:(NSCollectionViewLayout*)collectionViewLayout referenceSizeForFooterInSection:(NSInteger)section;

@end

NS_CLASS_AVAILABLE_MAC(10_11)
@interface NSCollectionViewFlowLayout : NSCollectionViewLayout
#ifndef __OBJC2__
{
@package
    struct {
        unsigned int delegateSizeForItem:1;
        unsigned int delegateReferenceSizeForHeader:1;
        unsigned int delegateReferenceSizeForFooter:1;
        unsigned int delegateInsetForSection:1;
        unsigned int delegateInteritemSpacingForSection:1;
        unsigned int delegateLineSpacingForSection:1;
        unsigned int delegateAlignmentOptions:1;
        unsigned int layoutDataIsValid:1;
        unsigned int delegateInfoIsValid:1;
        unsigned int roundsToScreenScale:1;
        unsigned int sectionHeadersFloat:1;
        unsigned int sectionFootersFloat:1;
        unsigned int isSingleColumnOrRow:1;
        unsigned int collapsesSectionsToFirstItem:1;
    } _gridLayoutFlags;
    
    CGFloat _interitemSpacing;
    CGFloat _lineSpacing;
    
    CGSize _itemSize; // for the cases the delegate method is not implemented
    CGSize _estimatedItemSize;
    CGSize _headerReferenceSize;
    CGSize _footerReferenceSize;
    NSEdgeInsets _sectionInset;
    
    id _data;
    CGSize _currentLayoutSize;
    
    NSMutableDictionary* _insertedItemsAttributesDict;
    NSMutableDictionary* _insertedSectionHeadersAttributesDict;
    NSMutableDictionary* _insertedSectionFootersAttributesDict;
    NSMutableDictionary* _deletedItemsAttributesDict;
    NSMutableDictionary* _deletedSectionHeadersAttributesDict;
    NSMutableDictionary* _deletedSectionFootersAttributesDict;
    
    NSCollectionViewScrollDirection _scrollDirection;
    NSDictionary *_rowAlignmentsOptionsDictionary;
    
    CGPoint _contentOffsetAdjustment;
    CGSize _contentSizeAdjustment;
    
    NSMutableArray* _indexPathsToValidate;
}
#endif

@property CGFloat minimumLineSpacing;
@property CGFloat minimumInteritemSpacing;
@property NSSize itemSize;
@property NSSize estimatedItemSize; // defaults to NSZeroSize - setting a non-zero size enables cells that self-size via -preferredLayoutAttributesFittingAttributes:
@property NSCollectionViewScrollDirection scrollDirection; // default is NSCollectionViewScrollDirectionVertical
@property NSSize headerReferenceSize;
@property NSSize footerReferenceSize;
@property NSEdgeInsets sectionInset;

/* Set these properties to YES to get headers that pin to the top of the visible area and footers that pin to the bottom while scrolling.  Archived with the layout's other persistent properties.  Enabling this feature may affect the parenting of header and footer views.
*/
@property BOOL sectionHeadersPinToVisibleBounds NS_AVAILABLE_MAC(10_12);
@property BOOL sectionFootersPinToVisibleBounds NS_AVAILABLE_MAC(10_12);


#pragma mark *** Section Collapse ***

/* Returns YES if the specified section is currently collapsed; NO if not, or if there is no such section.  Defaults to NO.
*/
- (BOOL)sectionAtIndexIsCollapsed:(NSUInteger)sectionIndex NS_AVAILABLE_MAC(10_12);

/* Collapses the specified section to a single row, if it is not already collapsed.
*/
- (void)collapseSectionAtIndex:(NSUInteger)sectionIndex NS_AVAILABLE_MAC(10_12);

/* Un-collapses the specified section, if it is currently collapsed.
*/
- (void)expandSectionAtIndex:(NSUInteger)sectionIndex NS_AVAILABLE_MAC(10_12);

@end

NS_ASSUME_NONNULL_END
