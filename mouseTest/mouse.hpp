//
//  mouse.hpp
//  mouseTest
//
//  Created by Yiwei Wang on 06/04/2018.
//  Copyright © 2018 Yiwei Wang. All rights reserved.
//

#ifndef mouse_hpp
#define mouse_hpp

#include <stdio.h>

#include <bitset>
#include <Carbon/Carbon.h>
#include <mach/mach_port.h>
#include <mach/mach_interface.h>
#include <mach/mach_init.h>

#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>

#include <AvailabilityMacros.h>
#include <IOKit/hidsystem/event_status_driver.h>
#include <AppKit/NSEvent.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <cstdlib>


extern "C" {
    typedef int CGSConnectionID;
    CGError CGSSetConnectionProperty(CGSConnectionID cid, CGSConnectionID targetCID, CFStringRef key, CFTypeRef value);
    int _CGSDefaultConnection();
}

class OSXScreen {
public:
    OSXScreen();
    void updateScreenShape();
    
    void warpCursor(SInt32 x, SInt32 y);
    // message handlers
    bool                onMouseMove(CGFloat mx, CGFloat my);
    // mouse button handler.  pressed is true if this is a mousedown
    // event, false if it is a mouseup event.  macButton is the index
    // of the button pressed using the mac button mapping.
    bool                onMouseButton(bool pressed, UInt16 macButton);
    bool                onMouseWheel(SInt32 xDelta, SInt32 yDelta) const;

    
    void hideCursor();
    virtual ~OSXScreen();
    
    // Quartz event tap support
    static CGEventRef    handleCGInputEvent(CGEventTapProxy proxy,
                                            CGEventType type,
                                            CGEventRef event,
                                            void* refcon);

private:
    CGDirectDisplayID m_displayID;
    // screen shape stuff
    SInt32                m_x, m_y;
    SInt32                m_w, m_h;
    SInt32                m_xCenter, m_yCenter;
    
    // mouse state
    mutable SInt32        m_xCursor, m_yCursor;
    mutable bool        m_cursorPosValid;

};
    // IScreen overrides
#endif /* mouse_hpp */
