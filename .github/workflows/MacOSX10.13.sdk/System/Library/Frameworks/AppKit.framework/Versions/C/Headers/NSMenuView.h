/*
        NSMenuView.h
        Application Kit
        Copyright (c) 1997-2017, Apple Inc.
        All rights reserved.
*/

/* old-style menus are not available on 64-bit */
#if ! __LP64__

#import <AppKit/NSView.h>
#import <AppKit/AppKitDefines.h>
#import <AppKit/NSMenu.h>
#import <AppKit/NSMenuItem.h>
#import <AppKit/NSMenuItemCell.h>
@class NSScreen;

NS_AUTOMATED_REFCOUNT_WEAK_UNAVAILABLE
@interface NSMenuView : NSView {
    /*All instance variables are private*/
    @private
    NSMenu *_menu;
    NSMutableArray *_cells;
    CGFloat _stateImageWidth;
    CGFloat _imageAndTitleWidth;
    CGFloat _keyEquivalentWidth;
    CGFloat *_extents;
    unsigned _extentsCapacity;
    int _highlightedItemIndex;
    BOOL *_submenuPopupCancel;
    NSMenuView *_attachedMenuView;
    CGFloat _scrollArrowHeight;
    CGFloat _maxWinHeight;
    NSFont *_font;
    NSSize _minSize;
    CGFloat _horizontalEdgePad;
    void *trackingState;
    id _scrollingView;
    struct __mvFlags {
        unsigned int needsSizing:1;
        unsigned int releasingWindow:1;
        unsigned int isHorizontal:1;
        unsigned int disableSizing:1;
        unsigned int attachedSubmenuWhileMouseWasUp:1;
        unsigned int needsToCreateCells:1;
        unsigned int allowsTearOffs:1;
        unsigned int isTearOff:1;
        unsigned int keyEquivGlyphWidth:16;
        unsigned int disableResize:1;
        unsigned int savedHODState:1;
        unsigned int drawCenter:1;
        unsigned int RESERVED:5;
    } _mvFlags;
}

+ (CGFloat)menuBarHeight;

- (id)initWithFrame:(NSRect)frame;  // DI
- (id)initAsTearOff;

- (void)setMenu:(NSMenu *)menu;
- (NSMenu *)menu;

- (void)itemChanged:(NSNotification *)notification;
- (void)itemAdded:(NSNotification *)notification;
- (void)itemRemoved:(NSNotification *)notification;

- (void)update;

- (void)setHorizontal:(BOOL)flag;
- (BOOL)isHorizontal;

- (void)setFont:(NSFont *)font;
- (NSFont *)font;

- (NSRect)innerRect; // inside the bezel (if any)
- (NSRect)rectOfItemAtIndex:(NSInteger)index;
- (NSInteger)indexOfItemAtPoint:(NSPoint)point;
- (void)setNeedsDisplayForItemAtIndex:(NSInteger)index;

- (void)setHighlightedItemIndex:(NSInteger)index;
- (NSInteger)highlightedItemIndex;

- (CGFloat)stateImageOffset;
- (CGFloat)stateImageWidth;
- (CGFloat)imageAndTitleOffset;
- (CGFloat)imageAndTitleWidth;
- (CGFloat)keyEquivalentOffset;
- (CGFloat)keyEquivalentWidth;

- (void)setMenuItemCell:(NSMenuItemCell *)cell forItemAtIndex:(NSInteger)index;
- (NSMenuItemCell *)menuItemCellForItemAtIndex:(NSInteger)index;

- (NSMenuView *)attachedMenuView;

- (void)setNeedsSizing:(BOOL)flag;
- (BOOL)needsSizing;

- (void)sizeToFit;
- (NSMenu *)attachedMenu;
- (BOOL)isAttached;
- (BOOL)isTornOff;
- (NSPoint)locationForSubmenu:(NSMenu *)submenu;

- (void)setWindowFrameForAttachingToRect:(NSRect)screenRect onScreen:(NSScreen *)screen preferredEdge:(NSRectEdge)edge popUpSelectedItem:(NSInteger)selectedItemIndex;
- (void)detachSubmenu;
- (void)attachSubmenuForItemAtIndex:(NSInteger)index;

- (void)performActionWithHighlightingForItemAtIndex:(NSInteger)index;

- (BOOL)trackWithEvent:(NSEvent *)event;

- (CGFloat)horizontalEdgePadding;
- (void)setHorizontalEdgePadding:(CGFloat)pad;
    // How much empty pad should be left on either edge of the item.

@end

#endif
