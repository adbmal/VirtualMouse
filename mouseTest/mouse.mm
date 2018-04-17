//
//  mouse.cpp
//  mouseTest
//
//  Created by Yiwei Wang on 06/04/2018.
//  Copyright © 2018 Yiwei Wang. All rights reserved.
//

#include "mouse.hpp"

OSXScreen::OSXScreen() {
    NSLog(@"OSXScreen init");
    m_displayID = CGMainDisplayID();
    NSLog(@"CGDirectDisplayID %d", m_displayID);
    CFMachPortRef            m_eventTapPort;
    CFRunLoopSourceRef        m_eventTapRLSR;
    m_eventTapPort = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                                      kCGEventMaskForAllEvents,
                                      handleCGInputEvent,
                                      this);
    if (!m_eventTapPort) {
        NSLog(@"failed to create quartz event tap");
    }
    
    m_eventTapRLSR = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, m_eventTapPort, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), m_eventTapRLSR, kCFRunLoopDefaultMode);
    
}

OSXScreen::~OSXScreen()
{
    
}


void OSXScreen::updateScreenShape() {
    // get info for each display
    CGDisplayCount displayCount = 0;
    
    if (CGGetActiveDisplayList(0, NULL, &displayCount) != CGDisplayNoErr) {
        return;
    }
    
    if (displayCount == 0) {
        return;
    }
    
    CGDirectDisplayID* displays = new CGDirectDisplayID[displayCount];
    if (displays == NULL) {
        return;
    }
    
    if (CGGetActiveDisplayList(displayCount,
                               displays, &displayCount) != CGDisplayNoErr) {
        delete[] displays;
        return;
    }
    
    // get smallest rect enclosing all display rects
    CGRect totalBounds = CGRectZero;
    for (CGDisplayCount i = 0; i < displayCount; ++i) {
        CGRect bounds = CGDisplayBounds(displays[i]);
        totalBounds   = CGRectUnion(totalBounds, bounds);
    }
    
    // get shape of default screen
    m_x = (SInt32)totalBounds.origin.x;
    m_y = (SInt32)totalBounds.origin.y;
    m_w = (SInt32)totalBounds.size.width;
    m_h = (SInt32)totalBounds.size.height;
    
    // get center of default screen
    CGDirectDisplayID main = CGMainDisplayID();
    const CGRect rect = CGDisplayBounds(main);
    m_xCenter = (rect.origin.x + rect.size.width) / 2;
    m_yCenter = (rect.origin.y + rect.size.height) / 2;
    
    delete[] displays;

    NSLog(@"screen shape: center=%d,%d size=%dx%d on %u %s",
          m_x, m_y, m_w, m_h, displayCount,
          (displayCount == 1) ? "display" : "displays");
}


void
OSXScreen::hideCursor(){
    CFStringRef propertyString = CFStringCreateWithCString(
                                                           NULL, "SetsCursorInBackground", kCFStringEncodingMacRoman);
    
    CGSSetConnectionProperty(
                             _CGSDefaultConnection(), _CGSDefaultConnection(),
                             propertyString, kCFBooleanTrue);
    
    CFRelease(propertyString);
    
    CGError error = CGDisplayHideCursor(m_displayID);
    if (error != kCGErrorSuccess) {
        NSLog(@"failed to hide cursor, error=%d", error);
    }
    
    // appears to fix "mouse randomly not hiding" bug
    CGAssociateMouseAndMouseCursorPosition(true);

}



void
OSXScreen::warpCursor(SInt32 x, SInt32 y)
{
    // move cursor without generating events
    CGPoint pos;
    pos.x = x;
    pos.y = y;
    CGWarpMouseCursorPosition(pos);
    
    // save new cursor position
    m_xCursor        = x;
    m_yCursor        = y;
    m_cursorPosValid = true;
}

