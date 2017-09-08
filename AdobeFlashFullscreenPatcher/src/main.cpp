//AdobeFlashFullscreenPatcher
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static constexpr const char *const g_cSignature = "\x46\x57\x53\x08\x07\x1B";
static constexpr const char *const g_cPatch =
        "\x46\x57\x53\x08\x5E\x00\x00\x00\x78\x00\x05\xDC\x00\x00\x05\xAA\x00\x00\x14\x01\x00\x44"
        "\x11\x00\x00\x00\x00\x43\x02\xFF\xFF\xFF\x3F\x03\x34\x00\x00\x00\x96\x07\x00\x00\x5F\x72"
        "\x6F\x6F\x74\x00\x1C\x96\x24\x00\x00\x41\x6C\x65\x72\x74\x52\x65\x73\x70\x6F\x6E\x73\x65"
        "\x4F\x76\x65\x72\x6C\x61\x79\x4D\x65\x73\x73\x61\x67\x65\x00\x00\x63\x6C\x6F\x73\x65\x00"
        "\x4F\x00\x40";
static constexpr const DWORD g_dwSizeMin = 12*1024*1024;
static constexpr const DWORD g_dwSizeMax = 64*1024*1024;
static constexpr const DWORD g_dwSizeOffset = 8*1024*1024;        //divisible by Allocation Granularity (GetSystemInfo)
static constexpr const DWORD g_dwSignatureLen = 6;
static constexpr const DWORD g_dwTotalBytes = 6912;
static constexpr const DWORD g_dwPatchLen = 91;
static constexpr const DWORD g_dwApproxMargin = 50;

//-------------------------------------------------------------------------------------------------
static inline void FZeroMemory(BYTE *pDst, DWORD dwSize)
{
    while (dwSize--)
        *pDst++ = '\0';
}

static inline void FCopyMemory(BYTE *pDst, const char *pSrc, DWORD dwSize)
{
    while (dwSize--)
        *pDst++ = *pSrc++;
}

static inline bool FCompareMemory(const BYTE *pBuf1, const char *pBuf2, DWORD dwSize)
{
    while (dwSize--)
        if (*pBuf1++ != *pBuf2++)
            return false;
    return true;
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
        const HANDLE hFile = CreateFileW(wString, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            LARGE_INTEGER iFileSize;
            if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.QuadPart > g_dwSizeMin && iFileSize.QuadPart < g_dwSizeMax)
            {
                const HANDLE hFileMapping = CreateFileMappingW(hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);
                CloseHandle(hFile);
                if (hFileMapping)
                {
                    if (BYTE *const pFileMap = static_cast<BYTE*>(MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, g_dwSizeOffset, 0)))
                    {
                        wString = L"Signature not found";
                        BYTE *pQuery = pFileMap;
                        const BYTE *const pEnd = pQuery+(iFileSize.LowPart-g_dwSizeOffset-g_dwTotalBytes-g_dwApproxMargin);
                        while (++pQuery < pEnd)
                            if (FCompareMemory(pQuery, g_cSignature, g_dwSignatureLen))
                            {
                                FCopyMemory(pQuery, g_cPatch, g_dwPatchLen);
                                FZeroMemory(pQuery+g_dwPatchLen, g_dwTotalBytes-g_dwPatchLen);
                                wString = nullptr;
                                break;
                            }
                        UnmapViewOfFile(pFileMap);
                    }
                    else
                        wString = L"MapViewOfFile error";
                    CloseHandle(hFileMapping);
                }
                else
                    wString = L"CreateFileMappingW error";
            }
            else
            {
                CloseHandle(hFile);
                wString = L"Invalid size of file";
            }
        }
        else
            wString = L"Can't open file";
    }
    else
        wString = L"You have to run with filepath to NPSWF*.dll in argument";

    if (wString)
        MessageBoxW(nullptr, wString, L"AdobeFlashFullscreenPatcher", MB_ICONERROR);
    else
        MessageBoxW(nullptr, L"File successfully patched", L"AdobeFlashFullscreenPatcher", MB_ICONINFORMATION);
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
