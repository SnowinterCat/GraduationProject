#include "Object3D.h"

Object3d::Object3d() :VertexNum(0), VertexStrides(0), VertexOffsets(0), IndexNum(0), IndexStrides(0), IndexOffsets(0)
{}

int Object3d::LoadPointListFromFile(const char* FileName)
{
	FILE* ObjectFile = nullptr;
	fopen_s(&ObjectFile, FileName, "r");
	if (ObjectFile == nullptr)
		return -2;
	fscanf_s(ObjectFile, "%u%u", &VertexNum, &IndexNum);
	IndexNum *= 3;
	VertexStrides = sizeof(VERTEXCOLOR);
	IndexStrides = sizeof(unsigned int);
	VertexOffsets = 0;
	IndexOffsets = 0;

	pVertex.resize(VertexNum);
	pIndices.resize(IndexNum);
	for (unsigned int i = 0; i < VertexNum; i++) {
		fscanf_s(ObjectFile, "%f%f%f%f%f%f%f%f",
			&pVertex[i].posit[0], &pVertex[i].posit[1], &pVertex[i].posit[2], &pVertex[i].posit[3],
			&pVertex[i].color[0], &pVertex[i].color[1], &pVertex[i].color[2], &pVertex[i].color[3]);
	}
	for (unsigned int i = 0; i < IndexNum; i+=3) {
		fscanf_s(ObjectFile, "%u%u%u", &pIndices[i], &pIndices[i + 1], &pIndices[i + 2]);
	}
	fclose(ObjectFile);
	return 0;
}

int Object3d::WritePointListToFile(const char* FileName)
{
	FILE* ObjectFile = nullptr;
	fopen_s(&ObjectFile, FileName, "w");
	if (ObjectFile == nullptr)
		return -2;
	fprintf_s(ObjectFile, "%u %u\n", VertexNum, IndexNum/3);

	for (unsigned int i = 0; i < VertexNum; i++) {
		fprintf_s(ObjectFile, "%.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f\n",
			pVertex[i].posit[0], pVertex[i].posit[1], pVertex[i].posit[2], pVertex[i].posit[3],
			pVertex[i].color[0], pVertex[i].color[1], pVertex[i].color[2], pVertex[i].color[3]);
	}
	for (unsigned int i = 0; i < IndexNum; i += 3) {
		fprintf_s(ObjectFile, "%u %u %u\n", pIndices[i], pIndices[i + 1], pIndices[i + 2]);
	}
	fclose(ObjectFile);
	return 0;
}

