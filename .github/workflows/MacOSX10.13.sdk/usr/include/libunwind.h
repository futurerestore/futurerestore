/* -*- mode: C; c-basic-offset: 4; -*- 
 *
 * Copyright (c) 2008-2011 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 * 
 * 
 *            C interface to libuwind 
 *
 * Source compatible with libuwind API documented at:
 *    http://www.nongnu.org/libunwind/man/libunwind(3).html
 * 
 */


#ifndef __LIBUNWIND__
#define __LIBUNWIND__

#include <stdint.h>
#include <stddef.h>
#include <mach/mach_types.h>
#include <Availability.h>

/* error codes */
enum {
    UNW_ESUCCESS           = 0,            /* no error */
    UNW_EUNSPEC            = -6540,        /* unspecified (general) error */
    UNW_ENOMEM             = -6541,        /* out of memory */
    UNW_EBADREG            = -6542,        /* bad register number */
    UNW_EREADONLYREG       = -6543,        /* attempt to write read-only register */
    UNW_ESTOPUNWIND        = -6544,        /* stop unwinding */
    UNW_EINVALIDIP         = -6545,        /* invalid IP */
    UNW_EBADFRAME          = -6546,        /* bad frame */
    UNW_EINVAL             = -6547,        /* unsupported operation or bad value */
    UNW_EBADVERSION        = -6548,        /* unwind info has unsupported version */
    UNW_ENOINFO            = -6549         /* no unwind info found */
};


struct unw_context_t { uint64_t data[128]; };
typedef struct unw_context_t     unw_context_t;

struct unw_cursor_t { uint64_t data[140]; };
typedef struct unw_cursor_t      unw_cursor_t;

typedef struct unw_addr_space*   unw_addr_space_t;

typedef int                      unw_regnum_t;
typedef uint64_t                 unw_word_t;
typedef double                   unw_fpreg_t;

struct unw_proc_info_t
{
    unw_word_t    start_ip;         /* start address of function */
    unw_word_t    end_ip;           /* address after end of function */
    unw_word_t    lsda;             /* address of language specific data area, or zero if not used */
    unw_word_t    handler;          /* personality routine, or zero if not used */
    unw_word_t    gp;               /* not used */
    unw_word_t    flags;            /* not used */
    uint32_t      format;           /* compact unwind encoding, or zero if none */
    uint32_t      unwind_info_size; /* size of dwarf unwind info, or zero if none */
    unw_word_t    unwind_info;      /* address of dwarf unwind info, or zero if none */
    unw_word_t    extra;            /* mach_header of mach-o image containing function */
};
typedef struct unw_proc_info_t unw_proc_info_t;


#ifdef __cplusplus
extern "C" {
#endif

#if !__arm__
extern int         unw_getcontext(unw_context_t*)                               __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_init_local(unw_cursor_t*, unw_context_t*)                __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_step(unw_cursor_t*)                                      __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_get_reg(unw_cursor_t*, unw_regnum_t, unw_word_t*)        __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_get_fpreg(unw_cursor_t*, unw_regnum_t, unw_fpreg_t*)     __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_set_reg(unw_cursor_t*, unw_regnum_t, unw_word_t)         __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_set_fpreg(unw_cursor_t*, unw_regnum_t, unw_fpreg_t)      __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_resume(unw_cursor_t*)                                    __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);

extern const char* unw_regname(unw_cursor_t*, unw_regnum_t)                     __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_get_proc_info(unw_cursor_t*, unw_proc_info_t*)           __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_is_fpreg(unw_cursor_t*, unw_regnum_t)                    __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_is_signal_frame(unw_cursor_t*)                           __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
extern int         unw_get_proc_name(unw_cursor_t*, char*, size_t, unw_word_t*) __OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_5_0);
//extern int       unw_get_save_loc(unw_cursor_t*, int, unw_save_loc_t*);
#endif


#if UNW_REMOTE
/* 
 * Mac OS X "remote" API for unwinding other processes on same machine
 *
 */
extern unw_addr_space_t    unw_local_addr_space;
extern unw_addr_space_t    unw_create_addr_space_for_task(task_t);
extern void                unw_destroy_addr_space(unw_addr_space_t);
extern int                 unw_init_remote_thread(unw_cursor_t*, unw_addr_space_t, thread_t*);
#endif


/* 
 * traditional libuwind "remote" API 
 *   NOT IMPLEMENTED on Mac OS X
 *
 * extern int               unw_init_remote(unw_cursor_t*, unw_addr_space_t, thread_t*);
 * extern unw_accessors_t   unw_get_accessors(unw_addr_space_t);
 * extern unw_addr_space_t  unw_create_addr_space(unw_accessors_t, int);
 * extern void              unw_flush_cache(unw_addr_space_t, unw_word_t, unw_word_t);
 * extern int               unw_set_caching_policy(unw_addr_space_t, unw_caching_policy_t);
 * extern void              _U_dyn_register(unw_dyn_info_t*);
 * extern void              _U_dyn_cancel(unw_dyn_info_t*);
 */


#ifdef __cplusplus
}
#endif


// architecture independent register numbers 
enum {
    UNW_REG_IP = -1,        // instruction pointer
    UNW_REG_SP = -2,        // stack pointer
};


// 32-bit x86 registers
enum {
    UNW_X86_EAX = 0,
    UNW_X86_ECX = 1,
    UNW_X86_EDX = 2,
    UNW_X86_EBX = 3,
    UNW_X86_EBP = 4,
    UNW_X86_ESP = 5,
    UNW_X86_ESI = 6,
    UNW_X86_EDI = 7
};


