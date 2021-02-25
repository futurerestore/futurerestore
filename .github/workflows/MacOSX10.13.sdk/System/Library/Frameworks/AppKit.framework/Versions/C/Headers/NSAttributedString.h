/*
        NSAttributedString.h
        Copyright (c) 1994-2017, Apple Inc.
        All rights reserved.
 
        This file defines Application Kit extensions to NSAttributedString and NSMutableAttributedString.
*/

#import <Foundation/NSAttributedString.h>
#import <AppKit/NSFontManager.h>
#import <AppKit/NSText.h>
#import <AppKit/NSPasteboard.h>
#import <AppKit/AppKitDefines.h>
@class NSTextBlock, NSTextTable, NSTextList;
@class NSFileWrapper;
@class NSURL;

NS_ASSUME_NONNULL_BEGIN
/************************ Attributes ************************/

// Predefined character attributes for text. If the key is not present in the dictionary, it indicates the default value described below.
APPKIT_EXTERN NSAttributedStringKey  NSFontAttributeName NS_AVAILABLE(10_0, 6_0);                // NSFont, default Helvetica(Neue) 12
APPKIT_EXTERN NSAttributedStringKey  NSParagraphStyleAttributeName NS_AVAILABLE(10_0, 6_0);      // NSParagraphStyle, default defaultParagraphStyle
APPKIT_EXTERN NSAttributedStringKey  NSForegroundColorAttributeName NS_AVAILABLE(10_0, 6_0);     // NSColor, default blackColor
APPKIT_EXTERN NSAttributedStringKey  NSBackgroundColorAttributeName NS_AVAILABLE(10_0, 6_0);     // NSColor, default nil: no background
APPKIT_EXTERN NSAttributedStringKey  NSLigatureAttributeName NS_AVAILABLE(10_0, 6_0);            // NSNumber containing integer, default 1: default ligatures, 0: no ligatures
APPKIT_EXTERN NSAttributedStringKey  NSKernAttributeName NS_AVAILABLE(10_0, 6_0);                // NSNumber containing floating point value, in points; amount to modify default kerning. 0 means kerning is disabled.
APPKIT_EXTERN NSAttributedStringKey  NSStrikethroughStyleAttributeName NS_AVAILABLE(10_0, 6_0);  // NSNumber containing integer, default 0: no strikethrough
APPKIT_EXTERN NSAttributedStringKey  NSUnderlineStyleAttributeName NS_AVAILABLE(10_0, 6_0);      // NSNumber containing integer, default 0: no underline
APPKIT_EXTERN NSAttributedStringKey  NSStrokeColorAttributeName NS_AVAILABLE(10_0, 6_0);         // NSColor, default nil: same as foreground color
APPKIT_EXTERN NSAttributedStringKey  NSStrokeWidthAttributeName NS_AVAILABLE(10_0, 6_0);         // NSNumber containing floating point value, in percent of font point size, default 0: no stroke; positive for stroke alone, negative for stroke and fill (a typical value for outlined text would be 3.0)
APPKIT_EXTERN NSAttributedStringKey  NSShadowAttributeName NS_AVAILABLE(10_0, 6_0);              // NSShadow, default nil: no shadow
APPKIT_EXTERN NSAttributedStringKey const NSTextEffectAttributeName NS_AVAILABLE(10_10, 7_0);          // NSString, default nil: no text effect

APPKIT_EXTERN NSAttributedStringKey  NSAttachmentAttributeName NS_AVAILABLE(10_0, 7_0);          // NSTextAttachment, default nil
APPKIT_EXTERN NSAttributedStringKey  NSLinkAttributeName NS_AVAILABLE(10_0, 7_0);                // NSURL (preferred) or NSString
APPKIT_EXTERN NSAttributedStringKey  NSBaselineOffsetAttributeName NS_AVAILABLE(10_0, 7_0);      // NSNumber containing floating point value, in points; offset from baseline, default 0
APPKIT_EXTERN NSAttributedStringKey  NSUnderlineColorAttributeName NS_AVAILABLE(10_0, 7_0);      // NSColor, default nil: same as foreground color
APPKIT_EXTERN NSAttributedStringKey  NSStrikethroughColorAttributeName NS_AVAILABLE(10_0, 7_0);  // NSColor, default nil: same as foreground color
APPKIT_EXTERN NSAttributedStringKey  NSObliquenessAttributeName NS_AVAILABLE(10_0, 7_0);         // NSNumber containing floating point value; skew to be applied to glyphs, default 0: no skew
APPKIT_EXTERN NSAttributedStringKey  NSExpansionAttributeName NS_AVAILABLE(10_0, 7_0);           // NSNumber containing floating point value; log of expansion factor to be applied to glyphs, default 0: no expansion

