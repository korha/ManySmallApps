//ErasePeInfoGcc
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <imagehlp.h>

static constexpr const DWORD g_dwSizeMin = 3*1024;
static constexpr const DWORD g_dwSizeMax = 1024*1024*1024;
static constexpr const DWORD g_dwTimeStamp = 978307200;        //2001.01.01 00:00:00
static constexpr const LONG g_lFaNew = 0x80;
static constexpr const WORD g_wOptionalHeaderSize32 = 0xE0;
static constexpr const WORD g_wOptionalHeaderSize64 = 0xF0;
static constexpr const DWORD g_dwApproxMargin = 42;

//-------------------------------------------------------------------------------------------------
#ifdef NDEBUG
#define ___assert___(cond) do{static_cast<void>(sizeof(cond));}while(false)
#else
#define ___assert___(cond) do{if(!(cond)){int i=__LINE__;char h[]="RUNTIME ASSERTION. Line:           "; \
    if(i>=0){char *c=h+35;do{*--c=i%10+'0';i/=10;}while(i>0);} \
    if(MessageBoxA(nullptr,__FILE__,h,MB_ICONERROR|MB_OKCANCEL)==IDCANCEL)ExitProcess(0);}}while(false)
#endif

template<typename T1, typename T2>
inline T1 pointer_cast(T2 *pSrc)
{return static_cast<T1>(static_cast<void*>(pSrc));}

template<typename T1, typename T2>
inline T1 pointer_cast(const T2 *pSrc)
{return static_cast<T1>(static_cast<const void*>(pSrc));}

static inline void FZeroMemory(BYTE *pDst, DWORD dwSize)
{
    ___assert___(dwSize >= g_dwApproxMargin && dwSize <= g_dwSizeMax);
    while (dwSize--)
        *pDst++ = '\0';
}

static inline bool FCompareMemory(const void *const pBuf1__, const char *pBuf2, DWORD dwSize)
{
    const char *pBuf1 = static_cast<const char*>(pBuf1__);
    while (dwSize--)
        if (*pBuf1++ != *pBuf2++)
            return false;
    return true;
}

//-------------------------------------------------------------------------------------------------
static const wchar_t * FNextArg(wchar_t **wCmdLine__)
{
    ___assert___(wCmdLine__);
    wchar_t *wCmdLine = *wCmdLine__;
    ___assert___(wCmdLine);
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
        bool bOk = true;
        do
        {
            const wchar_t *wError = nullptr;
            const HANDLE hFile = CreateFileW(wArg, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                LARGE_INTEGER iFileSize;
                if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.QuadPart >= g_dwSizeMin && iFileSize.QuadPart <= g_dwSizeMax)
                {
                    const HANDLE hFileMapping = CreateFileMappingW(hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);
                    CloseHandle(hFile);
                    if (hFileMapping)
                    {
                        if (BYTE *const pBtFileMap = static_cast<BYTE*>(MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, 0)))
                        {
                            const IMAGE_DOS_HEADER *const pImgDosHeader = pointer_cast<IMAGE_DOS_HEADER*>(pBtFileMap);
                            if (pImgDosHeader->e_magic == IMAGE_DOS_SIGNATURE && pImgDosHeader->e_lfanew == g_lFaNew)
                            {
                                IMAGE_NT_HEADERS *const pImgNtHeaders = pointer_cast<IMAGE_NT_HEADERS*>(pBtFileMap + g_lFaNew);
                                if (pImgNtHeaders->Signature == IMAGE_NT_SIGNATURE)
                                {
                                    WORD wSizeOfOptionalHeader = g_wOptionalHeaderSize32;
                                    IMAGE_FILE_HEADER *pImgFileHeader = &pImgNtHeaders->FileHeader;
                                    if ((pImgFileHeader->Machine == IMAGE_FILE_MACHINE_I386 ||
                                         (wSizeOfOptionalHeader = g_wOptionalHeaderSize64, pImgFileHeader->Machine == IMAGE_FILE_MACHINE_AMD64)) &&
                                            pImgNtHeaders->FileHeader.SizeOfOptionalHeader == wSizeOfOptionalHeader)
                                    {
                                        pImgFileHeader->TimeDateStamp = g_dwTimeStamp;
                                        DWORD dwHeaderSum, dwCheckSum;
                                        if (CheckSumMappedFile(pBtFileMap, iFileSize.LowPart, &dwHeaderSum, &dwCheckSum) == pImgNtHeaders)
                                            pImgNtHeaders->OptionalHeader.CheckSum = dwCheckSum;
                                        else
                                            wError = L"CheckSumMappedFile failed. Cancel All?";
                                    }
                                    else
                                        wError = L"Invalid File header. Cancel All?";
                                }
                                else
                                    wError = L"Incorrect NT header. Cancel All?";
                            }
                            else
                                wError = L"Invalid DOS header. Cancel All?";
                            UnmapViewOfFile(pBtFileMap);
                        }
                        else
                            wError = L"MapViewOfFile error. Cancel All?";
                        CloseHandle(hFileMapping);
                        if (!wError)
                        {
                            const DWORD dwAttrib = GetFileAttributesW(wArg);
                            if (!(dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_READONLY) && SetFileAttributesW(wArg, dwAttrib | FILE_ATTRIBUTE_READONLY)))
                                wError = L"File attribytes failed";
                        }
                    }
                    else
                        wError = L"CreateFileMapping error. Cancel All?";
                }
                else
                {
                    CloseHandle(hFile);
                    wError = L"Incorrect size of file. Cancel All?";
                }
            }
            else
                wError = L"Can't open file. Cancel All?";
            if (wError && MessageBoxW(nullptr, wError, wArg, MB_ICONWARNING | MB_YESNO) == IDYES)
            {
                bOk = false;
                break;
            }
        } while ((wArg = FNextArg(&wCmdLine)));

        if (bOk)
            MessageBoxW(nullptr, L"OK", L"", MB_ICONINFORMATION);
    }
    else
        MessageBoxW(nullptr, L"Invalid command line", L"", MB_ICONERROR);
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    ___assert___(IMAGE_SIZEOF_SHORT_NAME == 8);
    ___assert___(FIELD_OFFSET(IMAGE_NT_HEADERS32, Signature) == FIELD_OFFSET(IMAGE_NT_HEADERS64, Signature));
    ___assert___(FIELD_OFFSET(IMAGE_NT_HEADERS32, FileHeader) == FIELD_OFFSET(IMAGE_NT_HEADERS64, FileHeader));
    ___assert___(FIELD_OFFSET(IMAGE_OPTIONAL_HEADER32, CheckSum) == FIELD_OFFSET(IMAGE_OPTIONAL_HEADER64, CheckSum));

    FMain();
    ExitProcess(0);
    return 0;
}
