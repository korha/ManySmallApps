//PlayDirect
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static constexpr const DWORD g_dwDirectoryOffset = 1+(MAX_PATH-9)+1;        //12 = "opengl\Kernel.dll`" - "Main.pro`"
static constexpr const DWORD g_dwPathMargin = 19;        //"\opengl\Kernel.dll`"

//-------------------------------------------------------------------------------------------------
static inline void FCopyMemoryW(wchar_t *pDst, const wchar_t *pSrc)
{
    while ((*pDst++ = *pSrc++));
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    wchar_t wBuf[g_dwDirectoryOffset + MAX_PATH-10];        //"Kernel.dll"
    wchar_t *const pPath = wBuf+1;
    const DWORD dwLen = GetModuleFileNameW(nullptr, pPath, MAX_PATH+1);
    if (dwLen >= 4 && dwLen < MAX_PATH)
    {
        wchar_t *pDelim = pPath+dwLen;
        do
        {
            if (*--pDelim == L'\\')
                break;
        } while (pDelim > pPath);
        if (pDelim >= pPath+2 && pDelim <= pPath+MAX_PATH-g_dwPathMargin)
        {
            wchar_t *const pDirectory = wBuf+g_dwDirectoryOffset;
            *wBuf = L'"';
            *++pDelim = L'\0';
            FCopyMemoryW(pDirectory, pPath);
            FCopyMemoryW(pDirectory+(pDelim-pPath), L"opengl\\");
            FCopyMemoryW(pDelim, L"Main.pro\"");

            PROCESS_INFORMATION pi;
            STARTUPINFO si;
            DWORD dwSize = sizeof(STARTUPINFO);
            BYTE *pDst = static_cast<BYTE*>(static_cast<void*>(&si));
            while (dwSize--)
                *pDst++ = '\0';
            si.cb = sizeof(STARTUPINFO);
            if (CreateProcessW(nullptr, wBuf, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT, nullptr, pDirectory, &si, &pi))
            {
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
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