APPKIT_EXTERN NSAttributedStringKey  NSWritingDirectionAttributeName NS_AVAILABLE(10_6, 7_0);    // NSArray of NSNumbers representing the nested levels of writing direction overrides as defined by Unicode LRE, RLE, LRO, and RLO characters.  The control characters can be obtained by masking NSWritingDirection and NSWritingDirectionFormatType values.  LRE: NSWritingDirectionLeftToRight|NSWritingDirectionEmbedding, RLE: NSWritingDirectionRightToLeft|NSWritingDirectionEmbedding, LRO: NSWritingDirectionLeftToRight|NSWritingDirectionOverride, RLO: NSWritingDirectionRightToLeft|NSWritingDirectionOverride,

APPKIT_EXTERN NSAttributedStringKey  NSVerticalGlyphFormAttributeName NS_AVAILABLE(10_7, 6_0);   // An NSNumber containing an integer value.  0 means horizontal text.  1 indicates vertical text.  If not specified, it could follow higher-level vertical orientation settings.  Currently on iOS, it's always horizontal.  The behavior for any other value is undefined.

APPKIT_EXTERN NSAttributedStringKey NSCursorAttributeName; // NSCursor, default IBeamCursor
APPKIT_EXTERN NSAttributedStringKey NSToolTipAttributeName; // NSString, default nil: no tooltip

APPKIT_EXTERN NSAttributedStringKey NSMarkedClauseSegmentAttributeName; // Clause segment index NSNumber (intValue). This attribute is used in marked text indicating clause segments

APPKIT_EXTERN NSAttributedStringKey NSTextAlternativesAttributeName NS_AVAILABLE_MAC(10_8); // An NSTextAlternatives object.  Used primarily as a temporary attribute, with primaryString equal to the substring for the range to which it is attached, and alternativeStrings representing alternatives for that string that may be presented to the user.

APPKIT_EXTERN NSAttributedStringKey NSSpellingStateAttributeName;  // NSSpellingStateAttributeName is used and recognized only as a temporary attribute (see NSLayoutManager.h).  It indicates that spelling and/or grammar indicators should be shown for the specified characters, default 0: no spelling or grammar indicat


APPKIT_EXTERN NSAttributedStringKey NSSuperscriptAttributeName; // NSNumber containing integer, default 0
APPKIT_EXTERN NSAttributedStringKey NSGlyphInfoAttributeName;  // NSGlyphInfo specifying glyph for the associated attribute range


/************************ Attribute values ************************/
// This defines currently supported values for NSUnderlineStyleAttributeName and NSStrikethroughStyleAttributeName. NSUnderlineStyle*, NSUnderlinePattern*, and NSUnderlineByWord are or'ed together to produce an underline style.
typedef NS_ENUM(NSInteger, NSUnderlineStyle) {
    NSUnderlineStyleNone                                    = 0x00,
    NSUnderlineStyleSingle                                  = 0x01,
    NSUnderlineStyleThick NS_ENUM_AVAILABLE(10_0, 7_0)      = 0x02,
    NSUnderlineStyleDouble NS_ENUM_AVAILABLE(10_0, 7_0)     = 0x09,

    NSUnderlinePatternSolid NS_ENUM_AVAILABLE(10_0, 7_0)      = 0x0000,
    NSUnderlinePatternDot NS_ENUM_AVAILABLE(10_0, 7_0)        = 0x0100,
    NSUnderlinePatternDash NS_ENUM_AVAILABLE(10_0, 7_0)       = 0x0200,
    NSUnderlinePatternDashDot NS_ENUM_AVAILABLE(10_0, 7_0)    = 0x0300,
    NSUnderlinePatternDashDotDot NS_ENUM_AVAILABLE(10_0, 7_0) = 0x0400,

    NSUnderlineByWord NS_ENUM_AVAILABLE(10_0, 7_0)            = 0x8000
} NS_ENUM_AVAILABLE(10_0, 6_0);

