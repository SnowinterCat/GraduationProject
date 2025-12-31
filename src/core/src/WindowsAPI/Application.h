#pragma once
#include <gp/config.hpp>

#include "SimpleWindosAPI.h"
#include "../resource.h"

class Application {
	static const unsigned int MAX_LOADSTRING = 256u;

	TCHAR WinClassName[MAX_LOADSTRING];
	TCHAR WinTitle[MAX_LOADSTRING];

	HWND Hwnd;
public:
	Application() :WinClassName(), WinTitle(), Hwnd(nullptr){}
	int Register(HINSTANCE hInstance, WNDPROC WndProc);
	int CreatingWindow(HINSTANCE hInstance, int Wide, int High);
};
