//TorBrowserExternal
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//-------------------------------------------------------------------------------------------------
static inline void FCopyMemoryW(wchar_t *pDst, const wchar_t *pSrc)
{
    while ((*pDst++ = *pSrc++));
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    wchar_t wBuf[MAX_PATH];
    const DWORD dwLen = GetModuleFileNameW(nullptr, wBuf, MAX_PATH+1-4);        //".txt"
    if (dwLen >= 6 && dwLen < MAX_PATH-4)        //".txt"
    {
        wchar_t *pDelim = wBuf+dwLen;
        do
        {
            if (*--pDelim == L'\\')
                break;
        } while (pDelim > wBuf);
        if (pDelim >= wBuf+2)
        {
            ++pDelim;
            const wchar_t wSave = *pDelim;
            *pDelim = L'\0';
            if (SetCurrentDirectoryW(wBuf))
            {
                *pDelim = wSave;
                FCopyMemoryW(wBuf+dwLen, L".txt");
                const HANDLE hFile = CreateFileW(wBuf, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    DWORD dwSize = 0;
                    LARGE_INTEGER iFileSize;
                    if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.HighPart == 0 && iFileSize.LowPart &&
                            iFileSize.LowPart < MAX_PATH*sizeof(wchar_t) && iFileSize.LowPart%sizeof(wchar_t) == 0)
                        dwSize = ReadFile(hFile, wBuf, iFileSize.LowPart, &dwSize, nullptr) && dwSize == iFileSize.LowPart;
                    CloseHandle(hFile);
                    if (dwSize)
                    {
                        wBuf[iFileSize.LowPart/sizeof(wchar_t)] = L'\0';
                        const DWORD dwAttrib = GetFileAttributesW(wBuf);
                        if ((dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) || CreateDirectoryW(wBuf, nullptr))
                            if (wchar_t *wCmdLine = GetCommandLineW())
                                while (*wCmdLine)
                                    if (*wCmdLine++ == L'^')
                                    {
                                        wchar_t *const wNewCmdLine = wCmdLine;
                                        while (*wCmdLine)
                                        {
                                            if (*wCmdLine == L'\'')
                                                *wCmdLine = L'"';
                                            else if (*wCmdLine == L'^')
                                            {
                                                *wCmdLine = L'\0';
                                                PROCESS_INFORMATION pi;
                                                STARTUPINFO si;
                                                DWORD dwSize = sizeof(STARTUPINFO);
                                                BYTE *pDst = static_cast<BYTE*>(static_cast<void*>(&si));
                                                while (dwSize--)
                                                    *pDst++ = '\0';
                                                si.cb = sizeof(STARTUPINFO);
                                                if (CreateProcessW(nullptr, wNewCmdLine, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT, nullptr, wBuf, &si, &pi))
                                                {
                                                    CloseHandle(pi.hThread);
                                                    CloseHandle(pi.hProcess);
                                                }
                                                break;
                                            }
                                            ++wCmdLine;
                                        }
                                        break;
                                    }
                    }
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
