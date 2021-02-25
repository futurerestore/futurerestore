/*
 * Copyright (C) 2004-2016 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <WebKit/DOMDocument.h>

@class DOMHTMLCollection;
@class NSString;

NS_CLASS_AVAILABLE_MAC(10_4)
@interface DOMHTMLDocument : DOMDocument
@property (readonly, strong) DOMHTMLCollection *embeds NS_AVAILABLE_MAC(10_5);
@property (readonly, strong) DOMHTMLCollection *plugins NS_AVAILABLE_MAC(10_5);
@property (readonly, strong) DOMHTMLCollection *scripts NS_AVAILABLE_MAC(10_5);
@property (readonly) int width NS_AVAILABLE_MAC(10_5);
@property (readonly) int height NS_AVAILABLE_MAC(10_5);
@property (copy) NSString *dir NS_AVAILABLE_MAC(10_5);
@property (copy) NSString *designMode NS_AVAILABLE_MAC(10_5);
@property (readonly, copy) NSString *compatMode NS_AVAILABLE_MAC(10_6);
@property (copy) NSString *bgColor NS_AVAILABLE_MAC(10_5);
@property (copy) NSString *fgColor NS_AVAILABLE_MAC(10_5);
@property (copy) NSString *alinkColor NS_AVAILABLE_MAC(10_5);
@property (copy) NSString *linkColor NS_AVAILABLE_MAC(10_5);
@property (copy) NSString *vlinkColor NS_AVAILABLE_MAC(10_5);

- (void)open;
- (void)close;
- (void)write:(NSString *)text;
- (void)writeln:(NSString *)text;
- (void)clear NS_AVAILABLE_MAC(10_6);
- (void)captureEvents NS_AVAILABLE_MAC(10_5);
- (void)releaseEvents NS_AVAILABLE_MAC(10_5);
@end