// NSWritingDirectionFormatType values used by NSWritingDirectionAttributeName. It is or'ed with either NSWritingDirectionLeftToRight or NSWritingDirectionRightToLeft. Can specify the formatting controls defined by Unicode Bidirectional Algorithm.
typedef NS_ENUM(NSInteger, NSWritingDirectionFormatType) {
    NSWritingDirectionEmbedding     = (0 << 1),
    NSWritingDirectionOverride      = (1 << 1)
} NS_ENUM_AVAILABLE(10_11, 9_0);

// NSTextEffectAttributeName values
typedef NSString * NSTextEffectStyle NS_STRING_ENUM;
APPKIT_EXTERN NSTextEffectStyle const NSTextEffectLetterpressStyle NS_AVAILABLE(10_10, 7_0);

// Flag values supported for NSSpellingStateAttributeName as of Mac OS X version 10.5.  Prior to 10.5, any non-zero value caused the spelling indicator to be shown.
typedef NS_ENUM(NSInteger, NSSpellingState) {
    NSSpellingStateSpellingFlag NS_ENUM_AVAILABLE_MAC(10_5) = (1 << 0),
    NSSpellingStateGrammarFlag NS_ENUM_AVAILABLE_MAC(10_5)  = (1 << 1)
};

/************************ Attribute fixing ************************/

@interface NSMutableAttributedString (NSAttributedStringAttributeFixing)
// This method fixes attribute inconsistencies inside range.  It ensures NSFontAttributeName covers the characters, NSParagraphStyleAttributeName is only changing at paragraph boundaries, and NSTextAttachmentAttributeName is assigned to NSAttachmentCharacter.  NSTextStorage automatically invokes this method via -ensureAttributesAreFixedInRange:.
- (void)fixAttributesInRange:(NSRange)range NS_AVAILABLE(10_0, 7_0);

- (void)fixFontAttributeInRange:(NSRange)range;
- (void)fixParagraphStyleAttributeInRange:(NSRange)range;
- (void)fixAttachmentAttributeInRange:(NSRange)range;
@end


/************************ Document formats ************************/

typedef NSString * NSAttributedStringDocumentType NS_EXTENSIBLE_STRING_ENUM;

// Supported document types for the NSDocumentTypeDocumentAttribute key in the document attributes dictionary.
APPKIT_EXTERN NSAttributedStringDocumentType  NSPlainTextDocumentType NS_AVAILABLE(10_0, 7_0);
APPKIT_EXTERN NSAttributedStringDocumentType  NSRTFTextDocumentType  NS_AVAILABLE(10_0, 7_0);
APPKIT_EXTERN NSAttributedStringDocumentType  NSRTFDTextDocumentType NS_AVAILABLE(10_0, 7_0);
APPKIT_EXTERN NSAttributedStringDocumentType  NSHTMLTextDocumentType  NS_AVAILABLE(10_0, 7_0);
APPKIT_EXTERN NSAttributedStringDocumentType NSMacSimpleTextDocumentType;
APPKIT_EXTERN NSAttributedStringDocumentType NSDocFormatTextDocumentType;
APPKIT_EXTERN NSAttributedStringDocumentType NSWordMLTextDocumentType;
APPKIT_EXTERN NSAttributedStringDocumentType NSWebArchiveTextDocumentType;
APPKIT_EXTERN NSAttributedStringDocumentType NSOfficeOpenXMLTextDocumentType NS_AVAILABLE_MAC(10_5);
APPKIT_EXTERN NSAttributedStringDocumentType NSOpenDocumentTextDocumentType NS_AVAILABLE_MAC(10_5);

typedef NSString * NSTextLayoutSectionKey NS_STRING_ENUM;

