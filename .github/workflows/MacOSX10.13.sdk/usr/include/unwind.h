/* -*- mode: C++; c-basic-offset: 4; -*- 
 *
 * Copyright (c) 2010-2011 Apple Inc. All rights reserved.
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
 * Source compatible with Level 1 Base ABI documented at:
 *    http://www.codesourcery.com/public/cxx-abi/abi-eh.html
 * 
 */


#ifndef __UNWIND_H__
#define __UNWIND_H__

#include <stdint.h>
#include <stddef.h>
#include <Availability.h>


typedef enum {
	_URC_NO_REASON = 0,
	_URC_FOREIGN_EXCEPTION_CAUGHT = 1,
	_URC_FATAL_PHASE2_ERROR = 2,
	_URC_FATAL_PHASE1_ERROR = 3,
	_URC_NORMAL_STOP = 4,
	_URC_END_OF_STACK = 5,
	_URC_HANDLER_FOUND = 6,
	_URC_INSTALL_CONTEXT = 7,
	_URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

typedef enum {
        _UA_SEARCH_PHASE = 1,
        _UA_CLEANUP_PHASE = 2,
        _UA_HANDLER_FRAME = 4,
        _UA_FORCE_UNWIND = 8,
        _UA_END_OF_STACK = 16	// gcc extension to C++ ABI
} _Unwind_Action;


struct _Unwind_Context;		// opaque 
struct _Unwind_Exception;	// forward declaration

struct _Unwind_Exception {
	uint64_t                   exception_class;
	void					 (*exception_cleanup)(_Unwind_Reason_Code reason, struct _Unwind_Exception* exc);
	uintptr_t                  private_1;        // non-zero means forced unwind
	uintptr_t                  private_2;        // holds sp that phase1 found for phase2 to use
#if !__LP64__
	// The gcc implementation of _Unwind_Exception used attribute mode on the above fields
	// which had the side effect of causing this whole struct to round up to 32 bytes in size.
	// To be more explicit, we add pad fields added for binary compatibility.  
	uint32_t				reserved[3];
#endif
};


typedef _Unwind_Reason_Code (*_Unwind_Stop_Fn)
		(int version,
		 _Unwind_Action actions,
		 uint64_t exceptionClass,
		 struct _Unwind_Exception* exceptionObject,
		 struct _Unwind_Context* context,
		 void* stop_parameter );


typedef _Unwind_Reason_Code (*__personality_routine)
	    (int version,
	     _Unwind_Action actions,
	     uint64_t exceptionClass,
	     struct _Unwind_Exception* exceptionObject,
	     struct _Unwind_Context* context);
		 

		 
#ifdef __cplusplus
extern "C" {
#endif

//
// The following are the base functions documented by the C++ ABI
//
#if __arm__
	extern _Unwind_Reason_Code _Unwind_SjLj_RaiseException(struct _Unwind_Exception* exception_object);
	extern void		          _Unwind_SjLj_Resume(struct _Unwind_Exception* exception_object);
#else
	extern _Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception* exception_object);
	extern void		          _Unwind_Resume(struct _Unwind_Exception* exception_object);
#endif
extern void		_Unwind_DeleteException(struct _Unwind_Exception* exception_object);
extern uintptr_t _Unwind_GetGR(struct _Unwind_Context* context, int index);
extern void		_Unwind_SetGR(struct _Unwind_Context* context, int index, uintptr_t new_value);
extern uintptr_t _Unwind_GetIP(struct _Unwind_Context* context);
extern void		_Unwind_SetIP(struct _Unwind_Context*, uintptr_t new_value);
extern uintptr_t _Unwind_GetRegionStart(struct _Unwind_Context* context);
extern uintptr_t _Unwind_GetLanguageSpecificData(struct _Unwind_Context* context);
#if __arm__
	extern _Unwind_Reason_Code _Unwind_SjLj_ForcedUnwind(struct _Unwind_Exception* exception_object, _Unwind_Stop_Fn stop, void* stop_parameter );
#else	
	extern _Unwind_Reason_Code _Unwind_ForcedUnwind(struct _Unwind_Exception* exception_object, _Unwind_Stop_Fn stop, void* stop_parameter );
#endif

#if __arm__
	typedef struct _Unwind_FunctionContext* _Unwind_FunctionContext_t;
	extern void _Unwind_SjLj_Register(_Unwind_FunctionContext_t fc);
	extern void _Unwind_SjLj_Unregister(_Unwind_FunctionContext_t fc);
#endif	
	
//
// The following are semi-suppoted extensions to the C++ ABI
//


//
//	called by __cxa_rethrow().  
//
#if __arm__
	extern _Unwind_Reason_Code _Unwind_SjLj_Resume_or_Rethrow(struct _Unwind_Exception* exception_object);
#else
	extern _Unwind_Reason_Code _Unwind_Resume_or_Rethrow(struct _Unwind_Exception* exception_object);
#endif
	

//
// _Unwind_Backtrace() is a gcc extension that walks the stack and calls the 
// _Unwind_Trace_Fn once per frame until it reaches the bottom of the stack
// or the _Unwind_Trace_Fn function returns something other than _URC_NO_REASON.
//
typedef _Unwind_Reason_Code (*_Unwind_Trace_Fn)(struct _Unwind_Context*, void*);
extern _Unwind_Reason_Code	_Unwind_Backtrace(_Unwind_Trace_Fn, void*);


//
// _Unwind_GetCFA is a gcc extension that can be called from within a personality 
// handler to get the CFA (stack pointer before call) of current frame.
//
extern uintptr_t _Unwind_GetCFA(struct _Unwind_Context*);


//
// _Unwind_GetIPInfo is a gcc extension that can be called from within a personality 
// handler.  Similar to _Unwind_GetIP() but also returns in *ipBefore a non-zero 
// value if the instruction pointer is at or before the instruction causing 
// the unwind.  Normally, in a function call, the IP returned is the return address
// which is after the call instruction and may be past the end of the function
// containing the call instruction.
//
extern uintptr_t _Unwind_GetIPInfo(struct _Unwind_Context* context, int* ipBefore);


//
// __register_frame() is used with dynamically generated code to register the FDE
// for a generated (JIT) code.  The FDE must use pc-rel addressing to point to its
// function and optional LSDA.  __register_frame() has existed in all versions of
// Mac OS X, but in 10.4 and 10.5 it was buggy and did not actually register the 
// FDE with the unwinder.  In 10.6 and later it does register properly.
//
extern void	__register_frame(const void* fde);
extern void	__deregister_frame(const void* fde);


//
// _Unwind_Find_FDE() will locate the FDE if the pc is in some function that has
// an associated FDE. Note, Mac OS X 10.6 and later, introduces "compact unwind info"
// which the runtime uses in preference to dwarf unwind info.  This function
// will only work if the target function has an FDE but no compact unwind info.
//
struct dwarf_eh_bases
{
    uintptr_t tbase;
    uintptr_t dbase;
    uintptr_t func;
};
extern const void* _Unwind_Find_FDE(const void* pc, struct dwarf_eh_bases*);


//
// This function attempts to find the start (address of first instruction) of
// a function given an address inside the function.  It only works if the function
// has an FDE (dwarf unwind info).  
// This function is unimplemented on Mac OS X 10.6 and later.  Instead, use
// _Unwind_Find_FDE() and look at the dwarf_eh_bases.func result.
extern void* _Unwind_FindEnclosingFunction(void* pc);


// Mac OS X does not support text-rel and data-rel addressing so these functions are unimplemented
extern uintptr_t _Unwind_GetDataRelBase(struct _Unwind_Context* context) __attribute__((unavailable));
extern uintptr_t _Unwind_GetTextRelBase(struct _Unwind_Context* context) __attribute__((unavailable));



// Mac OS X 10.4 and 10.5 had implementations of these functions in libgcc_s.dylib,
// but they never worked.  These functions are no longer available.
extern void	 __register_frame_info_bases(const void* fde, void* ob, void* tb, void* db) __attribute__((unavailable));
extern void	 __register_frame_info(const void* fde, void* ob) __attribute__((unavailable));
extern void	 __register_frame_info_table_bases(const void* fde, void* ob,void* tb, void* db) __attribute__((unavailable));
extern void	 __register_frame_info_table(const void* fde, void* ob) __attribute__((unavailable));
extern void	 __register_frame_table(const void* fde) __attribute__((unavailable));
extern void* __deregister_frame_info(const void* fde) __attribute__((unavailable));
extern void* __deregister_frame_info_bases(const void* fde) __attribute__((unavailable));


#ifdef __cplusplus
}
#endif



#endif // __UNWIND_H__


