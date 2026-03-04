#pragma once
#include <gp/config.hpp>

#include <exception>
#include <sstream>
#include <cstdio>

#include <Windows.h>
#include <wrl.h>
// DirectX11运行库
#include <dxgi.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

constexpr unsigned int DebugLens = 256u;

class DebugException :public std::exception
{
public:
	DebugException(int Line, const char* File, HRESULT hr) noexcept
		:line(Line), file(File)
	{
		ErrorCode = (int)hr;
	}
	const char* what()const noexcept override;
private:
	int line;
	std::string file;
	int ErrorCode;
protected:
	mutable std::string whatBuffer;
};

#define ThrowIfFailed(hr) {HRESULT _hr=hr;if(FAILED(_hr)) {throw DebugException(__LINE__, __FILE__, _hr); }}

inline const char* DebugException::what() const noexcept
{
	char DebugBuffer[DebugLens] = {};
	std::snprintf(DebugBuffer, DebugLens, "[File]%s\n[Line]%d\n[Code]0x%x\n", file.c_str(), line, ErrorCode);
	OutputDebugStringA(DebugBuffer);
	whatBuffer = DebugBuffer;
	return whatBuffer.c_str();
}