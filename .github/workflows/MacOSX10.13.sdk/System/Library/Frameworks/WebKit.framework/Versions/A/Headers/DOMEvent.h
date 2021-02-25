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

#import <WebKit/DOMObject.h>

@class NSString;
@protocol DOMEventTarget;

enum {
    DOM_NONE = 0,
    DOM_CAPTURING_PHASE = 1,
    DOM_AT_TARGET = 2,
    DOM_BUBBLING_PHASE = 3
} NS_ENUM_AVAILABLE_MAC(10_4);

NS_CLASS_AVAILABLE_MAC(10_4)
@interface DOMEvent : DOMObject
@property (readonly, copy) NSString *type;
@property (readonly, strong) id <DOMEventTarget> target;
@property (readonly, strong) id <DOMEventTarget> currentTarget;
@property (readonly) unsigned short eventPhase;
@property (readonly) BOOL bubbles;
@property (readonly) BOOL cancelable;
@property (readonly) DOMTimeStamp timeStamp;
@property (readonly, strong) id <DOMEventTarget> srcElement NS_AVAILABLE_MAC(10_6);
@property BOOL returnValue NS_AVAILABLE_MAC(10_6);
@property BOOL cancelBubble NS_AVAILABLE_MAC(10_6);

- (void)stopPropagation;
- (void)preventDefault;
- (void)initEvent:(NSString *)eventTypeArg canBubbleArg:(BOOL)canBubbleArg cancelableArg:(BOOL)cancelableArg NS_AVAILABLE_MAC(10_5);
@end

@interface DOMEvent (DOMEventDeprecated)
- (void)initEvent:(NSString *)eventTypeArg :(BOOL)canBubbleArg :(BOOL)cancelableArg NS_DEPRECATED_MAC(10_4, 10_5);
@end
