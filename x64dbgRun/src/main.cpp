//x64dbgRun
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static constexpr const DWORD g_dwMinExeSize = 1536;

//-------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
inline T1 pointer_cast(T2 *pSrc)
{return static_cast<T1>(static_cast<void*>(pSrc));}

template<typename T1, typename T2>
inline T1 pointer_cast(const T2 *pSrc)
{return static_cast<T1>(static_cast<const void*>(pSrc));}

static inline void FCopyMemoryW(wchar_t *pDst, const wchar_t *pSrc)
{
    while ((*pDst++ = *pSrc++));
}

static inline void FCopyMemoryWEnd(wchar_t *pDst, const wchar_t *pSrc)
{
    while (*pDst)
        ++pDst;
    while ((*pDst++ = *pSrc++));
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
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

                const wchar_t *wIt = wArg;
                while (*wIt++);
                const DWORD dwArgLen = wIt-wArg-1;
                if (dwArgLen >= 4 && dwArgLen < MAX_PATH)
                {
                    const HANDLE hFile = CreateFileW(wArg, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                    if (hFile != INVALID_HANDLE_VALUE)
                    {
                        LARGE_INTEGER iFileSize;
                        if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.QuadPart >= g_dwMinExeSize)
                        {
                            alignas(unsigned long) BYTE btBuf[(1+MAX_PATH+1 + 1+MAX_PATH+1 + MAX_PATH-11)*sizeof(wchar_t)];        //"x96_dbg.exe"
                            DWORD dwSize;
                            dwSize = ReadFile(hFile, btBuf, g_dwMinExeSize, &dwSize, nullptr) && dwSize == g_dwMinExeSize;
                            CloseHandle(hFile);
                            if (dwSize)
                            {
                                const IMAGE_DOS_HEADER *const pImgDosHeader = pointer_cast<const IMAGE_DOS_HEADER*>(btBuf);
                                if (pImgDosHeader->e_magic == IMAGE_DOS_SIGNATURE &&
                                        pImgDosHeader->e_lfanew >= 8 && pImgDosHeader->e_lfanew <= 1024 && pImgDosHeader->e_lfanew%sizeof(DWORD) == 0)
                                {
                                    const IMAGE_NT_HEADERS *const pImgNtHeaders = pointer_cast<const IMAGE_NT_HEADERS*>(btBuf + pImgDosHeader->e_lfanew);
                                    if (pImgNtHeaders->Signature == IMAGE_NT_SIGNATURE)
                                        if (const wchar_t *const wAppend = pImgNtHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ? L"x32\\x32dbg.exe\" \"" :
                                                (pImgNtHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 ? L"x64\\x64dbg.exe\" \"" : nullptr))
                                        {
                                            wchar_t *const wBuf = pointer_cast<wchar_t*>(btBuf)+1;
                                            dwSize = GetModuleFileNameW(nullptr, wBuf, MAX_PATH+1);
                                            if (dwSize >= 4 && dwSize < MAX_PATH)
                                            {
                                                wchar_t *pDelim = wBuf+dwSize;
                                                do
                                                {
                                                    if (*--pDelim == L'\\')
                                                        break;
                                                } while (pDelim > wBuf);
                                                if (pDelim >= wBuf+2 && pDelim <= wBuf+MAX_PATH-16)        //"\x96\x96dbg.exe`"
                                                {
                                                    wBuf[-1] = L'"';
                                                    ++pDelim;
                                                    const wchar_t wTemp = *pDelim;
                                                    *pDelim = L'\0';
                                                    wchar_t *const pDirectory = wBuf + 1+MAX_PATH+1 + 1+MAX_PATH+1;
                                                    FCopyMemoryW(pDirectory, wBuf);
                                                    FCopyMemoryWEnd(pDirectory, wAppend[1] == L'3' ? L"x32\\" : L"x64\\");
                                                    *pDelim = wTemp;

                                                    FCopyMemoryW(pDelim, wAppend);
                                                    pDelim += 17;        //"x96\x96dbg.exe" "
                                                    FCopyMemoryW(pDelim, wArg);
                                                    pDelim += dwArgLen;
                                                    pDelim[0] = L'"';
                                                    pDelim[1] = L'\0';

                                                    PROCESS_INFORMATION pi;
                                                    STARTUPINFO si;
                                                    DWORD dwSize = sizeof(STARTUPINFO);
                                                    BYTE *pDst = static_cast<BYTE*>(static_cast<void*>(&si));
                                                    while (dwSize--)
                                                        *pDst++ = '\0';
                                                    si.cb = sizeof(STARTUPINFO);
                                                    if (CreateProcessW(nullptr, wBuf-1, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT, nullptr, pDirectory, &si, &pi))
                                                    {
                                                        CloseHandle(pi.hThread);
                                                        CloseHandle(pi.hProcess);
                                                    }
                                                }
                                            }
                                        }
                                }
                            }
                        }
                        else
                            CloseHandle(hFile);
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
