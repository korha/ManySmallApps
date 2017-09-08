//Runner
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static constexpr const DWORD g_dwMaxSize = MAX_PATH*4;

//-------------------------------------------------------------------------------------------------
static inline bool FCompareMemoryW(const wchar_t *pBuf1, const wchar_t *pBuf2)
{
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

//-------------------------------------------------------------------------------------------------
static const wchar_t * FNextArg(wchar_t **wCmdLine__)
{
    wchar_t *wCmdLine = *wCmdLine__;
    while (*wCmdLine == L' ' || *wCmdLine == L'\t')
        ++wCmdLine;
    if (*wCmdLine != L'\0')
    {
        wchar_t *wArg = wCmdLine;
        if (*wCmdLine++ == L'"')
        {
            while (*wCmdLine != L'\"')
            {
                if (*wCmdLine == L'\0')
                    return nullptr;
                ++wCmdLine;
            }
            if (wCmdLine[1] != L' ' && wCmdLine[1] != L'\t' && wCmdLine[1] != L'\0')
                return nullptr;
            ++wArg;
        }
        else
            while (*wCmdLine != L' ' && *wCmdLine != L'\t' && *wCmdLine != L'\0')
                ++wCmdLine;

        *wCmdLine__ = (*wCmdLine == L'\0' ? wCmdLine : (*wCmdLine++ = L'\0', wCmdLine));
        return wArg;
    }
    return nullptr;
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    wchar_t *wCmdLine = GetCommandLineW();
    const wchar_t *wArg = nullptr;
    if (wCmdLine && FNextArg(&wCmdLine) && (wArg = FNextArg(&wCmdLine)))
    {
        wchar_t wBuf[g_dwMaxSize+1];
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        DWORD dwSize = sizeof(STARTUPINFO);
        BYTE *pDst = static_cast<BYTE*>(static_cast<void*>(&si));
        while (dwSize--)
            *pDst++ = '\0';
        si.cb = sizeof(STARTUPINFO);
        do
        {
            const HANDLE hFile = CreateFileW(wArg, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                DWORD dwSize = 0;
                LARGE_INTEGER iFileSize;
                if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.HighPart == 0 &&
                        iFileSize.LowPart >= sizeof(wchar_t) && iFileSize.LowPart <= g_dwMaxSize*sizeof(wchar_t) &&
                        iFileSize.LowPart%sizeof(wchar_t) == 0)
                    dwSize = ReadFile(hFile, wBuf, iFileSize.LowPart, &dwSize, nullptr) && dwSize == iFileSize.LowPart;
                if (CloseHandle(hFile) && dwSize && *wBuf)
                {
                    wBuf[iFileSize.LowPart] = L'\0';
                    const wchar_t *wCurDir = nullptr;
                    wchar_t *wIt = wBuf;
                    while (*wIt)
                    {
                        if (*wIt == L'\n')
                        {
                            *wIt = L'\0';
                            if (*++wIt)
                            {
                                wCurDir = wIt;
                                while (*wIt)
                                {
                                    if (*wIt == L'\n')
                                    {
                                        *wIt++ = L'\0';
                                        si.dwFlags = 0;
                                        si.wShowWindow = 0;
                                        DWORD dwCreationFlags = CREATE_UNICODE_ENVIRONMENT;
                                        if (FCompareMemoryW(wIt, L"MINIMIZED"))
                                        {
                                            si.dwFlags = STARTF_USESHOWWINDOW;
                                            si.wShowWindow = SW_SHOWMINNOACTIVE;
                                        }
                                        else if (FCompareMemoryW(wIt, L"MAXIMIZED"))
                                        {
                                            si.dwFlags = STARTF_USESHOWWINDOW;
                                            si.wShowWindow = SW_MAXIMIZE;
                                        }
                                        else if (FCompareMemoryW(wIt, L"HIDDEN"))
                                            dwCreationFlags = CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW;
                                        else if (FCompareMemoryW(wIt, L"NORMAL_NOT_ACTIVATE"))
                                        {
                                            si.dwFlags = STARTF_USESHOWWINDOW;
                                            si.wShowWindow = SW_SHOWNOACTIVATE;
                                        }
                                        else if (FCompareMemoryW(wIt, L"MINIMIZED_ACTIVATE"))
                                        {
                                            si.dwFlags = STARTF_USESHOWWINDOW;
                                            si.wShowWindow = SW_SHOWMINIMIZED;
                                        }
                                        else if (FCompareMemoryW(wIt, L"MINIMIZED_ACTIVATE_NEXT"))
                                        {
                                            si.dwFlags = STARTF_USESHOWWINDOW;
                                            si.wShowWindow = SW_MINIMIZE;
                                        }
                                        if (CreateProcessW(nullptr, wBuf, nullptr, nullptr, FALSE, dwCreationFlags, nullptr, wCurDir, &si, &pi))
                                        {
                                            CloseHandle(pi.hThread);
                                            CloseHandle(pi.hProcess);
                                        }
                                        break;
                                    }
                                    ++wIt;
                                }
                            }
                            break;
                        }
                        ++wIt;
                    }
                }
            }
        } while ((wArg = FNextArg(&wCmdLine)));
    }
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