// 64-bit x86_64 registers
enum {
    UNW_X86_64_RAX =  0,
    UNW_X86_64_RDX =  1,
    UNW_X86_64_RCX =  2,
    UNW_X86_64_RBX =  3,
    UNW_X86_64_RSI =  4,
    UNW_X86_64_RDI =  5,
    UNW_X86_64_RBP =  6,
    UNW_X86_64_RSP =  7,
    UNW_X86_64_R8  =  8,
    UNW_X86_64_R9  =  9,
    UNW_X86_64_R10 = 10,
    UNW_X86_64_R11 = 11,
    UNW_X86_64_R12 = 12,
    UNW_X86_64_R13 = 13,
    UNW_X86_64_R14 = 14,
    UNW_X86_64_R15 = 15
};


// 32-bit ppc register numbers
enum {
    UNW_PPC_R0  =  0,
    UNW_PPC_R1  =  1,
    UNW_PPC_R2  =  2,
    UNW_PPC_R3  =  3,
    UNW_PPC_R4  =  4,
    UNW_PPC_R5  =  5,
    UNW_PPC_R6  =  6,
    UNW_PPC_R7  =  7,
    UNW_PPC_R8  =  8,
    UNW_PPC_R9  =  9,
    UNW_PPC_R10 = 10,
    UNW_PPC_R11 = 11,
    UNW_PPC_R12 = 12,
    UNW_PPC_R13 = 13,
    UNW_PPC_R14 = 14,
    UNW_PPC_R15 = 15,
    UNW_PPC_R16 = 16,
    UNW_PPC_R17 = 17,
    UNW_PPC_R18 = 18,
    UNW_PPC_R19 = 19,
    UNW_PPC_R20 = 20,
    UNW_PPC_R21 = 21,
    UNW_PPC_R22 = 22,
    UNW_PPC_R23 = 23,
    UNW_PPC_R24 = 24,
    UNW_PPC_R25 = 25,
    UNW_PPC_R26 = 26,
    UNW_PPC_R27 = 27,
    UNW_PPC_R28 = 28,
    UNW_PPC_R29 = 29,
    UNW_PPC_R30 = 30,
    UNW_PPC_R31 = 31,
    UNW_PPC_F0  = 32,
    UNW_PPC_F1  = 33,
    UNW_PPC_F2  = 34,
    UNW_PPC_F3  = 35,
    UNW_PPC_F4  = 36,
    UNW_PPC_F5  = 37,
    UNW_PPC_F6  = 38,
    UNW_PPC_F7  = 39,
    UNW_PPC_F8  = 40,
    UNW_PPC_F9  = 41,
    UNW_PPC_F10 = 42,
    UNW_PPC_F11 = 43,
    UNW_PPC_F12 = 44,
    UNW_PPC_F13 = 45,
    UNW_PPC_F14 = 46,
    UNW_PPC_F15 = 47,
    UNW_PPC_F16 = 48,
    UNW_PPC_F17 = 49,
    UNW_PPC_F18 = 50,
    UNW_PPC_F19 = 51,
    UNW_PPC_F20 = 52,
    UNW_PPC_F21 = 53,
    UNW_PPC_F22 = 54,
    UNW_PPC_F23 = 55,
    UNW_PPC_F24 = 56,
    UNW_PPC_F25 = 57,
    UNW_PPC_F26 = 58,
    UNW_PPC_F27 = 59,
    UNW_PPC_F28 = 60,
    UNW_PPC_F29 = 61,
    UNW_PPC_F30 = 62,
    UNW_PPC_F31 = 63,
    UNW_PPC_MQ  = 64,
    UNW_PPC_LR  = 65,
    UNW_PPC_CTR = 66,
    UNW_PPC_AP  = 67,
	UNW_PPC_CR0 = 68,
    UNW_PPC_CR1 = 69,
	UNW_PPC_CR2 = 70,
	UNW_PPC_CR3 = 71,
	UNW_PPC_CR4 = 72,
	UNW_PPC_CR5 = 73,
	UNW_PPC_CR6 = 74,
	UNW_PPC_CR7 = 75,
	UNW_PPC_XER = 76,
	UNW_PPC_V0  = 77,
    UNW_PPC_V1  = 78,
    UNW_PPC_V2  = 79,
    UNW_PPC_V3  = 80,
    UNW_PPC_V4  = 81,
    UNW_PPC_V5  = 82,
    UNW_PPC_V6  = 83,
    UNW_PPC_V7  = 84,
    UNW_PPC_V8  = 85,
    UNW_PPC_V9  = 86,
    UNW_PPC_V10 = 87,
    UNW_PPC_V11 = 88,
    UNW_PPC_V12 = 89,
    UNW_PPC_V13 = 90,
    UNW_PPC_V14 = 91,
    UNW_PPC_V15 = 92,
    UNW_PPC_V16 = 93,
    UNW_PPC_V17 = 94,
    UNW_PPC_V18 = 95,
    UNW_PPC_V19 = 96,
    UNW_PPC_V20 = 97,
    UNW_PPC_V21 = 98,
    UNW_PPC_V22 = 99,
    UNW_PPC_V23 = 100,
    UNW_PPC_V24 = 101,
    UNW_PPC_V25 = 102,
    UNW_PPC_V26 = 103,
    UNW_PPC_V27 = 104,
    UNW_PPC_V28 = 105,
    UNW_PPC_V29 = 106,
    UNW_PPC_V30 = 107,
    UNW_PPC_V31 = 108,
    UNW_PPC_VRSAVE  = 109,
    UNW_PPC_VSCR    = 110,
    UNW_PPC_SPE_ACC = 111,
    UNW_PPC_SPEFSCR = 112
	
};




#endif

