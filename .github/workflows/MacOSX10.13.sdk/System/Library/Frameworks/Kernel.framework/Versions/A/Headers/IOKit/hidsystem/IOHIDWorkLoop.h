
#ifndef _IOKIT_IOHIDWORKLOOP_H
#define _IOKIT_IOHIDWORKLOOP_H

#include <IOKit/IOWorkLoop.h>
class IOHIDWorkLoop : public IOWorkLoop
{
    OSDeclareDefaultStructors(IOHIDWorkLoop)
    
protected:
	lck_grp_t *	fLockGroup;
	
	bool init ( void );
	void free ( void );
	
public:
    // Create a workloop
    static IOHIDWorkLoop * workLoop();
};

#endif /* ! _IOKIT_IOHIDWORKLOOP_H */

