#pragma once
#include <gp/config.hpp>

#include <Windows.h>
#include <wrl.h>
// DirectX11运行库
#include <dxgi.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

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
