#pragma once
#include <gp/config.hpp>

#include "../WindowsAPI.h"
#include "Camera.h"
#include "Object3D.h"
#include "../Registration.h"

#include "../DebugException.h"

class Graphics {
	static const unsigned int BufferLens = 256u;

	UINT Wide, High;

	UINT GPUnum;
	char GPUInformation[8][BufferLens];
	Timer GraphicTimer;
	Camera MainCamera;

	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;//渲染目标视图
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;//深度模板视图
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;//深度模板状态
	D3D11_VIEWPORT ViewPort;// 观察视角
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;//顶点着色器
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;//像素着色器
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;//输入布局（输入格式）
public:
	// 3D资源
	Object3d Obj[8];
	//Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;		//顶点缓冲
	//Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;		//索引缓冲
	//Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;		//常量缓冲

public:
	Graphics();
	~Graphics() = default;
	int init(HWND hWnd, UINT GPUid);
	void load3DResource();
	void Render();

	bool match(bool& Flag);
	bool RefreshObject(int index);
private:
	void StartFrame();
	void DoFrame();
	void EndFrame();
};

inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShaderFromFile(const WCHAR* filename, const D3D_SHADER_MACRO* defines, const char* entrypoint, const char* target)
{
	// 根据_DEBUG选择Shader的编译方式
	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	Microsoft::WRL::ComPtr<ID3DBlob> Shader = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	HRESULT hr = D3DCompileFromFile(filename, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint, target, compileFlags, 0, &Shader, &errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	return Shader;
}