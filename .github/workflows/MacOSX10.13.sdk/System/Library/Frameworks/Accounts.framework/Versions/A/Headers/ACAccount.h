//
//  ACAccount.h
//  Accounts
//
//  Copyright (c) 2011-2012 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Accounts/AccountsDefines.h>

@class ACAccountType, ACAccountCredential;

// The ACAccount class represents an account stored on the system.
// Accounts are created not bound to any store. Once an account is saved it belongs
// to the store it was saved into.

ACCOUNTS_CLASS_AVAILABLE(10_8, 5_0)
@interface ACAccount : NSObject

// Creates a new account object with a specified account type.
- (instancetype)initWithAccountType:(ACAccountType *)type NS_DESIGNATED_INITIALIZER;

// This identifier can be used to look up the account using [ACAccountStore accountWithIdentifier:].
@property (readonly, weak, NS_NONATOMIC_IOSONLY) NSString      *identifier;

// Accounts are stored with a particular account type. All available accounts of a particular type 
// can be looked up using [ACAccountStore accountsWithAccountType:]. When creating new accounts
// this property is required.
@property (strong, NS_NONATOMIC_IOSONLY)   ACAccountType       *accountType;

// A human readable description of the account.
// This property is only available to applications that have been granted access to the account by the user.
@property (copy, NS_NONATOMIC_IOSONLY)     NSString            *accountDescription;

// The username for the account. This property can be set and saved during account creation. The username is
// only available to applications that have been granted access to the account by the user.
@property (copy, NS_NONATOMIC_IOSONLY)     NSString            *username;

// For accounts that support it (currently only Facebook accounts), you can get the user's full name for display
// purposes without having to talk to the network.
@property (readonly, NS_NONATOMIC_IOSONLY)  NSString           *userFullName NS_AVAILABLE_IOS(NA);

// The credential for the account. This property can be set and saved during account creation. It is 
// inaccessible once the account has been saved.
@property (strong, NS_NONATOMIC_IOSONLY)   ACAccountCredential *credential;

@end
