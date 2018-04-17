
#include "MSScreen.h"

IScreen* IScreen::getScreen() {
    return new MSScreen();
}


MSScreen::MSScreen() {
}
MSScreen::~MSScreen() {
}

void MSScreen::getShape(int& w, int& h) const {
	w = m_w;
	h = m_h;
}

void MSScreen::getCursorPos(int& x, int& y) const {

}

void MSScreen::run() {
}

void MSScreen::stop() {
}

void MSScreen::setCursorVisible(bool show) {

}


void MSScreen::warpCursor(int x, int y)
{
    
}