// Keys for NSLayoutOrientationSectionsAttribute.
APPKIT_EXTERN NSTextLayoutSectionKey  NSTextLayoutSectionOrientation NS_AVAILABLE(10_7, 7_0); // NSNumber containing NSTextLayoutOrientation value. default: NSTextLayoutOrientationHorizontal
APPKIT_EXTERN NSTextLayoutSectionKey  NSTextLayoutSectionRange NS_AVAILABLE(10_7, 7_0); // NSValue containing NSRange representing a character range. default: a range covering the whole document

typedef NSString * NSAttributedStringDocumentAttributeKey NS_EXTENSIBLE_STRING_ENUM;

// Keys for options and document attributes dictionaries.  They are in and out document properties used by both read/write methods.

APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSDocumentTypeDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"DocumentType", one of the document types declared above.  For reader methods, this key in options can specify the document type for interpreting the contents.  Upon return, the document attributes can contain this key for indicating the actual format used to read the contents.  For write methods, this key specifies the format for generating the data.

APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSConvertedDocumentAttribute;  // @"Converted", NSNumber containing integer; if missing, or 0, the file was originally in the format specified by document type; if negative, the file was originally in the format specified by document type, but the conversion to NSAttributedString may have been lossy; if 1 or more, it was converted to the specified type by a filter service
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSCocoaVersionDocumentAttribute;  // @"CocoaRTFVersion", NSNumber containing integer.  Stores the version of Cocoa the file was created with.  NSNumber containing float.  Absence of this value indicates file not labelled as being created by Cocoa or its predecessors.  Values less than 100 are pre-Mac OS X; 100 is Mac OS X 10.0 and 10.1; 102 is Mac OS X 10.2 and 10.3; values greater than 102 correspond to values of NSAppKitVersionNumber on 10.4 and later systems.
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSFileTypeDocumentAttribute NS_AVAILABLE_MAC(10_6);  // NSString indicating which document type was used to interpret the document, specified as a UTI; for reading, this is available along with NSDocumentTypeDocumentAttribute, but for writing the two are mutually exclusive

APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSTitleDocumentAttribute;  // NSString containing document title
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSCompanyDocumentAttribute;  // NSString containing company or organization name
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSCopyrightDocumentAttribute;  // NSString containing document copyright info
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSSubjectDocumentAttribute;  // NSString containing subject of document
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSAuthorDocumentAttribute;  // NSString containing author name (not necessarily same as "last editor," see below)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSKeywordsDocumentAttribute;  // NSArray of NSString, containing keywords
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSCommentDocumentAttribute;  // NSString containing document comments
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSEditorDocumentAttribute;  // NSString containing name of person who last edited the document
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSCreationTimeDocumentAttribute;  // NSDate containing the creation date of the document; note that this is not the file system creation date of the file, but of the document, as stored in the document
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSModificationTimeDocumentAttribute;  // NSDate containing the modification date of the document contents
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSManagerDocumentAttribute NS_AVAILABLE_MAC(10_5);  // NSString containing name of the author's manager
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSCategoryDocumentAttribute NS_AVAILABLE_MAC(10_6);  // NSString containing the document category

// NSPlainTextDocumentType document attributes
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSCharacterEncodingDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"CharacterEncoding", NSNumber containing integer specifying NSStringEncoding for the file; default for plain text is the default encoding.  This key in options can specify the string encoding for reading the data.  Upon return, the document attributes can contain the actual encoding used.  For writing methods, this value is used for generating the plain text data.
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSDefaultAttributesDocumentAttribute NS_AVAILABLE(10_11, 7_0);  // @"DefaultAttributes", NSDictionary containing attributes to be applied to plain files.  Used by reader methods.  This key in options can specify the default attributes applied to the entire document contents.  The document attributes can contain this key indicating the actual attributes used.


