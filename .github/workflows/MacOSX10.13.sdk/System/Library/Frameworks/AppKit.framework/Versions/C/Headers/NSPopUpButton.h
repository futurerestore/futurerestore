/*
        NSPopUpButton.h
        Application Kit
        Copyright (c) 1997-2017, Apple Inc.
        All rights reserved.
*/

#import <Foundation/NSArray.h>
#import <AppKit/NSButton.h>
#import <AppKit/NSMenuItemCell.h>
#import <AppKit/NSMenuItem.h>

NS_ASSUME_NONNULL_BEGIN

@class NSMenu;

@interface NSPopUpButton : NSButton {
    /*All instance variables are private*/
    @private
    struct __pbFlags {
        unsigned int needsPullsDownFromTemplate:1;  // for decoding old nibs only!
        unsigned int RESERVED:31;
    } _pbFlags;
#if __LP64__
    id _popupReserved __unused;
#endif
}

- (instancetype)initWithFrame:(NSRect)buttonFrame pullsDown:(BOOL)flag;

// Overrides behavior of NSView.  This is the menu for the popup, not a context menu.  PopUpButtons do not have context menus.
@property (nullable, strong) NSMenu *menu;

// Behavior settings
@property BOOL pullsDown;
    
@property BOOL autoenablesItems;

@property NSRectEdge preferredEdge;
    // The preferred edge is used for pull down menus and for popups under severe screen position restrictions.  It indicates what edge of the cell the menu should pop out from.

// Adding and removing items
- (void)addItemWithTitle:(NSString *)title;
- (void)addItemsWithTitles:(NSArray<NSString *> *)itemTitles;
- (void)insertItemWithTitle:(NSString *)title atIndex:(NSInteger)index;

- (void)removeItemWithTitle:(NSString *)title;
- (void)removeItemAtIndex:(NSInteger)index;
- (void)removeAllItems;


// Accessing the items
@property (readonly, copy) NSArray<NSMenuItem *> *itemArray;
@property (readonly) NSInteger numberOfItems;

- (NSInteger)indexOfItem:(NSMenuItem *)item;
- (NSInteger)indexOfItemWithTitle:(NSString *)title;
- (NSInteger)indexOfItemWithTag:(NSInteger)tag;
- (NSInteger)indexOfItemWithRepresentedObject:(nullable id)obj;
- (NSInteger)indexOfItemWithTarget:(nullable id)target andAction:(nullable SEL)actionSelector;

- (nullable NSMenuItem *)itemAtIndex:(NSInteger)index;
- (nullable NSMenuItem *)itemWithTitle:(NSString *)title;
@property (nullable, readonly, strong) NSMenuItem *lastItem;


// Dealing with selection
- (void)selectItem:(nullable NSMenuItem *)item;
- (void)selectItemAtIndex:(NSInteger)index;
- (void)selectItemWithTitle:(NSString *)title;
- (BOOL)selectItemWithTag:(NSInteger)tag;
- (void)setTitle:(NSString *)string;

@property (nullable, readonly, strong) NSMenuItem *selectedItem;
@property (readonly) NSInteger indexOfSelectedItem;
@property (readonly) NSInteger selectedTag;
- (void)synchronizeTitleAndSelectedItem;

// Title conveniences
- (NSString *)itemTitleAtIndex:(NSInteger)index;
@property (readonly, copy) NSArray<NSString *> *itemTitles;
@property (nullable, readonly, copy) NSString *titleOfSelectedItem;

@end

/* Notifications */
APPKIT_EXTERN NSNotificationName NSPopUpButtonWillPopUpNotification;

NS_ASSUME_NONNULL_END
