#include "Camera.h"

int Camera::init(ID3D11Device* pDevice, float angleY, float Wide, float High, float near_plane, float far_plane)
{
	AngleY = angleY;
	Area = { Wide, High, near_plane, far_plane };
	{
		float4x4 cb;
		// 常数缓冲描述符
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0u;
		cbDesc.ByteWidth = sizeof(cb);
		cbDesc.StructureByteStride = 0u;        //???
		// 常数缓冲资源描述符
		D3D11_SUBRESOURCE_DATA cbssDesc = {};
		cbssDesc.pSysMem = &cb;
		// 创建常数缓冲
		ThrowIfFailed(pDevice->CreateBuffer(&cbDesc, &cbssDesc, &pConstantBuffer));
	}
	return 0;
}

int Camera::Update(ID3D11DeviceContext* pContext)
{
	float dx = Translate[1], dy = Translate[2], dz = Translate[3];
	DirectX::XMMATRIX trans = { //相机平移矩阵
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		dx,		dy,		dz,		1.0f
	};
	float sinX = std::sin(Rotate[1]), cosX = std::cos(Rotate[1]);
	float sinY = std::sin(Rotate[2]), cosY = std::cos(Rotate[2]);
	float sinZ = std::sin(Rotate[3]), cosZ = std::cos(Rotate[3]);
	// 相机坐标与局部坐标相反
	// 先滚动roZ，再俯仰roX，再偏航roY
	DirectX::XMMATRIX rota = { //相机旋转矩阵
		 cosY*cosZ-sinX*sinY*sinZ, cosX*sinZ, sinY*cosZ+sinX*cosY*sinZ, 0,
		-cosY*sinZ-sinX*sinY*cosZ, cosX*cosZ, cosY*cosZ*sinX-sinY*sinZ, 0,
		-cosX*sinY				 , -sinX	, cosX*cosY				  , 0,
		0, 0, 0, 1
	};
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(AngleY, 1.0f * Area[0] / Area[1], Area[2], Area[3]);
	
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(trans * rota * projection);

	D3D11_MAPPED_SUBRESOURCE mappedData = {};
	ThrowIfFailed(pContext->Map(pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy(mappedData.pData, &transform, sizeof(transform));
	pContext->Unmap(pConstantBuffer.Get(), 0);
    return 0;
}

int Camera::Bind(ID3D11DeviceContext* pContext)
{
	// 绑定常数缓冲
	pContext->VSSetConstantBuffers(1u, 1u, pConstantBuffer.GetAddressOf());
	return 0;
}

int Camera::AngleYAdd(float dangleY)
{
	AngleY += dangleY;
	return 0;
}

int Camera::SightDistanceAdd(float dnear_plane, float dfar_plane)
{
	Area[2] += dnear_plane;
	Area[3] += dfar_plane;
	return 0;
}

int Camera::MoveAdd(float dx, float dy, float dz)
{
	Translate[1] -= dx;
	Translate[2] -= dy;
	Translate[3] -= dz;
	return 0;
}

int Camera::RotateAdd(float droX, float droY, float droZ)
{
	Rotate[1] -= droX;
	Rotate[2] -= droY;
	Rotate[3] -= droZ;
	return 0;
}

int Camera::AngleYTo(float angleY)
{
	AngleY = angleY;
	return 0;
}

int Camera::SightDistanceTo(float near_plane, float far_plane)
{
	Area[2] = near_plane;
	Area[3] = far_plane;
	return 0;
}

int Camera::MoveTo(float x, float y, float z)
{
	Translate[1] = -x;
	Translate[2] = -y;
	Translate[3] = -z;
	return 0;
}

int Camera::RotateTo(float roX, float roY, float roZ)
{
	Rotate[1] = -roX;
	Rotate[2] = -roY;
	Rotate[3] = -roZ;
	return 0;
}