// NSRTFTextDocumentType and NSRTFDTextDocumentType document attributes
// Document dimension
// They are document attributes used by read/write methods.
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSPaperSizeDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"PaperSize", NSValue containing NSSize (in points)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSLeftMarginDocumentAttribute;  // @"LeftMargin", NSNumber containing floating point value (in points)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSRightMarginDocumentAttribute;  // @"RightMargin", NSNumber containing floating point value (in points)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSTopMarginDocumentAttribute;  // @"TopMargin", NSNumber containing floating point value (in points)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSBottomMarginDocumentAttribute;  // @"BottomMargin", NSNumber containing floating point value (in points)

APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSViewSizeDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"ViewSize", NSValue containing NSSize (in points)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSViewZoomDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"ViewZoom", NSNumber containing floating point value (100 == 100% zoom)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSViewModeDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"ViewMode", NSNumber containing integer; 0 = normal; 1 = page layout

// Document settings
// They are document attributes used by read/write methods.
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSReadOnlyDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"ReadOnly", NSNumber containing integer; if missing, or 0 or negative, not readonly; 1 or more, readonly. Note that this has nothing to do with the file system protection on the file, but instead, on how the file should be displayed to the user
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSBackgroundColorDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"BackgroundColor", NSColor, representing the document-wide page background color
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSHyphenationFactorDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"HyphenationFactor", NSNumber containing floating point value (0=off, 1=full hyphenation)
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSDefaultTabIntervalDocumentAttribute NS_AVAILABLE(10_0, 7_0);  // @"DefaultTabInterval", NSNumber containing floating point value, representing the document-wide default tab stop interval, in points
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey  NSTextLayoutSectionsAttribute NS_AVAILABLE(10_7, 7_0);  // NSArray of dictionaries.  Each dictionary describing a layout orientation section.  The dictionary can have two attributes: NSTextLayoutSectionOrientation and NSTextLayoutSectionRange.  When there is a gap between sections, it's assumed to have NSTextLayoutOrientationHorizontal.

// NSHTMLTextDocumentType document attributes
// Additional document attributes added in 10.4, for HTML writing only.  These provide control over the form of generated HTML.  NSExcludedElementsDocumentAttribute allows control over the tags used.  The recognized values in the NSExcludedElementsDocumentAttribute array are (case-insensitive) HTML tags, plus DOCTYPE (representing a doctype declaration) and XML (representing an XML declaration).  By default, if this attribute is not present, the excluded elements will be those deprecated in HTML 4 (APPLET, BASEFONT, CENTER, DIR, FONT, ISINDEX, MENU, S, STRIKE, and U) plus XML.  If XML is on the list, HTML forms will be used; if XML is not on the list, XHTML forms will be used where there is a distinction.  Either NSCharacterEncodingDocumentAttribute or NSTextEncodingNameDocumentAttribute may be used to control the encoding used for generated HTML; character entities will be used for characters not representable in the specified encoding.  Finally, NSPrefixSpacesDocumentAttribute allows some control over formatting.
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSExcludedElementsDocumentAttribute;  // for HTML writing only; NSArray containing NSStrings, representing HTML elements not to be used in generated HTML
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSTextEncodingNameDocumentAttribute;  // for HTML writing only; NSString containing the name, IANA or otherwise, of a text encoding to be used; mutually exclusive with NSCharacterEncodingDocumentAttribute
APPKIT_EXTERN NSAttributedStringDocumentAttributeKey NSPrefixSpacesDocumentAttribute;  // for HTML writing only; NSNumber containing integer, default 0, representing the number of spaces per level by which to indent certain nested HTML elements

// The following are keys for various options that can be specified in the options dictionaries for the text import APIs below.  Except for NSTextSizeMultiplierDocumentOption and NSFileTypeDocumentOption, the values have been recognized for some time, but the actual identifier shown below was added in 10.4. If you want your app to run on earlier systems, you need to continue using the actual string value that these identifiers represent. The actual string value is specified in the comments below.

typedef NSString * NSAttributedStringDocumentReadingOptionKey NS_EXTENSIBLE_STRING_ENUM;

APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSDocumentTypeDocumentOption;  // @"DocumentType", NSString indicating a document type to be forced when loading the document, specified as one of the NSDocumentTypeDocumentAttribute constants listed above
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSDefaultAttributesDocumentOption;  // @"DefaultAttributes", for plain text only; NSDictionary containing attributes to be applied to plain files
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSCharacterEncodingDocumentOption;  // @"CharacterEncoding", for plain text and HTML; NSNumber containing integer specifying NSStringEncoding to be used to interpret the file

APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSTextEncodingNameDocumentOption;  // @"TextEncodingName", for HTML only; NSString containing a name, IANA or otherwise, specifying an encoding to be used to interpret the file; mutually exclusive with NSCharacterEncodingDocumentOption
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSBaseURLDocumentOption;  // @"BaseURL", for HTML only; NSURL containing a URL to be treated as the base URL for the document
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSTimeoutDocumentOption;  // @"Timeout", for HTML only; NSNumber containing floating point value; time in seconds to wait for a document to finish loading; if not present or not positive, a default timeout will be used
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSWebPreferencesDocumentOption;  // @"WebPreferences", for HTML only; WebPreferences; specifies a WebPreferences object describing a set of preferences; if not present, a default set of preferences will be used
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSWebResourceLoadDelegateDocumentOption;  // @"WebResourceLoadDelegate", for HTML only; NSObject; specifies an object to serve as the WebResourceLoadDelegate; if not present, a default delegate will be used that will permit the loading of subsidiary resources but will not respond to authentication challenges
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSTextSizeMultiplierDocumentOption;  // for HTML only; NSNumber containing floating point value, default 1.0; specifies a scale factor for font sizes, corresponding to WebView's textSizeMultiplier
APPKIT_EXTERN NSAttributedStringDocumentReadingOptionKey NSFileTypeDocumentOption NS_AVAILABLE_MAC(10_6);  // NSString indicating a document type to be forced when loading the document, specified as a UTI string; mutually exclusive with NSDocumentTypeDocumentOption

// In Mac OS X 10.4 and later, WebKit is always used for HTML documents, and all of the above options are recognized.  In Mac OS X 10.3, there is an additional options key, @"UseWebKit" (NSNumber containing integer; if present and positive, specifies that WebKit-based HTML importing is to be used).  In Mac OS X 10.3, the Timeout, WebPreferences, and WebResourceLoadDelegate options are recognized only when WebKit-based HTML importing is used.


@interface NSAttributedString (NSAttributedStringDocumentFormats)
// Methods initializing the receiver contents with an external document data.  options specify document attributes for interpreting the document contents.  NSDocumentTypeDocumentAttribute, NSCharacterEncodingDocumentAttribute, and NSDefaultAttributesDocumentAttribute are supported options key.  When they are not specified, these methods will examine the data and do their best to detect the appropriate attributes.  If dict is non-NULL, it will return a dictionary with various document-wide attributes accessible via NS...DocumentAttribute keys.
- (nullable instancetype)initWithURL:(NSURL *)url options:(NSDictionary<NSAttributedStringDocumentReadingOptionKey, id> *)options documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict error:(NSError **)error NS_AVAILABLE(10_4, 9_0);
- (nullable instancetype)initWithData:(NSData *)data options:(NSDictionary<NSAttributedStringDocumentReadingOptionKey, id> *)options documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict error:(NSError **)error NS_AVAILABLE(10_0, 7_0);

// Generates an NSData object for the receiver contents in range.  It requires a document attributes dict specifying at least the NSDocumentTypeDocumentAttribute to determine the format to be written.
- (nullable NSData *)dataFromRange:(NSRange)range documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> *)dict error:(NSError **)error NS_AVAILABLE(10_0, 7_0);

// Returns an NSFileWrapper object for the receiver contents in range.  It requires a document attributes dict specifying at least the NSDocumentTypeDocumentAttribute to determine the format to be written.  The method returns a directory file wrapper for those document types represented by a file package such as NSRTFDTextDocumentType; otherwise, it returns a regular-file file wrapper.
- (nullable NSFileWrapper *)fileWrapperFromRange:(NSRange)range documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> *)dict error:(NSError **)error NS_AVAILABLE(10_0, 7_0);