// Quartz event tap support
CGEventRef OSXScreen::handleCGInputEvent(CGEventTapProxy proxy,
                              CGEventType type,
                              CGEventRef event,
                              void* refcon)
{
    OSXScreen* screen = (OSXScreen*)refcon;
    CGPoint pos;
    CGKeyCode keycode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    NSLog(@"key code %d", keycode);
    switch(type) {
        case kCGEventLeftMouseDown:
        case kCGEventRightMouseDown:
        case kCGEventOtherMouseDown:
            NSLog(@"kCGEventMouseDown %lld", CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber) + 1);
                        screen->onMouseButton(true, CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber) + 1);
            break;
        case kCGEventLeftMouseUp:
        case kCGEventRightMouseUp:
        case kCGEventOtherMouseUp:
            NSLog(@"kCGEventMouseUp %lld", CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber) + 1);
                        screen->onMouseButton(false, CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber) + 1);
            break;
        case kCGEventLeftMouseDragged:
        case kCGEventRightMouseDragged:
        case kCGEventOtherMouseDragged:
        case kCGEventMouseMoved:
            pos = CGEventGetLocation(event);
            screen->onMouseMove(pos.x, pos.y);
            NSLog(@"onMouseMove %f %f", pos.x, pos.y);

            // The system ignores our cursor-centering calls if
            // we don't return the event. This should be harmless,
            // but might register as slight movement to other apps
            // on the system. It hasn't been a problem before, though.
            return event;
            break;
        case kCGEventScrollWheel:
            NSLog(@"kCGEventScrollWheel");
//            screen->onMouseWheel(screen->mapScrollWheelToSynergy(
//                                                                 CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis2)),
//                                 screen->mapScrollWheelToSynergy(
//                                                                 CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1)));
            break;
        case kCGEventKeyDown:
            NSLog(@"kCGEventKeyDown");
            break;
        case kCGEventKeyUp:
            NSLog(@"kCGEventKeyUp");
            break;
        case kCGEventFlagsChanged:
            NSLog(@"kCGEventFlagsChanged");
            //            screen->onKey(event);
            break;
        case kCGEventTapDisabledByTimeout:
            // Re-enable our event-tap
            //            CGEventTapEnable(screen->m_eventTapPort, true);
            //            LOG((CLOG_INFO "quartz event tap was disabled by timeout, re-enabling"));
            break;
        case kCGEventTapDisabledByUserInput:
            //            LOG((CLOG_ERR "quartz event tap was disabled by user input"));
            break;
        case NX_NULLEVENT:
            break;
        default:
            if (type == NX_SYSDEFINED) {
                if (screen->isMediaKeyEvent (event)) {
                    NSLog(@"detected media key event");
//                    screen->onMediaKey (event);
                } else {
                    NSLog(@"ignoring unknown system defined event");
                    return event;
                }
                break;
            }
            
            //            LOG((CLOG_DEBUG3 "unknown quartz event type: 0x%02x", type));
    }
    
    //    if (screen->m_isOnScreen) {
    //        return event;
    //    } else {
    //        return NULL;
    //    }
    return NULL;
}

