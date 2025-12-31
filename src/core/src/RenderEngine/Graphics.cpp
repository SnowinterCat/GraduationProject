#include "Graphics.h"

Graphics::Graphics() :Wide(0), High(0), GPUnum(0), GPUInformation(), ViewPort()
{
    // 创建Factory
    ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&pFactory)));
    // 枚举所有GPU设备
    for (GPUnum = 0; GPUnum < 8 && pFactory->EnumAdapters(GPUnum, &pAdapter) != DXGI_ERROR_NOT_FOUND; GPUnum++)
    {
        DXGI_ADAPTER_DESC desc = {};
        pAdapter->GetDesc(&desc);
        snprintf(GPUInformation[GPUnum], BufferLens, "%ls", desc.Description);
    }
/*  for (UINT i = 0; i < GPUnum; i++) {
        snprintf(DebugBuffer, DebugLens, "GPU%u : %s\n", i, GPUInformation[i]);
        OutputDebugStringA(DebugBuffer);
    }*/
}

int Graphics::init(HWND hWnd, UINT GPUid)
{
    if (hWnd == nullptr)
        return -1;
    if (pFactory == nullptr)
        return -1;
    // 
    RECT rt = {};
    GetClientRect(hWnd, &rt);
    Wide = rt.right - rt.left;
    High = rt.bottom - rt.top;

    // 禁用IDXGIFactory的Alt+Enter键的全屏功能
    ThrowIfFailed(pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    // 创建交换链描述符
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = 0;
    swapChainDesc.BufferDesc.Height = 0;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//指示如何拉伸图像以适应显示器分辨率
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//指定扫描线顺序
    // 指定多重采样参数
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1; // 后台缓冲数
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    // 创建GPU: Device, SwapChain, DeviceContext
    D3D_FEATURE_LEVEL nowFeatureLevel = {};
    if (GPUid == 0)
    {
        // 未指定GPUid则使用默认GPU创建GPU设备
        ThrowIfFailed(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
            &swapChainDesc, &pSwapChain,
            &pDevice, &nowFeatureLevel, &pContext));
    }
    else
    {
        // 选择指定GPU
        if (pFactory->EnumAdapters(GPUid - 1, &pAdapter) == DXGI_ERROR_NOT_FOUND)
            return -1;
        // 使用指定GPU创建GPU设备
        ThrowIfFailed(D3D11CreateDeviceAndSwapChain(pAdapter.Get(), D3D_DRIVER_TYPE_HARDWARE,
            nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
            &swapChainDesc, &pSwapChain,
            &pDevice, &nowFeatureLevel, &pContext));
    }

#ifdef _DEBUG
    char DebugBuffer[DebugLens] = {};
    if (GPUid == 0)
        snprintf(DebugBuffer, DebugLens, "选择GPU : %s\nFeature Level : 0x%x\n", GPUInformation[0], nowFeatureLevel);
    else
        snprintf(DebugBuffer, DebugLens, "选择GPU : %s\nFeature Level : 0x%x\n", GPUInformation[GPUid - 1], nowFeatureLevel);
    OutputDebugStringA(DebugBuffer);
#endif // _DEBUG

    {
        // 创建渲染目标视图
        Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
        ThrowIfFailed(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
        ThrowIfFailed(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRTV));
        // 创建深度测试图层
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
        D3D11_TEXTURE2D_DESC descDepth = {};
        descDepth.Width = Wide;
        descDepth.Height = High;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
        ThrowIfFailed(pDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil));
        // 深度模板视图描述符
        D3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc = {};
        DSVdesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DSVdesc.Texture2D.MipSlice = 0;
        // 创建深度模板视图
        pDevice->CreateDepthStencilView(pDepthStencil.Get(), &DSVdesc, &pDSV);
        // 绑定渲染目标视图和深度模板视图
        pContext->OMSetRenderTargets(1u, pRTV.GetAddressOf(), pDSV.Get());

        // 创建深度测试状态
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.StencilEnable = false;
        pDevice->CreateDepthStencilState(&dsDesc, &pDSState);
        // 绑定深度测试状态
        pContext->OMSetDepthStencilState(pDSState.Get(), 1);
    }
    {
        // 创建观察窗口
        ViewPort.TopLeftX = 0;
        ViewPort.TopLeftY = 0;
        ViewPort.Width = (float)Wide;
        ViewPort.Height = (float)High;
        ViewPort.MinDepth = 0;
        ViewPort.MaxDepth = 1;
        // 绑定视图窗口
        pContext->RSSetViewports(1u, &ViewPort);
    }
    {
        // 加载顶点着色器
        Microsoft::WRL::ComPtr<ID3DBlob> VS, PS;
        VS = CompileShaderFromFile(L"Shader/Shaders.hlsl", nullptr, "VSMain", "vs_4_0");
        ThrowIfFailed(pDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &pVertexShader));
        // 加载像素着色器
        PS = CompileShaderFromFile(L"Shader/Shaders.hlsl", nullptr, "PSMain", "ps_4_0");
        ThrowIfFailed(pDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &pPixelShader));
        // 绑定顶点着色器和像素着色器
        pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
        pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

        // 加载输入布局（输入格式）
        D3D11_INPUT_ELEMENT_DESC ieDesc[] = {
            { "POSITION" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        ThrowIfFailed(pDevice->CreateInputLayout(ieDesc, (int)std::size(ieDesc), VS->GetBufferPointer(), VS->GetBufferSize(), &pInputLayout));
        // 绑定输入布局（输入格式）
        pContext->IASetInputLayout(pInputLayout.Get());
    }

    // 初始化主相机
    MainCamera.init(pDevice.Get(), 0.5f * DirectX::XM_PI, (float)Wide, (float)High, 1.0f, 100.0f);
    MainCamera.MoveTo(0.0f, 0.0f, -3.0f);

    /* 绑定顶点图元拓扑为：三角形列表
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

    /* 绑定顶点图元拓扑为：顶点列表*/
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    ThrowIfFailed(GetLastError());

    load3DResource();
    return 0;
}