- (nullable instancetype)initWithRTF:(NSData *)data documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict;
- (nullable instancetype)initWithRTFD:(NSData *)data documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict;
- (nullable instancetype)initWithHTML:(NSData *)data documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict;
- (nullable instancetype)initWithHTML:(NSData *)data baseURL:(NSURL *)base documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict;
- (nullable instancetype)initWithDocFormat:(NSData *)data documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict;
- (nullable instancetype)initWithHTML:(NSData *)data options:(NSDictionary<NSAttributedStringDocumentReadingOptionKey, id> *)options documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict;
- (nullable instancetype)initWithRTFDFileWrapper:(NSFileWrapper *)wrapper documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict;
- (nullable NSData *)RTFFromRange:(NSRange)range documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> *)dict;
- (nullable NSData *)RTFDFromRange:(NSRange)range documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> *)dict;
- (nullable NSFileWrapper *)RTFDFileWrapperFromRange:(NSRange)range documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> *)dict;
- (nullable NSData *)docFormatFromRange:(NSRange)range documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> *)dict;
@end

@interface NSMutableAttributedString (NSMutableAttributedStringDocumentFormats)
// Methods replacing the receiver contents with an external document data.  options specify document attributes for interpreting the document contents.  NSDocumentTypeDocumentAttribute, NSCharacterEncodingDocumentAttribute, and NSDefaultAttributesDocumentAttribute are supported options key.  When they are not specified, these methods will examine the data and do their best to detect the appropriate attributes.  If dict is non-NULL, it will return a dictionary with various document-wide attributes accessible via NS...DocumentAttribute keys.
- (BOOL)readFromURL:(NSURL *)url options:(NSDictionary<NSAttributedStringDocumentReadingOptionKey, id> *)opts documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict error:(NSError **)error  API_AVAILABLE(macosx(10.5), ios(9.0), watchos(2.0), tvos(9.0));
- (BOOL)readFromData:(NSData *)data options:(NSDictionary<NSAttributedStringDocumentReadingOptionKey, id> *)opts documentAttributes:(NSDictionary<NSAttributedStringDocumentAttributeKey, id> * __nullable * __nullable)dict error:(NSError **)error NS_AVAILABLE(10_0, 7_0);
@end


/************************ Misc methods ************************/
@interface NSAttributedString (NSAttributedStringKitAdditions)
// Attributes which should be copied/pasted with "copy font".
- (NSDictionary<NSAttributedStringKey, id> *)fontAttributesInRange:(NSRange)range;

// Attributes which should be copied/pasted with "copy ruler".
- (NSDictionary<NSAttributedStringKey, id> *)rulerAttributesInRange:(NSRange)range;

// Returns YES if the receiver contains a property configured (NSAttachmentAttributeName with NSAttachmentCharacter) in range
- (BOOL)containsAttachmentsInRange:(NSRange)range NS_AVAILABLE(10_11, 9_0);

// Returns NSNotFound if no line break location found in the specified range; otherwise returns the index of the first character that should go on the NEXT line.
- (NSUInteger)lineBreakBeforeIndex:(NSUInteger)location withinRange:(NSRange)aRange;
- (NSUInteger)lineBreakByHyphenatingBeforeIndex:(NSUInteger)location withinRange:(NSRange)aRange;

- (NSRange)doubleClickAtIndex:(NSUInteger)location;
- (NSUInteger)nextWordFromIndex:(NSUInteger)location forward:(BOOL)isForward;


// Convenience methods for calculating the range of an individual text block, range of an entire table, range of a list, and the index within a list.
- (NSRange)rangeOfTextBlock:(NSTextBlock *)block atIndex:(NSUInteger)location;
- (NSRange)rangeOfTextTable:(NSTextTable *)table atIndex:(NSUInteger)location;
- (NSRange)rangeOfTextList:(NSTextList *)list atIndex:(NSUInteger)location;
- (NSInteger)itemNumberInTextList:(NSTextList *)list atIndex:(NSUInteger)location;
@end

@interface NSAttributedString (NSAttributedStringPasteboardAdditions)  <NSPasteboardReading, NSPasteboardWriting>
// Methods to determine what types can be loaded as NSAttributedStrings.
@property(class, readonly, copy) NSArray<NSString *> *textTypes NS_AVAILABLE_MAC(10_5);
@property(class, readonly, copy) NSArray<NSString *> *textUnfilteredTypes NS_AVAILABLE_MAC(10_5);
@end

