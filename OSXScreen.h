

#include "IScreen.h"


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

class OSXScreen : public IScreen {
public:
    OSXScreen();
    virtual void getShape(int& x, int& y,
                          int& width, int& height) const;

    virtual void getCursorPos(int& x, int& y) const;
    virtual void run();
    virtual void stop();
    virtual void setCursorVisible(bool);
    virtual void warpCursor(int x, int y);


    // message handlers
    bool onMouseMove(CGFloat mx, CGFloat my);
    // mouse button handler.  pressed is true if this is a mousedown
    // event, false if it is a mouseup event.  macButton is the index
    // of the button pressed using the mac button mapping.
    bool onMouseButton(bool pressed, UInt16 macButton);
    bool onMouseWheel(SInt32 xDelta, SInt32 yDelta) const;

    // Quartz event tap support
    static CGEventRef handleCGInputEvent(CGEventTapProxy proxy,
                                            CGEventType type,
                                            CGEventRef event,
                                            void* refcon);
private:
    bool updateScreenShape();
    // the display
    CGDirectDisplayID    m_displayID;

    // screen shape stuff
    int m_x, m_y;
    int m_w, m_h;
    int m_xCenter, m_yCenter;
    bool m_cursorShow;

    // mouse state
    mutable int        m_xCursor, m_yCursor;

    CFRunLoopRef m_loop;
    CFMachPortRef            m_eventTapPort;
    CFRunLoopSourceRef        m_eventTapRLSR;

};