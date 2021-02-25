/*
	NSFontPanel.h
	Application Kit
	Copyright (c) 1994-2017, Apple Inc.
	All rights reserved.
*/

#import <AppKit/NSPanel.h>

NS_ASSUME_NONNULL_BEGIN

@class NSFontPanel, NSMutableDictionary, NSFontManager, NSMutableArray, NSTableView, NSFontDescriptor, NSFont;


typedef NS_OPTIONS(NSUInteger, NSFontPanelModeMask) {
    NSFontPanelModeMaskFace = 1 << 0,
    NSFontPanelModeMaskSize = 1 << 1,
    NSFontPanelModeMaskCollection = 1 << 2,
    NSFontPanelModeMaskUnderlineEffect = 1<<8,
    NSFontPanelModeMaskStrikethroughEffect = 1<<9,
    NSFontPanelModeMaskTextColorEffect = 1<< 10,
    NSFontPanelModeMaskDocumentColorEffect = 1<<11,
    NSFontPanelModeMaskShadowEffect = 1<<12,
    NSFontPanelModeMaskAllEffects = 0XFFF00,
    NSFontPanelModesMaskStandardModes = 0xFFFF,
    NSFontPanelModesMaskAllModes = 0xFFFFFFFF
} NS_ENUM_AVAILABLE_MAC(10_13);

@interface NSObject (NSFontPanelValidationAdditions)
- (NSFontPanelModeMask)validModesForFontPanel:(NSFontPanel *)fontPanel;
@end


NS_AUTOMATED_REFCOUNT_WEAK_UNAVAILABLE
@interface NSFontPanel : NSPanel {
    /*All instance variables are private*/
    NSFontManager *_manager;
    NSArray *_collectionNames;
    id _selection;
    void *_carbonNotification; 
    id _targetObject;

    id			_familyList;
    id                  _faceList;
    id                  _sizeList;
    id                  _mainCollectionList;
    id			_sizeField;
    id                  _sizeSlider;
    id                  _sizeSliderBox;
    id			_preview;
    id			_previewContainer;
    id                  _mainSplitView;
    id			_mmCollectionList;	
    id			_mmFamilyList;
    id                  _mmFaceList;
    id                  _mmSizeList;
    id			_fullSizeHeightConstraint;
    id                  _searchField;
    id			_fixedListButton;
    id		        _sliderButton; 
    id		        _accessoryView; 
    struct __fpFlags {
	unsigned int	    setFontChange:1;
	unsigned int	    setFontAttributeChange:1;
	unsigned int	    _delRespFamily:1;
	unsigned int	    _delRespFace:1;
	unsigned int	    _delRespSize:1;
	unsigned int	    _delRespColl:1;
	unsigned int	    _collectionDisabled:1; // used by validate font panel modes. 
	unsigned int	    _sizeDisabled:1; // used by validate font panel modes. 
	unsigned int	    _faceDisabled:1; // used by validate font panel modes. 
        unsigned int        showEffects:1;
        unsigned int        _uiMode:8;
        unsigned int        _miniMode:1;
        unsigned int _reserved:13;
    } _fpFlags;

    id			_regularModeBox; 
    id			_miniModeBox; 
    id			_sizeBox;
    id			_collectionLabel; 
    id			_sizeLabel; 
    id                  _faceDivider;
    id                  _familyLabel;
    id			_sizeStyleButton; 
    id			_newSizeField; 
    id			_editSizeList; 
    id			_editSizeListBox; 
    id			_editSizeSliderBox; 
    id			_editSizeSliderMaxField; 
    id			_editSizeSliderMinField; 
    id			_sizeEditWindow; 
    id			_availableSizes; 

    id _addCollectionButton;
    id _removeCollectionButton;

    CGFloat _fontPanelPreviewHeight; 
    id _typographyPanel;
    id _actionButton;
    id _fontEffectsBox;
    int _sizeStyle;

    id _fontPanelToolbar;
    id _fontPanelContentView;

#if !__LP64__
    id _fpUnused[70];
#endif /* !__LP64__ */
}

@property (class, readonly, strong) NSFontPanel *sharedFontPanel;
@property (class, readonly) BOOL sharedFontPanelExists;

@property (nullable, strong) NSView *accessoryView;
- (void)setPanelFont:(NSFont *)fontObj isMultiple:(BOOL)flag;
- (NSFont *)panelConvertFont:(NSFont *)fontObj;
@property BOOL worksWhenModal;
@property (getter=isEnabled) BOOL enabled;

/* This method triggers a re-load to the default state, so that the delegate will be called, and have an opportunity to scrutinize the default list of fonts to be displayed in the panel.
*/
- (void) reloadDefaultFontFamilies;

@end


// The following enum items are deprecated. Use NSFontPanelModeMask instead
enum {
    NSFontPanelFaceModeMask = 1 << 0,
    NSFontPanelSizeModeMask = 1 << 1,
    NSFontPanelCollectionModeMask = 1 << 2,
    NSFontPanelUnderlineEffectModeMask = 1<<8,
    NSFontPanelStrikethroughEffectModeMask = 1<<9,
    NSFontPanelTextColorEffectModeMask = 1<< 10,
    NSFontPanelDocumentColorEffectModeMask = 1<<11,
    NSFontPanelShadowEffectModeMask = 1<<12,
    NSFontPanelAllEffectsModeMask = 0XFFF00,
    NSFontPanelStandardModesMask = 0xFFFF,
    NSFontPanelAllModesMask = 0xFFFFFFFF
};

/* Tags of views in the FontPanel
 THESE ARE ALL OBSOLETE and should not be used.
 */
enum {
    NSFPPreviewButton			= 131,
    NSFPRevertButton			= 130,
    NSFPSetButton			= 132,
    NSFPPreviewField			= 128,
    NSFPSizeField			= 129,
    NSFPSizeTitle			= 133,
    NSFPCurrentField			= 134
} NS_ENUM_DEPRECATED_MAC(10_0, 10_0);

NS_ASSUME_NONNULL_END
