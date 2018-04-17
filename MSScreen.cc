
#include "MSScreen.h"

#include <windows.h>

#include <stdio.h>
IScreen* IScreen::getScreen() {
    return new MSScreen();
}


HHOOK hMouseHook;

LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
	if (pMouseStruct != NULL) {
		if (wParam == WM_LBUTTONDOWN)
		{
			printf("clicked");
		}
		printf("Mouse position X = %d  Mouse Position Y = %d\n", pMouseStruct->pt.x, pMouseStruct->pt.y);
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
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
   	HINSTANCE hInstance = GetModuleHandle(NULL);

	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, hInstance, NULL);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	UnhookWindowsHookEx(hMouseHook);
}

void MSScreen::stop() {
}

void MSScreen::setCursorVisible(bool show) {

}


void MSScreen::warpCursor(int x, int y)
{

}

