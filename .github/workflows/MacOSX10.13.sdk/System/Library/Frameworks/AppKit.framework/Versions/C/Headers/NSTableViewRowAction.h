/*
    NSTableViewRowAction.h
    Application Kit
    Copyright (c) 2014-2017, Apple Inc.
    All rights reserved.
*/

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class NSColor, NSButton, NSImage;

typedef NS_ENUM(NSInteger, NSTableViewRowActionStyle) {
    NSTableViewRowActionStyleRegular,
    NSTableViewRowActionStyleDestructive, // Destructive buttons will do a special animation when being deleted
} NS_ENUM_AVAILABLE_MAC(10_11);

/* An array of this class should be returned from the NSTableView delegate method tableView:rowActionsForRow:edge:.
 */
NS_CLASS_AVAILABLE_MAC(10_11)
@interface NSTableViewRowAction : NSObject <NSCopying> {
@private
    NSTableViewRowActionStyle _style;
    NSString *_title;
    void (^_handler)(NSTableViewRowAction *action, NSInteger row);
    NSColor *_backgroundColor;
#ifndef __OBJC2__
    unsigned int _flags:32;
#endif
    NSImage *_image;
}

+ (instancetype)rowActionWithStyle:(NSTableViewRowActionStyle)style title:(NSString *)title handler:(void (^)(NSTableViewRowAction *action, NSInteger row))handler;

@property (readonly) NSTableViewRowActionStyle style;
@property (copy) NSString *title;
@property (null_resettable, copy) NSColor *backgroundColor; // The default background color is dependent on style. Generally this is red for destructive actions, and blue for others.

/* Prefer using an image over text for the row action button
 */
@property (nullable, strong) NSImage *image API_AVAILABLE(macosx(10.12));

@end

NS_ASSUME_NONNULL_END
