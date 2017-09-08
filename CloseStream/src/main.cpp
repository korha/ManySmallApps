//CloseStream
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#include <windows.h>

static constexpr const wchar_t *const g_wGuidClass = L"App::453988b7-21af-daca-ae4e-a8a46306df75";

//-------------------------------------------------------------------------------------------------
static inline bool FCompareMemoryW(const wchar_t *pBuf1)
{
    const wchar_t *pBuf2 = L"1:0:1:";
    DWORD dwSize = 6;
    while (dwSize--)
        if (*pBuf1++ != *pBuf2++)
            return false;
    return true;
}

//-------------------------------------------------------------------------------------------------
static DWORD WINAPI ThreadStartRoutine(LPVOID lpThreadParameter)
{
    wchar_t wBuf[7];        //"1:0:1:?"
    SOCKET sockAccept;
    while ((sockAccept = accept(reinterpret_cast<SOCKET>(lpThreadParameter), nullptr, nullptr)) != INVALID_SOCKET)
    {
        HWND hWnd = nullptr;
        while ((hWnd = FindWindowExW(nullptr, hWnd, L"MediaPlayerClassicW", nullptr)))
            if (GetWindowTextW(hWnd, wBuf, 7) == 6 && FCompareMemoryW(wBuf))
                PostMessageW(hWnd, WM_CLOSE, 0, 0);
        closesocket(sockAccept);
    }
    return 0;
}

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
void FMain()
{
    const wchar_t *wError = L"Incorrect parameter";
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
                wchar_t *wArg = wCmdLine;
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
                wError = nullptr;

                if (!(wArg[0] == L'/' && wArg[1] == L'q' && wArg[2] == L'\0'))
                {
                    if (
                            wArg[ 0] >= L'0' && wArg[ 0] <= L'2' &&
                            wArg[ 1] >= L'0' && wArg[ 1] <= L'9' &&
                            wArg[ 2] >= L'0' && wArg[ 2] <= L'9' &&
                            wArg[ 3] == L'.' &&
                            wArg[ 4] >= L'0' && wArg[ 4] <= L'2' &&
                            wArg[ 5] >= L'0' && wArg[ 5] <= L'9' &&
                            wArg[ 6] >= L'0' && wArg[ 6] <= L'9' &&
                            wArg[ 7] == L'.' &&
                            wArg[ 8] >= L'0' && wArg[ 8] <= L'2' &&
                            wArg[ 9] >= L'0' && wArg[ 9] <= L'9' &&
                            wArg[10] >= L'0' && wArg[10] <= L'9' &&
                            wArg[11] == L'.' &&
                            wArg[12] >= L'0' && wArg[12] <= L'2' &&
                            wArg[13] >= L'0' && wArg[13] <= L'9' &&
                            wArg[14] >= L'0' && wArg[14] <= L'9' &&
                            wArg[15] == L':' &&
                            wArg[16] >= L'0' && wArg[16] <= L'6' &&
                            wArg[17] >= L'0' && wArg[17] <= L'9' &&
                            wArg[18] >= L'0' && wArg[18] <= L'9' &&
                            wArg[19] >= L'0' && wArg[19] <= L'9' &&
                            wArg[20] >= L'0' && wArg[20] <= L'9' &&
                            wArg[21] == L'\0')
                    {
                        const u_long iOctet1 =                                                (wArg[ 0]-L'0')*100 + (wArg[ 1]-L'0')*10 + (wArg[ 2]-L'0');
                        const u_long iOctet2 =                                                (wArg[ 4]-L'0')*100 + (wArg[ 5]-L'0')*10 + (wArg[ 6]-L'0');
                        const u_long iOctet3 =                                                (wArg[ 8]-L'0')*100 + (wArg[ 9]-L'0')*10 + (wArg[10]-L'0');
                        const u_long iOctet4 =                                                (wArg[12]-L'0')*100 + (wArg[13]-L'0')*10 + (wArg[14]-L'0');
                        const u_long iPort   = (wArg[16]-L'0')*10000 + (wArg[17]-L'0')*1000 + (wArg[18]-L'0')*100 + (wArg[19]-L'0')*10 + (wArg[20]-L'0');
                        if (iOctet1 <= 255 && iOctet2 <= 255 && iOctet3 <= 255 && iOctet4 <= 255 && iPort > 0 && iPort <= 65535)
                        {
                            WSADATA wsaData;
                            if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR)
                            {
                                const SOCKET sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                                if (sockListen != INVALID_SOCKET)
                                {
                                    HANDLE hThread = nullptr;
                                    sockaddr_in sockAddrIn;
                                    sockAddrIn.sin_family = AF_INET;
                                    sockAddrIn.sin_addr.S_un.S_un_b.s_b1 = iOctet1;
                                    sockAddrIn.sin_addr.S_un.S_un_b.s_b2 = iOctet2;
                                    sockAddrIn.sin_addr.S_un.S_un_b.s_b3 = iOctet3;
                                    sockAddrIn.sin_addr.S_un.S_un_b.s_b4 = iOctet4;
                                    sockAddrIn.sin_port = (iPort & 0xFF) << 8 | iPort >> 8;        //to network byte order
                                    if (bind(sockListen, static_cast<const sockaddr*>(static_cast<const void*>(&sockAddrIn)), sizeof(SOCKADDR_IN)) == NO_ERROR)
                                    {
                                        if (listen(sockListen, 1) == NO_ERROR)
                                        {
                                            WNDCLASSEXW wndCl;
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
                                                if (CreateWindowExW(0, g_wGuidClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr))
                                                {
                                                    if ((hThread = CreateThread(nullptr, 0, ThreadStartRoutine, reinterpret_cast<LPVOID>(sockListen), 0, nullptr)))
                                                    {
                                                        MSG msg;
                                                        while (GetMessageW(&msg, nullptr, 0, 0) > 0)
                                                            DispatchMessageW(&msg);
                                                    }
                                                    else
                                                        wError = L"CreateThread fail";
                                                }
                                                else
                                                    wError = L"CreateWindowExW fail";
                                                UnregisterClassW(g_wGuidClass, GetModuleHandleW(nullptr));
                                            }
                                            else
                                                wError = L"RegisterClassW fail";
                                        }
                                        else
                                            wError = L"Listen fail";
                                    }
                                    else
                                        wError = L"Bind fail";
                                    closesocket(sockListen);
                                    if (hThread)
                                    {
                                        WaitForSingleObject(hThread, 10000);
                                        CloseHandle(hThread);
                                    }
                                }
                                else
                                    wError = L"Socket fail";
                                WSACleanup();
                            }
                            else
                                wError = L"WSAStartup fail";
                        }
                        else
                            wError = L"Invalid argument";
                    }
                    else
                        wError = L"Invalid argument";
                }
            }
        }
    }
    if (wError)
        MessageBoxW(nullptr, wError, L"CloseStream", MB_ICONERROR);
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
