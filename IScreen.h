

class ScreenHandle {
public:
    virtual void onShapeUpdate(int w, int h) = 0;
    virtual void onMouseMove(float x, float y) = 0;
};

class IScreen {
public:
    static IScreen* getScreen();

    IScreen():m_handle(nullptr) {}
    virtual ~IScreen() { }

    virtual void setHandle(ScreenHandle *); 

    // Get screen shape
    /*!
    Return the position of the upper-left corner of the screen in x and y and the size of the screen in width and height.
    */
    virtual void getShape(int& width, int& height) const = 0;

    //! Get cursor position
    /*!
    Return the current position of the cursor in x and y.
    */
    virtual void getCursorPos(int& x, int& y) const = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
    virtual void setCursorVisible(bool) = 0;
    virtual void warpCursor(int x, int y) = 0;

protected:
    ScreenHandle *m_handle;

};
