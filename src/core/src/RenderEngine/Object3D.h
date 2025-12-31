#pragma once
#include <gp/config.hpp>

#include <vector>
#include "../WindowsAPI/SimpleWindosAPI.h"
#include "../Math.h"
#include "../DebugException.h"

struct VERTEXCOLOR {
	float4 posit;
	float4 color;
};

// 物体为左手系
class Object3d {
public:
	// 顶点数据
	unsigned int VertexNum;
	unsigned int VertexStrides;
	unsigned int VertexOffsets;
	// 索引数据
	unsigned int IndexNum;
	unsigned int IndexStrides;
	unsigned int IndexOffsets;
	// 顶点列表
	std::vector<VERTEXCOLOR> pVertex;
	// 索引列表
	std::vector<unsigned int> pIndices;
private:
	// 其他显示相关的变量和函数

	float4 Rotate, Translate;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;		//顶点缓冲
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;		//索引缓冲
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;	//常量缓冲
public:

	Object3d();
	~Object3d() = default;
	// 输入
	int LoadPointListFromFile(const char* FileName);
	int WritePointListToFile(const char* FileName);
	int CreateObject(ID3D11Device* pDevice);
	int CreatePointListObjectFromFile(const char* FileName, ID3D11Device* pDevice);
	// 输出
	
	// 修改
	int modify(float4x4 rotaMat, float4 transVec);
	// Render
	int Update(ID3D11DeviceContext* pContext);
	int Bind(ID3D11DeviceContext* pContext);
	int Draw(ID3D11DeviceContext* pContext);

	int MoveAdd(float dx, float dy, float dz);
	int ScaleAdd(float dscale);
	int RotateAdd(float droX, float droY, float droZ);

	int MoveTo(float x, float y, float z);
	int ScaleTo(float scale);
	int RotateTo(float roX, float roY, float roZ);
};