void Graphics::load3DResource()
{
    // 读取点云数据
    ThrowIfFailed(Obj[0].CreatePointListObjectFromFile("PointCloud/9.pc", pDevice.Get()));
    ThrowIfFailed(Obj[1].CreatePointListObjectFromFile("PointCloud/10.pc", pDevice.Get()));
    //Obj[0].MoveTo(0.0f, -1.0f, 2.0f);
    //registration(Obj[0], Obj[1]);
    //Obj[1].RotateAdd(0.0f, 3.1415926f, 0.0f);
    Obj[0].MoveAdd(0.0f, 0.5f, 0.0f);
    Obj[1].MoveAdd(0.0f, 0.5f, 0.0f);

    //Obj[0].RotateTo(acos(-1.0f) / 2, 0.0f, 0.0f);
    /*float PI = acos(-1.0f);
    float sin60 = sin(-PI * 80 / 180), cos60 = cos(-PI * 80 / 180);
    float4x4 rota;
    rota << cos60, 0.0f, sin60, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin60, 0.0f, cos60, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    float4 trans;
    trans << -0.1f, 0.0f, -0.05f;*/
    //Obj[1].modify(rota, trans);
    //Obj[1].CreateObject(pDevice.Get());
}

void Graphics::Render()
{
    StartFrame();
    DoFrame();
    EndFrame();
}

bool Graphics::match(bool& stopFlag)
{
    float4x4 rotaMat;
    float4 transVec;
    bool flag = registration(Obj[0], Obj[1], rotaMat, transVec, stopFlag);
    if (stopFlag == true)
        return false;
    Obj[1].modify(rotaMat, transVec);
    return true;
}

bool Graphics::RefreshObject(int index)
{
    Obj[index].CreateObject(pDevice.Get());
    return true;
}

void Graphics::StartFrame()
{
    static const float Framecolor[] = { 0.0f,0.0f,0.0f,1.0f };
    pContext->ClearRenderTargetView(pRTV.Get(), Framecolor);
    pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::DoFrame()
{
    float theta = GraphicTimer.AllTime();//0.0f;3.1415926 / 4.0f * 2.0f; // 0.78539815;// 
    float R = 1.5f;
    float Rsin = std::sin(theta) * R;
    float Rcos = std::cos(theta) * R;

    MainCamera.MoveTo(Rsin, 1.5f, -Rcos);
    MainCamera.RotateTo(0.0f, theta, 0.0f);

    MainCamera.Update(pContext.Get());
    MainCamera.Bind(pContext.Get());

    //pContext->DrawIndexed(Obj[0].IndexNum, 0u, 0u);
    int ObjectNum = 2;
    for (int i = 0; i < ObjectNum; i++) {
        /*if (i == 0)
            Obj[i].RotateAdd(0.0f, 0.0f, 0.01f);*/
        Obj[i].Draw(pContext.Get());
    }
}

void Graphics::EndFrame()
{
    pSwapChain->Present(1u, 0u);
}