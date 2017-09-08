//CpuLoad
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <commctrl.h>

static constexpr const wchar_t *const g_wGuidClass = L"App::14c0c92b-4f51-c5c2-4a37-e127b1eda99c";
static constexpr const int g_iMaxThreads = 128;
static volatile bool g_bActive = false;

//-------------------------------------------------------------------------------------------------
static DWORD WINAPI ThreadProc(LPVOID)
{
    while (g_bActive);
    return 0;
}

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    enum
    {
        eBtn = 1
    };

    static HICON hImgStart, hImgStop;
    static HWND hWndEdit, hWndUpDown, hWndBtn;
    static HFONT hFont;
    static HANDLE *pThreads;
    static LRESULT iNumOfThreads;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        const CREATESTRUCT *const pCreateStruct = reinterpret_cast<const CREATESTRUCT*>(lParam);
        const HINSTANCE hInst = pCreateStruct->hInstance;
        if ((hImgStart = static_cast<HICON>(LoadImageW(hInst, L"IDI_ICON2", IMAGE_ICON, 16, 16, 0))))
            if ((hImgStop = static_cast<HICON>(LoadImageW(hInst, L"IDI_ICON3", IMAGE_ICON, 16, 16, 0))))
                if (const HWND hWndStatic = CreateWindowExW(0, WC_STATIC, L"Threads:", WS_CHILD | WS_VISIBLE, 65, 14, 52, 14, hWnd, nullptr, hInst, nullptr))
                    if ((hWndEdit = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDIT, nullptr, WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_NUMBER, 118, 11, 41, 20, hWnd, nullptr, hInst, nullptr)))
                        if ((hWndUpDown = CreateWindowExW(0, UPDOWN_CLASS, nullptr, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_NOTHOUSANDS, 0, 0, 0, 0, hWnd, nullptr, hInst, nullptr)))
                            if ((hWndBtn = CreateWindowExW(0, WC_BUTTON, L"Start", WS_CHILD | WS_VISIBLE, 84, 40, 75, 22, hWnd, reinterpret_cast<HMENU>(eBtn), hInst, nullptr)))
                            {
                                NONCLIENTMETRICS nonClientMetrics;
                                nonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
                                if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nonClientMetrics, 0) &&
                                        (hFont = CreateFontIndirectW(&nonClientMetrics.lfMessageFont)))
                                {
                                    SYSTEM_INFO sysInfo;
                                    GetNativeSystemInfo(&sysInfo);
                                    if (sysInfo.dwNumberOfProcessors > 0 && sysInfo.dwNumberOfProcessors <= 128)
                                    {
                                        pThreads = static_cast<HANDLE*>(pCreateStruct->lpCreateParams);

                                        SendMessageW(hWndUpDown, UDM_SETRANGE, 0, MAKELPARAM(sysInfo.dwNumberOfProcessors, 1));
                                        SendMessageW(hWndUpDown, UDM_SETPOS, 0, 1);

                                        SendMessageW(hWndStatic, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), FALSE);
                                        SendMessageW(hWndEdit, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), FALSE);
                                        SendMessageW(hWndUpDown, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), FALSE);
                                        SendMessageW(hWndBtn, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), FALSE);

                                        SendMessageW(hWndBtn, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(hImgStart));
                                        return 0;
                                    }
                                }
                            }
        return -1;
    }
    case WM_COMMAND:
    {
        if (wParam == eBtn)
        {
            if (g_bActive)
            {
                g_bActive = false;
                SetWindowTextW(hWndBtn, L"Start");
                SendMessageW(hWndBtn, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(hImgStart));
                for (HANDLE *hIt = pThreads, *hEnd = pThreads+iNumOfThreads; hIt < hEnd; ++hIt)
                    if (*hIt)
                    {
                        WaitForSingleObject(*hIt, INFINITE);
                        CloseHandle(*hIt);
                    }
                EnableWindow(hWndEdit, TRUE);
                EnableWindow(hWndUpDown, TRUE);
            }
            else
            {
                iNumOfThreads = SendMessageW(hWndUpDown, UDM_GETPOS, 0, 0);
                if (iNumOfThreads > 0 && iNumOfThreads <= g_iMaxThreads)
                {
                    EnableWindow(hWndUpDown, FALSE);
                    EnableWindow(hWndEdit, FALSE);
                    SetWindowTextW(hWndBtn, L"Stop");
                    SendMessageW(hWndBtn, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(hImgStop));
                    g_bActive = true;
                    for (HANDLE *hIt = pThreads, *hEnd = pThreads+iNumOfThreads; hIt < hEnd; ++hIt)
                        *hIt = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
                }
                else
                    SendMessageW(hWndUpDown, UDM_SETPOS, 0, 1);
            }
        }
        return 0;
    }
    case WM_ENDSESSION:
    {
        SendMessageW(hWnd, WM_CLOSE, 0, 0);
        return 0;
    }
    case WM_DESTROY:
    {
        if (hImgStart)
            DestroyIcon(hImgStart);
        if (hImgStop)
            DestroyIcon(hImgStop);
        if (hFont)
            DeleteObject(hFont);
        if (g_bActive)
        {
            g_bActive = false;
            for (HANDLE *hIt = pThreads, *hEnd = pThreads+iNumOfThreads; hIt < hEnd; ++hIt)
                if (*hIt)
                {
                    WaitForSingleObject(*hIt, INFINITE);
                    CloseHandle(*hIt);
                }
        }
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    INITCOMMONCONTROLSEX initComCtrlEx;
    initComCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    initComCtrlEx.dwICC = ICC_STANDARD_CLASSES;
    if (InitCommonControlsEx(&initComCtrlEx) == TRUE)
    {
        RECT rect; rect.left = 0; rect.top = 0; rect.right = 244; rect.bottom = 75;
        if (AdjustWindowRectEx(&rect, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX), FALSE, 0))
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
                RECT rectArea;
                if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &rectArea, 0))
                {
                    rect.right -= rect.left;
                    rect.bottom -= rect.top;
                    HANDLE hThreads[g_iMaxThreads];
                    if (CreateWindowExW(0, g_wGuidClass, L"CpuLoad", (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX),
                                        (rectArea.right-rectArea.left)/2-rect.right/2, (rectArea.bottom-rectArea.top)/2-rect.bottom/2, rect.right, rect.bottom, nullptr, nullptr, hInst, &hThreads))
                    {
                        MSG msg;
                        while (GetMessageW(&msg, nullptr, 0, 0) > 0)
                        {
                            TranslateMessage(&msg);
                            DispatchMessageW(&msg);
                        }
                    }
                }
                UnregisterClassW(g_wGuidClass, hInst);
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
