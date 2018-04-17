

#include "IScreen.h"

class MSScreen : public IScreen {
public:
    MSScreen();
    ~MSScreen();
    virtual void getShape(int& width, int& height) const;

    virtual void getCursorPos(int& x, int& y) const;
    virtual void run();
    virtual void stop();
    virtual void setCursorVisible(bool);
    virtual void warpCursor(int x, int y);

private:

    // screen shape stuff
    int m_x, m_y;
    int m_w, m_h;
    int m_xCenter, m_yCenter;
    bool m_cursorShow;

    // mouse state
    mutable int        m_xCursor, m_yCursor;
};