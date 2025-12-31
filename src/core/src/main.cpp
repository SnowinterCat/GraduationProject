#include <gp/config.hpp>

#include <future>
#include "WindowsAPI.h"
#include "RenderEngine.h"

#include "DebugException.h"

constexpr int WinWide = 1280;
constexpr int WinHigh = 720;

Application app;
Graphics    graphic;

LRESULT WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

_Use_decl_annotations_ int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                          LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    if (!app.Register(hInstance, WinProc)) {
        MessageBox(NULL, L"窗口注册失败", NULL, MB_OK);
        return -1;
    }
    if (app.CreatingWindow(hInstance, WinWide, WinHigh) < 0) {
        MessageBox(NULL, L"创建窗口失败", NULL, MB_OK);
        return -1;
    }
    // 消息循环
    MSG msg;
    while (true) {
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (msg.message == WM_QUIT)
            break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    try {
        static std::future<bool> matchThread;
        static bool              stopFlag = false;
        if (matchThread._Is_ready() == true) {
            bool flag = matchThread.get();
            graphic.RefreshObject(1);
        }

        switch (message) {
        case WM_PAINT:
            graphic.Render();
            break;
        case WM_LBUTTONDOWN:
            break;
        case WM_KEYDOWN: {
            switch (wParam) {
            case 'P':
                if (matchThread.valid() == false)
                    matchThread = std::async(&Graphics::match, &graphic, std::ref(stopFlag));
                break;
            case 'S':
                graphic.Obj[1].WritePointListToFile("10.pcr");
                break;
            default:
                break;
            }
            break;
        }
        case WM_CREATE:
            ThrowIfFailed(graphic.init(hWnd, 0));
            break;
        case WM_CLOSE:
            stopFlag = true;
            if (matchThread.valid() == true)
                matchThread.wait();
            PostQuitMessage(0);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }
    catch (const DebugException &e) {
        MessageBoxA(nullptr, e.what(), "Debug Exception", MB_OK);
    }
    catch (const std::exception &e) {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK);
    }
    catch (...) {
        MessageBoxA(nullptr, "No details", "Unknow Exception", MB_OK);
    }
    PostQuitMessage(-1);
    return -1;
}