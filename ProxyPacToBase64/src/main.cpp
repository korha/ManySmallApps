//ProxyPacToBase64
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//-------------------------------------------------------------------------------------------------
static inline wchar_t * FCopyMemoryAW(wchar_t *pDst, const char *pSrc)
{
    while ((*pDst++ = *pSrc++));
    return pDst-1;
}

//-------------------------------------------------------------------------------------------------
static const wchar_t * FGetArg()
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
                        return nullptr;
                    ++wCmdLine;
                }
                ++wCmdLine;
                if (*wCmdLine != L' ' && *wCmdLine != L'\t')
                    return nullptr;
            }
            else
                while (*wCmdLine != L' ' && *wCmdLine != L'\t')
                {
                    if (*wCmdLine == L'\0' || *wCmdLine == L'\"')
                        return nullptr;
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
                *wCmdLine = L'\0';
                return wArg;
            }
        }
    }
    return nullptr;
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    const wchar_t *wString = FGetArg();
    if (wString)
    {
        if (const HANDLE hProcHeap = GetProcessHeap())
        {
            const HANDLE hFile = CreateFileW(wString, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                char *cInBuf = nullptr;
                DWORD dwSize = 0;
                LARGE_INTEGER iFileSize;
                if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.HighPart == 0 && iFileSize.LowPart >= 52 && iFileSize.LowPart <= 1024*1024)
                {
                    dwSize = (cInBuf = static_cast<char*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, iFileSize.LowPart))) &&
                            ReadFile(hFile, cInBuf, iFileSize.LowPart, &dwSize, nullptr) && dwSize == iFileSize.LowPart;
                }
                if (CloseHandle(hFile) && dwSize)
                {
                    const char *cIt = cInBuf;
                    const char *cLast = cInBuf+iFileSize.LowPart;
                    int iNumQuote = 0, iNumStick = 0, iNumBracket = 0, iNumAmpersand = 0, iNumFigure = 0;
                    for (; cIt < cLast; ++cIt)
                    {
                        if (*cIt == '"')
                            ++iNumQuote;
                        else if (*cIt == '|')
                            ++iNumStick;
                        else if (*cIt == '(' || *cIt == ')')
                            ++iNumBracket;
                        else if (*cIt == '&')
                            ++iNumAmpersand;
                        else if (*cIt == '{' || *cIt == '}')
                            ++iNumFigure;
                        else if (!((*cIt >= 'a' && *cIt <= 'z') ||
                                   (*cIt >= 'A' && *cIt <= 'Z') ||
                                   (*cIt >= ',' && *cIt <= ';') ||
                                   *cIt == '*' || *cIt == ' ' || *cIt == '\n' || *cIt == '!' || *cIt == '\r' || *cIt == '='))
                        {
                            cIt = nullptr;
                            break;
                        }
                    }
                    if (cIt)
                    {
                        if (!((iNumQuote & 1) || (iNumStick & 1) || (iNumBracket & 1) || (iNumAmpersand & 1) || (iNumFigure & 1)))
                        {
                            wString = L"Clipboard failed";
                            if (OpenClipboard(nullptr))
                            {
                                bool bOk = false;
                                if (const HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (46 + (iFileSize.LowPart+2)/3*4 + 1)*sizeof(wchar_t)))
                                {
                                    if (wchar_t *const wOutBuf = static_cast<wchar_t*>(GlobalLock(hGlobal)))
                                    {
                                        constexpr const char *const cTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                                        wchar_t *wIt = FCopyMemoryAW(wOutBuf, "data:application/x-ns-proxy-autoconfig;base64,");
                                        cIt = cInBuf;
                                        const char *const cLastSafe = cInBuf + iFileSize.LowPart/3*3;
                                        while (cIt < cLastSafe)
                                        {
                                            *wIt++ = cTable[cIt[0] >> 2];
                                            *wIt++ = cTable[63 & (cIt[0] << 4 | cIt[1] >> 4)];
                                            *wIt++ = cTable[63 & (cIt[1] << 2 | cIt[2] >> 6)];
                                            *wIt++ = cTable[63 & cIt[2]];
                                            cIt += 3;
                                        }
                                        if (cLastSafe < cLast)
                                        {
                                            *wIt++ = cTable[cIt[0] >> 2];
                                            if (cLastSafe+1 == cLast)
                                            {
                                                *wIt++ = cTable[63 & (cIt[0] << 4)];
                                                *wIt++ = L'=';
                                            }
                                            else
                                            {
                                                *wIt++ = cTable[63 & (cIt[0] << 4 | cIt[1] >> 4)];
                                                *wIt++ = cTable[63 & (cIt[1] << 2)];
                                            }
                                            *wIt++ = L'=';
                                        }
                                        *wIt = L'\0';
                                        if (EmptyClipboard() && SetClipboardData(CF_UNICODETEXT, hGlobal))
                                            bOk = true;
                                        GlobalUnlock(hGlobal);
                                    }
                                    GlobalFree(hGlobal);
                                }
                                if (CloseClipboard() && bOk)
                                    wString = nullptr;
                            }
                        }
                        else
                            wString = L"Some symbols needs pair";
                    }
                    else
                        wString = L"File contains illegal symbol";
                }
                else
                    wString = L"Read file error";
                if (cInBuf)
                    HeapFree(hProcHeap, HEAP_NO_SERIALIZE, cInBuf);
            }
            else
                wString = L"Open file error";
        }
        else
            wString = L"GetProcessHeap failed";
    }
    else
        wString = L"Need argument";

    if (wString)
        MessageBoxW(nullptr, wString, L"ProxyPacToBase64", MB_ICONERROR);
    else
        MessageBoxW(nullptr, L"Base64 copied to clipboard successfully", L"ProxyPacToBase64", MB_ICONINFORMATION);
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
