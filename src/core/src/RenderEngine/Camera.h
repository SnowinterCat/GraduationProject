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

#include "../Math.h"

#include "../DebugException.h"

class Camera {
private:
	// Y轴上的视角角度
	float AngleY;

	// Area.val[0]:Wide			Area.val[1]:High,
	// Area.val[2]:near_plane	Area.val[3]:far_plane
	float4 Area;
	float4 Rotate, Translate;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;	//常量缓冲
public:
	Camera() :AngleY(0), Area(), Rotate(), Translate() {}
	~Camera() = default;
	int init(ID3D11Device* pDevice, float angleY, float Wide, float High, float near_plane, float far_plane);

	int Update(ID3D11DeviceContext* pContext);
	int Bind(ID3D11DeviceContext* pContext);

	int AngleYAdd(float dangleY);
	int SightDistanceAdd(float dnear_plane, float dfar_plane);
	int MoveAdd(float dx, float dy, float dz);
	int RotateAdd(float droX, float droY, float droZ);

	int AngleYTo(float angleY);
	int SightDistanceTo(float near_plane, float far_plane);
	int MoveTo(float x, float y, float z);
	int RotateTo(float roX, float roY, float roZ);
};