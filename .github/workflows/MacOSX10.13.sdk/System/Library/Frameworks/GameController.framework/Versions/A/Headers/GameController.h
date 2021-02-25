//
//  GameController.h
//  GameController
//
//  Copyright (c) 2012 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIViewController.h>
#else
#import <AppKit/NSViewController.h>
#endif

#import <GameController/GameControllerExtern.h>
#import <GameController/GCControllerElement.h>

#import <GameController/GCControllerAxisInput.h>
#import <GameController/GCControllerButtonInput.h>
#import <GameController/GCControllerDirectionPad.h>

#import <GameController/GCMotion.h>

#import <GameController/GCGamepad.h>
#import <GameController/GCGamepadSnapshot.h>

#import <GameController/GCExtendedGamepad.h>
#import <GameController/GCExtendedGamepadSnapshot.h>

#import <GameController/GCMicroGamepad.h>
#import <GameController/GCMicroGamepadSnapshot.h>

#import <GameController/GCController.h>