@interface NSMutableAttributedString (NSMutableAttributedStringKitAdditions)
- (void)superscriptRange:(NSRange)range;
- (void)subscriptRange:(NSRange)range;
- (void)unscriptRange:(NSRange)range;
- (void)applyFontTraits:(NSFontTraitMask)traitMask range:(NSRange)range;
- (void)setAlignment:(NSTextAlignment)alignment range:(NSRange)range;
- (void)setBaseWritingDirection:(NSWritingDirection)writingDirection range:(NSRange)range;
@end

/************************ Deprecated ************************/
APPKIT_EXTERN NSAttributedStringKey NSCharacterShapeAttributeName NS_DEPRECATED_MAC(10_0, 10_11, "This attribute is bound to a specific implementation of ATS feature and not generically supported by wide range of fonts. The majority of characters accessed through this API are now encoded in the Unicode standard. Use the CTFont feature API for fine control over character shape choices.");
APPKIT_EXTERN NSAttributedStringKey NSUsesScreenFontsDocumentAttribute NS_DEPRECATED_MAC(10_8, 10_11);


enum {
    NSNoUnderlineStyle NS_ENUM_DEPRECATED_MAC(10_0, 10_9, "Use NSUnderlineStyleNone instead") = 0,
    NSSingleUnderlineStyle NS_ENUM_DEPRECATED_MAC(10_0, 10_9, "Use NSUnderlineStyleSingle instead")
};

APPKIT_EXTERN NSUInteger NSUnderlineStrikethroughMask NS_DEPRECATED_MAC(10_0, 10_9, "Use NSStrikethroughStyleAttributeName instead");
APPKIT_EXTERN NSUInteger NSUnderlineByWordMask NS_DEPRECATED_MAC(10_0, 10_11, "Use NSUnderlineByWord instead");

@interface NSAttributedString(NSDeprecatedKitAdditions)
// This property is soft deprecated starting with OS X 10.11. It will be officially deprecated in a future release. Use -containsAttachmentsInRange: instead
@property (readonly) BOOL containsAttachments;

+ (NSArray *)textFileTypes NS_DEPRECATED_MAC(10_1, 10_5);
+ (NSArray *)textPasteboardTypes NS_DEPRECATED_MAC(10_1, 10_5);
+ (NSArray *)textUnfilteredFileTypes NS_DEPRECATED_MAC(10_1, 10_5);
+ (NSArray *)textUnfilteredPasteboardTypes NS_DEPRECATED_MAC(10_1, 10_5);

- (nullable instancetype)initWithURL:(NSURL *)url documentAttributes:(NSDictionary* __nullable * __nullable)dict NS_DEPRECATED_MAC(10_0, 10_11, "Use -initWithURL:options:documentAttributes:error: instead");
- (nullable instancetype)initWithPath:(NSString *)path documentAttributes:(NSDictionary* __nullable * __nullable)dict NS_DEPRECATED_MAC(10_0, 10_11, "Use -initWithURL:options:documentAttributes:error: instead");

- (nullable NSURL *)URLAtIndex:(NSUInteger)location effectiveRange:(NSRangePointer)effectiveRange NS_DEPRECATED_MAC(10_5, 10_11, "Use NSDataDetector instead");
@end

@interface NSMutableAttributedString (NSDeprecatedKitAdditions)
- (BOOL)readFromURL:(NSURL *)url options:(NSDictionary *)options documentAttributes:(NSDictionary* __nullable * __nullable)dict NS_DEPRECATED_MAC(10_0, 10_11, "Use -readFromURL:options:documentAttributes:error: instead");
- (BOOL)readFromData:(NSData *)data options:(NSDictionary *)options documentAttributes:(NSDictionary* __nullable * __nullable)dict NS_DEPRECATED_MAC(10_0, 10_11, "Use -readFromData:options:documentAttributes:error: instead");
@end
NS_ASSUME_NONNULL_END
