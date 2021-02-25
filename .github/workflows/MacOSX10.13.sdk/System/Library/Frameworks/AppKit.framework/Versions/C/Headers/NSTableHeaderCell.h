/*
        NSTableHeaderCell.h
        Application Kit
        Copyright (c) 1995-2017, Apple Inc.
        All rights reserved.
*/
#import <AppKit/NSTextFieldCell.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSTableHeaderCell : NSTextFieldCell {

}

/* Draws a sorting indicator given a cellFrame. If priority is 0, this is the primary sort indicator. If ascending is YES, a "^" indicator will be drawn.  Override this routine to customimze the sorting UI.
*/
- (void)drawSortIndicatorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView ascending:(BOOL)ascending priority:(NSInteger)priority;

/* Returns the location to display the sorting indicator given the cellFrame.
*/
- (NSRect)sortIndicatorRectForBounds:(NSRect)rect;

@end

NS_ASSUME_NONNULL_END