bool
OSXScreen::onMouseMove(CGFloat mx, CGFloat my)
{
    
    CGFloat x = mx - m_xCursor;
    CGFloat y = my - m_yCursor;
    NSLog(@"mouse move %+f,%+f, %+f,%+f", mx, my, x, y);

    if ((x == 0 && y == 0) || (mx == m_xCenter && mx == m_yCenter)) {
        return true;
    }
    
    // save position to compute delta of next motion
    m_xCursor = (SInt32)mx;
    m_yCursor = (SInt32)my;
    
//    if (m_isOnScreen) {
//        // motion on primary screen
//        sendEvent(m_events->forIPrimaryScreen().motionOnPrimary(),
//                  MotionInfo::alloc(m_xCursor, m_yCursor));
//        if (m_buttonState.test(0)) {
//            m_draggingStarted = true;
//        }
//    }
//    else {
//        // motion on secondary screen.  warp mouse back to
//        // center.
        warpCursor(m_xCenter, m_yCenter);
//
//        // examine the motion.  if it's about the distance
//        // from the center of the screen to an edge then
//        // it's probably a bogus motion that we want to
//        // ignore (see warpCursorNoFlush() for a further
//        // description).
//        static SInt32 bogusZoneSize = 10;
//        if (-x + bogusZoneSize > m_xCenter - m_x ||
//            x + bogusZoneSize > m_x + m_w - m_xCenter ||
//            -y + bogusZoneSize > m_yCenter - m_y ||
//            y + bogusZoneSize > m_y + m_h - m_yCenter) {
//            LOG((CLOG_DEBUG "dropped bogus motion %+d,%+d", x, y));
//        }
//        else {
//            // send motion
//            // Accumulate together the move into the running total
//            static CGFloat m_xFractionalMove = 0;
//            static CGFloat m_yFractionalMove = 0;
//
//            m_xFractionalMove += x;
//            m_yFractionalMove += y;
//
//            // Return the integer part
//            SInt32 intX = (SInt32)m_xFractionalMove;
//            SInt32 intY = (SInt32)m_yFractionalMove;
//
//            // And keep only the fractional part
//            m_xFractionalMove -= intX;
//            m_yFractionalMove -= intY;
//            sendEvent(m_events->forIPrimaryScreen().motionOnSecondary(), MotionInfo::alloc(intX, intY));
//        }
//    }
    
    return true;
}

bool
OSXScreen::onMouseButton(bool pressed, UInt16 macButton)
{
//    // Buttons 2 and 3 are inverted on the mac
//    ButtonID button = mapMacButtonToSynergy(macButton);
//
//    if (pressed) {
//        LOG((CLOG_DEBUG1 "event: button press button=%d", button));
//        if (button != kButtonNone) {
//            KeyModifierMask mask = m_keyState->getActiveModifiers();
//            sendEvent(m_events->forIPrimaryScreen().buttonDown(), ButtonInfo::alloc(button, mask));
//        }
//    }
//    else {
//        LOG((CLOG_DEBUG1 "event: button release button=%d", button));
//        if (button != kButtonNone) {
//            KeyModifierMask mask = m_keyState->getActiveModifiers();
//            sendEvent(m_events->forIPrimaryScreen().buttonUp(), ButtonInfo::alloc(button, mask));
//        }
//    }
//
//    // handle drags with any button other than button 1 or 2
//    if (macButton > 2) {
//        if (pressed) {
//            // one more button
//            if (m_dragNumButtonsDown++ == 0) {
//                enableDragTimer(true);
//            }
//        }
//        else {
//            // one less button
//            if (--m_dragNumButtonsDown == 0) {
//                enableDragTimer(false);
//            }
//        }
//    }
//
//    if (macButton == kButtonLeft) {
//        EMouseButtonState state = pressed ? kMouseButtonDown : kMouseButtonUp;
//        m_buttonState.set(kButtonLeft - 1, state);
//        if (pressed) {
//            m_draggingFilename.clear();
//            LOG((CLOG_DEBUG2 "dragging file directory is cleared"));
//        }
//        else {
//            if (m_fakeDraggingStarted) {
//                m_getDropTargetThread = new Thread(new TMethodJob<OSXScreen>(
//                                                                             this, &OSXScreen::getDropTargetThread));
//            }
//
//            m_draggingStarted = false;
//        }
//    }
    
    return true;
}

bool
OSXScreen::onMouseWheel(SInt32 xDelta, SInt32 yDelta) const
{
    NSLog(@"event: button wheel delta=%+d,%+d", xDelta, yDelta);
//    sendEvent(m_events->forIPrimaryScreen().wheel(), WheelInfo::alloc(xDelta, yDelta));
    return true;
}

bool OSXScreen::isMediaKeyEvent(CGEventRef event) {
    NSEvent* nsEvent = nil;
    @try {
        nsEvent = [NSEvent eventWithCGEvent: event];
        if ([nsEvent subtype] != 8) {
            return false;
        }
        uint32_t const nxKeyId = ([nsEvent data1] & 0xFFFF0000) >> 16;
//        if (convertNXKeyTypeToKeyID (nxKeyId)) {
            return true;
//        }
    } @catch (NSException* e) {
    }
    return false;
}