int Object3d::CreateObject(ID3D11Device* pDevice)
{
	{
		// 顶点缓冲区描述符
		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.CPUAccessFlags = 0u;
		vbDesc.MiscFlags = 0u;
		vbDesc.ByteWidth = this->VertexNum * this->VertexStrides;
		vbDesc.StructureByteStride = this->VertexStrides;
		// 顶点缓冲资源描述符
		D3D11_SUBRESOURCE_DATA vbssDesc = {};
		vbssDesc.pSysMem = static_cast<void*>(&pVertex[0]);
		// 创建顶点缓冲区
		ThrowIfFailed(pDevice->CreateBuffer(&vbDesc, &vbssDesc, &pVertexBuffer));
	}
	{
		// 索引缓冲描述符
		D3D11_BUFFER_DESC inDesc = {};
		inDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		inDesc.Usage = D3D11_USAGE_DEFAULT;
		inDesc.CPUAccessFlags = 0u;
		inDesc.MiscFlags = 0u;
		inDesc.ByteWidth = this->IndexNum * this->IndexStrides;// sizeof(unsigned int);
		inDesc.StructureByteStride = this->IndexStrides;// sizeof(unsigned int);
		// 索引缓冲资源描述符
		D3D11_SUBRESOURCE_DATA inssDesc = {};
		inssDesc.pSysMem = static_cast<void*>(&pIndices[0]);
		ThrowIfFailed(pDevice->CreateBuffer(&inDesc, &inssDesc, &pIndexBuffer));
	}
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

int Object3d::CreatePointListObjectFromFile(const char* FileName, ID3D11Device* pDevice)
{
	ThrowIfFailed(LoadPointListFromFile(FileName));
	CreateObject(pDevice);

	this->ScaleTo(1.0f);
	return 0;
}

int Object3d::modify(float4x4 rotaMat, float4 transVec)
{
	for (unsigned int i = 0; i < VertexNum; ++i)
		pVertex[i].posit = pVertex[i].posit * rotaMat + transVec;
	return 0;
}

int Object3d::Update(ID3D11DeviceContext* pContext)
{
	float sinX = std::sin(Rotate[1]), cosX = std::cos(Rotate[1]);
	float sinY = std::sin(Rotate[2]), cosY = std::cos(Rotate[2]);
	float sinZ = std::sin(Rotate[3]), cosZ = std::cos(Rotate[3]);
	// 局部坐标与相机坐标相反
	// 先偏航roY，再俯仰roX，再滚动roZ
	DirectX::XMMATRIX rota = { //自旋转矩阵
		 cosY*cosZ+sinX*sinY*sinZ,  cosY*sinZ-sinX*sinY*cosZ, cosX*sinY, 0,
		-cosX*sinZ				 ,  cosX*cosZ			    , sinX	   , 0,
		-sinY*cosZ+sinX*cosY*sinZ, -sinY*sinZ-sinX*cosY*cosZ, cosX*cosY, 0,
		0, 0, 0, 1
	};
	float Scale = Translate[0];
	float dx = Translate[1], dy = Translate[2], dz = Translate[3];
	DirectX::XMMATRIX trans = { //缩放平移矩阵
		Scale,	0.0f,	0.0f,	0.0f,
		0.0f,	Scale,	0.0f,	0.0f,
		0.0f,	0.0f,	Scale,	0.0f,
		dx,		dy,		dz,		1.0f
	};
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(rota * trans);//rota * trans;

	D3D11_MAPPED_SUBRESOURCE mappedData = {};
	ThrowIfFailed(pContext->Map(pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy(mappedData.pData, &transform, sizeof(transform));
	pContext->Unmap(pConstantBuffer.Get(), 0);
	return 0;
}

int Object3d::Bind(ID3D11DeviceContext* pContext)
{
	// 绑定顶点缓冲
	// StartSlot:绑定的输入槽
	// NumBuffers:需要绑定的顶点缓冲区的数量
	// ppVertexBuffers:顶点缓冲区数组，一个二维指针，或者一个指向缓冲区的指针
	// pStrides:一个步幅数组，一个指针，第NumBuffers个表示需要绑定的顶点缓冲区的步幅
	// pOffsets:一个偏移值数组，一个指针，第NumBuffers个表示需要绑定的顶点缓冲区的偏移值
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &VertexStrides, &VertexOffsets);
	// 绑定索引缓冲
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	// 绑定常数缓冲
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	return 0;
}

int Object3d::Draw(ID3D11DeviceContext* pContext)
{
	Update(pContext);
	Bind(pContext);
	pContext->Draw(VertexNum, 0u);
	//pContext->DrawIndexed(IndexNum, 0u, 0u);
	return 0;
}

int Object3d::MoveAdd(float dx, float dy, float dz)
{
	Translate[1] += dx;
	Translate[2] += dy;
	Translate[3] += dz;
	return 0;
}

int Object3d::ScaleAdd(float dscale)
{
	Translate[0] += dscale;
	return 0;
}

int Object3d::RotateAdd(float droX, float droY, float droZ)
{
	Rotate[1] += droX;
	Rotate[2] += droY;
	Rotate[3] += droZ;
	return 0;
}

int Object3d::MoveTo(float x, float y, float z)
{
	Translate[1] = x;
	Translate[2] = y;
	Translate[3] = z;
	return 0;
}

int Object3d::ScaleTo(float scale)
{
	Translate[0] = scale;
	return 0;
}

int Object3d::RotateTo(float roX, float roY, float roZ)
{
	Rotate[1] = roX;
	Rotate[2] = roY;
	Rotate[3] = roZ;
	return 0;
}
