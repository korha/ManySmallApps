//TorBrowserRun
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ntdef.h>
#include <ntstatus.h>

#define IMPORT __declspec(dllimport)

template<typename T1, typename T2>
inline T1 pointer_cast(const T2 *const pSrc)
{return static_cast<T1>(static_cast<const void*>(pSrc));}

typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize;
    ULONG HardFaultCount;
    ULONG NumberOfThreadsHighWatermark;
    ULONGLONG CycleTime;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
} SYSTEM_PROCESS_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemProcessInformation = 5
} SYSTEM_INFORMATION_CLASS;

extern "C" IMPORT NTSTATUS NTAPI NtQuerySystemInformation
(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

static constexpr const DWORD g_dwPathMargin = 63;        //"\browser\extensions\{972ce4c6-7e08-4474-a285-3208198ce6fd}.xpi`"

//-------------------------------------------------------------------------------------------------
static inline void FCopyMemoryW(wchar_t *pDst, const wchar_t *pSrc)
{
    while ((*pDst++ = *pSrc++));
}

static inline wchar_t * FStrChrW(wchar_t *pSrc, const wchar_t wChar)
{
    while (*pSrc && *pSrc != wChar)
        ++pSrc;
    return *pSrc == wChar ? pSrc : nullptr;
}

static inline bool FCheckTor(const wchar_t *pBuf1)
{
    if (pBuf1)
    {
        const wchar_t *pBuf2 = L"tor.exe";
        while (*pBuf1 == *pBuf2 && *pBuf2)
            ++pBuf1, ++pBuf2;
        return *pBuf1 == *pBuf2;
    }
    return false;
}

//-------------------------------------------------------------------------------------------------
static inline void FSetTime(const wchar_t *const wBuf, const FILETIME *const ft, const bool bIsFolder)
{
    const HANDLE hFile = CreateFileW(wBuf, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, bIsFolder ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        SetFileTime(hFile, ft, ft, ft);
        CloseHandle(hFile);
    }
}

//-------------------------------------------------------------------------------------------------
static void FSubFolder(WIN32_FIND_DATA *const findFileData, const FILETIME *const ft, const wchar_t *const wBuf, wchar_t *const wDelim)
{
    const HANDLE hFile = FindFirstFileW(wBuf, findFileData);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (findFileData->dwFileAttributes != INVALID_FILE_ATTRIBUTES && (findFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                FCopyMemoryW(wDelim, findFileData->cFileName);
                FSetTime(wBuf, ft, true);
                if (!(findFileData->cFileName[0] == L'.' && (findFileData->cFileName[1] == L'\0' || (findFileData->cFileName[1] == L'.' && findFileData->cFileName[2] == L'\0'))))
                {
                    FCopyMemoryW(wDelim, findFileData->cFileName);
                    wchar_t *wNewDelim = wDelim;
                    while (*wNewDelim++);
                    wNewDelim[-1] = L'\\';
                    wNewDelim[0] = L'*';
                    wNewDelim[1] = L'\0';
                    FSubFolder(findFileData, ft, wBuf, wNewDelim);
                }
            }
            else
            {
                FCopyMemoryW(wDelim, findFileData->cFileName);
                FSetTime(wBuf, ft, false);
            }
        } while (FindNextFileW(hFile, findFileData));
        FindClose(hFile);
    }
}

//-------------------------------------------------------------------------------------------------
void FMain()
{
    wchar_t wBuf[MAX_PATH+MAX_PATH-7-1];       //"C:\A\A_" "."
    wchar_t *const pBufPath = wBuf+1,
            *const pBufAdd = wBuf+MAX_PATH;
    const DWORD dwLen = GetModuleFileNameW(nullptr, pBufPath, MAX_PATH+1);
    if (dwLen >= 6 && dwLen < MAX_PATH)
    {
        wchar_t *pDelim = pBufPath+dwLen;
        do
        {
            if (*--pDelim == L'\\')
                break;
        } while (pDelim > pBufPath);
        if (pDelim >= pBufPath+4 && pDelim <= pBufPath+MAX_PATH-g_dwPathMargin)
        {
            ++pDelim;
            const wchar_t wSave = *pDelim;
            if (wSave != L'_')
            {
                *pDelim = L'\0';
                FCopyMemoryW(pBufAdd, pBufPath);
                *pDelim = wSave;
                if (wchar_t *pDelimFrom = FStrChrW(pDelim, L'_'))
                    if (*++pDelimFrom != L'.')
                        if (wchar_t *pDelimTo = FStrChrW(pDelimFrom, L'.'))
                        {
                            *pDelimTo = L'\0';
                            pDelimTo = pBufAdd+(pDelim-pBufPath);
                            FCopyMemoryW(pDelimTo, pDelimFrom);
                            if (SetEnvironmentVariableW(L"XRE_PROFILE_PATH", pBufAdd))
                            {
                                bool bTorOk = false;
                                if (const HANDLE hProcHeap = GetProcessHeap())
                                {
                                    ULONG iSize;
                                    if (NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &iSize) == STATUS_INFO_LENGTH_MISMATCH)
                                        if (SYSTEM_PROCESS_INFORMATION *const pProcInfo = static_cast<SYSTEM_PROCESS_INFORMATION*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, iSize += 2048)))
                                        {
                                            if (NT_SUCCESS(NtQuerySystemInformation(SystemProcessInformation, pProcInfo, iSize, nullptr)))
                                            {
                                                const SYSTEM_PROCESS_INFORMATION *pIt = pProcInfo;
                                                do
                                                {
                                                    if (FCheckTor(pIt->ImageName.Buffer))
                                                    {
                                                        bTorOk = true;
                                                        break;
                                                    }
                                                } while (pIt->NextEntryOffset ?
                                                         (pIt = pointer_cast<const SYSTEM_PROCESS_INFORMATION*>(pointer_cast<const BYTE*>(pIt) + pIt->NextEntryOffset), true) :
                                                         false);
                                            }
                                            HeapFree(hProcHeap, HEAP_NO_SERIALIZE, pProcInfo);
                                        }
                                }
                                if (bTorOk || MessageBoxW(nullptr, L"Tor is not runned. Skip?", L"BrowserRun", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
                                {
                                    *wBuf = L'"';
                                    pDelimFrom[-1] = L'"';
                                    *pDelimFrom = L'\0';
                                    *pDelimTo = L'\0';

                                    PROCESS_INFORMATION pi;
                                    STARTUPINFO si;
                                    DWORD dwSize = sizeof(STARTUPINFO);
                                    BYTE *pDst = static_cast<BYTE*>(static_cast<void*>(&si));
                                    while (dwSize--)
                                        *pDst++ = '\0';
                                    si.cb = sizeof(STARTUPINFO);

                                    if (CreateProcessW(nullptr, wBuf, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT, nullptr, pBufAdd, &si, &pi))
                                    {
                                        CloseHandle(pi.hThread);
                                        CloseHandle(pi.hProcess);

                                        WIN32_FIND_DATA findFileData;
                                        FILETIME ft;        //2001.01.01 00:00:00:000
                                        ft.dwHighDateTime = 29389701;
                                        ft.dwLowDateTime = 3365781504;
                                        *pDelim = L'\0';
                                        FSetTime(wBuf+1, &ft, true);
                                        *pDelim = L'\\';
                                        *++pDelim = L'*';
                                        pDelim[1] = L'\0';
                                        FSubFolder(&findFileData, &ft, wBuf+1, pDelim);
                                    }
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
