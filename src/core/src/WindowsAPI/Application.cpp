#include "Application.h"

int Application::Register(HINSTANCE hInstance, WNDPROC WndProc)
{
	LoadString(hInstance, IDS_WinName, WinClassName, MAX_LOADSTRING);
	LoadString(hInstance, IDS_WinTitle, WinTitle, MAX_LOADSTRING);
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WinIcon));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WinClassName;
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WinIcon));
	return RegisterClassEx(&wc);
}

int Application::CreatingWindow(HINSTANCE hInstance, int Wide, int High)
{
	RECT WinRect = { 0,0,Wide,High };
	AdjustWindowRect(&WinRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	Hwnd = CreateWindowEx(0, WinClassName, WinTitle,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		0, 0, WinRect.right - WinRect.left, WinRect.bottom - WinRect.top,
		nullptr, nullptr, hInstance, nullptr);
	if (Hwnd == nullptr)
		return -1;
	ShowWindow(Hwnd, SW_SHOW);
	UpdateWindow(Hwnd);
	return 0;
}
