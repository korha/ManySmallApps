//KeyBlock
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static const wchar_t *const g_wGuidClass = L"App::dfef1c68-654e-66f8-add4-2d21f9cad11c";

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_ENDSESSION)
    {
        SendMessageW(hWnd, WM_CLOSE, 0, 0);
        return 0;
    }
    if (uMsg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return (nCode == HC_ACTION) ? 1 : CallNextHookEx(nullptr, nCode, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    const HINSTANCE hInst = GetModuleHandleW(nullptr);
    WNDCLASSEX wndCl;
    wndCl.cbSize = sizeof(WNDCLASSEX);
    wndCl.style = 0;
    wndCl.lpfnWndProc = WindowProc;
    wndCl.cbClsExtra = 0;
    wndCl.cbWndExtra = 0;
    wndCl.hInstance = hInst;
    wndCl.hIcon = static_cast<HICON>(LoadImageW(hInst, L"IDI_ICON1", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
    wndCl.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wndCl.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE+1);
    wndCl.lpszMenuName = nullptr;
    wndCl.lpszClassName = g_wGuidClass;
    wndCl.hIconSm = static_cast<HICON>(LoadImageW(hInst, L"IDI_ICON1", IMAGE_ICON, 16, 16, 0));

    if (RegisterClassExW(&wndCl))
    {
        RECT rect;
        if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0) &&
                CreateWindowExW(0, g_wGuidClass, L"KeyBlock", (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX),
                                (rect.right-rect.left)/2-250/2, (rect.bottom-rect.top)/2-100/2, 250, 100, nullptr, nullptr, hInst, nullptr))
            if (const HHOOK hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInst, 0))
            {
                MSG msg;
                while (GetMessageW(&msg, nullptr, 0, 0) > 0)
                    DispatchMessageW(&msg);
                UnhookWindowsHookEx(hHook);
            }
        UnregisterClassW(g_wGuidClass, hInst);
    }
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
