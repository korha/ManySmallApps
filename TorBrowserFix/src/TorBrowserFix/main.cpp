//TorBrowserFix
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#include <ws2tcpip.h>
#include <shlobj.h>
#include <iphlpapi.h>

#ifndef _WIN64
#define EXPORT //__declspec(dllexport)

static constexpr const DWORD g_dwPathMargin = 23;        //"\AccessibleMarshal.dll`"
static constexpr const LONG g_iKeyDummy = 0x80001000;
static constexpr const DWORD g_dwBufSecond = 1+MAX_PATH+26;        //"" -contentproc --channel=""
static wchar_t g_wBufApp[g_dwBufSecond+1+MAX_PATH+1];
static wchar_t *g_wDelimApp = nullptr;
static wchar_t *g_wDelimAppSave = nullptr;
static wchar_t *g_wSecond;
static wchar_t *g_wDelimSecond;
static SIZE_T g_szPathBytes;
static bool g_bHasSpace = false;

//-------------------------------------------------------------------------------------------------
static inline bool FCompareMemoryW(const wchar_t *pBuf1, const wchar_t *pBuf2)
{
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

static inline void FCopyMemoryW(wchar_t *pDst, const wchar_t *pSrc)
{
    while ((*pDst++ = *pSrc++));
}

static inline bool FIsStartWithW(const wchar_t *pFullStr, const wchar_t *pBeginStr)
{
    while (*pBeginStr)
        if (*pFullStr++ != *pBeginStr++)
            return false;
    return true;
}

//-------------------------------------------------------------------------------------------------
EXPORT LPWSTR * CommandLineToArgvWStub(LPCWSTR lpCmdLine, int *pNumArgs)
{
    return CommandLineToArgvW(lpCmdLine, pNumArgs);
}

//-------------------------------------------------------------------------------------------------
static LONG WINAPI RegOpenKeyStub(HKEY, LPCVOID, PHKEY)
{
    return ERROR_ACCESS_DENIED;
}

static LONG WINAPI RegOpenKeyExAStub(HKEY, LPCSTR, DWORD, REGSAM, PHKEY)
{
    return ERROR_ACCESS_DENIED;
}

static LONG WINAPI RegOpenKeyExWStub(HKEY hKey, LPCWSTR lpSubKey, DWORD, REGSAM, PHKEY phkResult)
{
    if (hKey == HKEY_CURRENT_USER && lpSubKey && FCompareMemoryW(lpSubKey, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Attachments"))
    {
        *phkResult = reinterpret_cast<HKEY>(g_iKeyDummy);
        return ERROR_SUCCESS;
    }
    return ERROR_ACCESS_DENIED;
}

typedef LONG (WINAPI *PRegQueryValueExW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
static PRegQueryValueExW RegQueryValueExWReal;
static LONG WINAPI RegQueryValueExWStub(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
    if (hKey == reinterpret_cast<HKEY>(g_iKeyDummy))
    {
        if (lpValueName && lpData && FCompareMemoryW(lpValueName, L"SaveZoneInformation"))
        {
            if (lpType)
                *lpType = REG_DWORD;
            *static_cast<DWORD*>(static_cast<void*>(lpData)) = 1;
            if (lpcbData)
                *lpcbData = sizeof(DWORD);
            return ERROR_SUCCESS;
        }
        return ERROR_ACCESS_DENIED;
    }
    return RegQueryValueExWReal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static LONG WINAPI RegCreateKeyStub(HKEY, LPCVOID, PHKEY)
{
    return ERROR_ACCESS_DENIED;
}

static LONG WINAPI RegCreateKeyExStub(HKEY, LPCVOID, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD)
{
    return ERROR_ACCESS_DENIED;
}

typedef LONG (WINAPI *PRegCloseKey)(HKEY hKey);
static PRegCloseKey RegCloseKeyReal;
static LONG WINAPI RegCloseKeyStub(HKEY hKey)
{
    return hKey == reinterpret_cast<HKEY>(g_iKeyDummy) ? ERROR_SUCCESS : RegCloseKeyReal(hKey);
}

//-------------------------------------------------------------------------------------------------
static WINBOOL WINAPI CreateProcessStub(LPCVOID, LPVOID, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCVOID, LPSTARTUPINFO, LPPROCESS_INFORMATION)
{
    return FALSE;
}

typedef WINBOOL (WINAPI *PCreateProcessW)(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
static PCreateProcessW CreateProcessWReal;
static WINBOOL WINAPI CreateProcessWStub(LPCWSTR, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
    bool bOk = false;
    if (FIsStartWithW(lpCommandLine, g_wBufApp))        //content tab
        bOk = true;
    else if (*g_wDelimSecond = L'E', FIsStartWithW(lpCommandLine, g_wSecond-1))        //external runner
        bOk = true;
    else
    {
        //torrent file
        const wchar_t *wIt = lpCommandLine;
        if (*wIt == L'"' && *++wIt >= L'A' && *wIt <= L'Z' && *++wIt == L':' && *++wIt == L'\\')
            while (*++wIt)
                if (*wIt == L'"')
                {
                    if (wIt[-1] == L'e' && wIt[-2] == L'x' && wIt[-3] == L'e' && wIt[-4] == L'.' &&
                            *++wIt == L' ' && *++wIt == L'"' && *++wIt >= L'A' && *wIt <= L'Z' && *++wIt == L':' && *++wIt == L'\\')
                        while (*++wIt)
                            if (*wIt == L'"')
                            {
                                if (wIt[1] == L'\0' &&
                                        wIt[-1] == L't' &&
                                        wIt[-2] == L'n' &&
                                        wIt[-3] == L'e' &&
                                        wIt[-4] == L'r' &&
                                        wIt[-5] == L'r' &&
                                        wIt[-6] == L'o' &&
                                        wIt[-7] == L't' &&
                                        wIt[-8] == L'.')
                                    bOk = true;
                                break;
                            }
                    break;
                }
        //restart app
        if (!bOk)
        {
            if (g_bHasSpace)
            {
                const wchar_t wSave = *g_wDelimApp;
                *g_wDelimApp = L'\0';
                if (FCompareMemoryW(lpCommandLine, g_wBufApp))
                    bOk = true;
                *g_wDelimApp = wSave;
            }
            else
            {
                g_wDelimApp[-1] = L'\0';
                if (FCompareMemoryW(lpCommandLine, g_wBufApp+1))
                {
                    g_wDelimApp[-1] = L'"';
                    const wchar_t wSave = *g_wDelimApp;
                    *g_wDelimApp = L'\0';
                    const WINBOOL bRes = CreateProcessWReal(nullptr, g_wBufApp, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
                    *g_wDelimApp = wSave;
                    return bRes;
                }
                else
                    g_wDelimApp[-1] = L'"';
            }
        }
    }
    *g_wDelimSecond = L'\0';
    return (bOk || MessageBoxW(nullptr, lpCommandLine, L"Browser. Run?", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) ?
                CreateProcessWReal(nullptr, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation) :
                FALSE;
}

static HINSTANCE WINAPI ShellExecuteStub(HWND, LPCVOID, LPCVOID, LPCVOID, LPCVOID, INT)
{
    return reinterpret_cast<HINSTANCE>(ERROR_FILE_NOT_FOUND);
}

static WINBOOL WINAPI ShellExecuteExStub(SHELLEXECUTEINFO *)
{
    return FALSE;
}

typedef WINBOOL (WINAPI *PShellExecuteExW)(SHELLEXECUTEINFOW *pExecInfo);
static PShellExecuteExW ShellExecuteExWReal;
static WINBOOL WINAPI ShellExecuteExWStub(SHELLEXECUTEINFOW *pExecInfo)
{
    pExecInfo->fMask |= SEE_MASK_FLAG_NO_UI;
    return ShellExecuteExWReal(pExecInfo);
}

//-------------------------------------------------------------------------------------------------
static int WSAAPI gethostnameStub(char *, int)
{
    return SOCKET_ERROR;
}

static struct hostent * WSAAPI gethostbynameStub(const char *)
{
    return nullptr;
}

static struct hostent * WSAAPI gethostbyaddrStub(const char *, int, int)
{
    return nullptr;
}

static HANDLE WSAAPI WSAAsyncGetHostByNameStub(HWND, u_int, const char *, char *, int)
{
    return nullptr;
}

static HANDLE WSAAPI WSAAsyncGetHostByAddrStub(HWND, u_int, const char *, int, int, char *, int)
{
    return nullptr;
}

static WINBOOL WINAPI GetComputerNameStub(LPVOID, LPDWORD)
{
    return FALSE;
}

static WINBOOL WINAPI GetComputerNameExStub(COMPUTER_NAME_FORMAT, LPVOID, LPDWORD)
{
    return FALSE;
}

static DWORD WINAPI SendARPStub(IPAddr, IPAddr, PVOID, PULONG)
{
    const DWORD dwReturn[] = {ERROR_BAD_NET_NAME, ERROR_GEN_FAILURE, ERROR_NOT_FOUND, ERROR_NOT_SUPPORTED};
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return dwReturn[ft.dwLowDateTime & 0b11];
}

//-------------------------------------------------------------------------------------------------
static HRESULT WINAPI SHGetFolderPathWStub(HWND, int, HANDLE, DWORD, LPWSTR pszPath)
{
    const wchar_t *pSrc = g_wSecond;
    while ((*pszPath++ = *pSrc++));
    return S_OK;
}

static WINBOOL WINAPI SHGetSpecialFolderPathWStub(HWND, LPWSTR pszPath, int, WINBOOL)
{
    const wchar_t *pSrc = g_wSecond;
    while ((*pszPath++ = *pSrc++));
    return TRUE;
}

static HRESULT WINAPI SHGetKnownFolderPathStub(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR *ppszPath)
{
    if (wchar_t *pMemory = static_cast<wchar_t*>(CoTaskMemAlloc(g_szPathBytes)))
    {
        *ppszPath = pMemory;
        const wchar_t *pSrc = g_wSecond;
        while ((*pMemory++ = *pSrc++));
        return S_OK;
    }
    return S_FALSE;
}

static HRESULT WINAPI SHGetSpecialFolderLocationStub(HWND, int, PIDLIST_ABSOLUTE *ppidl)
{
    return SHILCreateFromPath(g_wSecond, ppidl, nullptr);
}

static DWORD WINAPI GetTempPathWStub(DWORD, LPWSTR lpBuffer)
{
    const wchar_t *pSrc = g_wSecond;
    while ((*lpBuffer++ = *pSrc++));
    lpBuffer[-1] = L'\\';
    *lpBuffer = L'\0';
    return pSrc-g_wSecond;
}

//-------------------------------------------------------------------------------------------------
static bool FPatch(BYTE *pAddress, size_t szOffset, void **const ppOriginal)
{
    constexpr const DWORD dwPatchSize = 1+sizeof(size_t);
    const SIZE_T szLen = ppOriginal ? (pAddress -= dwPatchSize, dwPatchSize+2) : dwPatchSize;
    DWORD dwOldProtect;
    if (VirtualProtect(pAddress, szLen, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        szOffset -= reinterpret_cast<size_t>(pAddress) + dwPatchSize;
        const BYTE *pByte = static_cast<const BYTE*>(static_cast<const void*>(&szOffset));
        pAddress[0] = 0xE9;        //jump near
        pAddress[1] = pByte[0];
        pAddress[2] = pByte[1];
        pAddress[3] = pByte[2];
        pAddress[4] = pByte[3];
        if (ppOriginal)
        {
            pAddress[5] = 0xEB;        //jump short
            pAddress[6] = 0xF9;        //-7
            *ppOriginal = pAddress+dwPatchSize+2;
        }
        DWORD dwTemp;
        if (VirtualProtect(pAddress, szLen, dwOldProtect, &dwTemp))
            return true;
    }
    return false;
}

template <typename T1, typename T2>
static inline bool FCreateHook(T1 *const pTarget, T2 *const pDetour, T1 **const ppOriginal = nullptr)
{return FPatch(reinterpret_cast<BYTE*>(pTarget), reinterpret_cast<size_t>(pDetour), reinterpret_cast<void**>(ppOriginal));}

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
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE hInstDll, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        if (const wchar_t *const wCmdLine = GetCommandLineW())
        {
            wchar_t *const pBuf = g_wBufApp+1;
            DWORD dwTemp = GetModuleFileNameW(nullptr, pBuf, MAX_PATH+1);
            if (dwTemp >= 6 && dwTemp < MAX_PATH)
            {
                wchar_t *wIt = pBuf;
                while (*wIt)
                {
                    if (*wIt == L'\\')
                        g_wDelimApp = wIt;
                    else if (*wIt == L' ')
                        g_bHasSpace = true;
                    ++wIt;
                }
                if (g_wDelimApp >= pBuf+4 && g_wDelimApp <= pBuf+MAX_PATH-g_dwPathMargin)
                {
                    *g_wBufApp = L'"';
                    *wIt = L'"';
                    *++wIt = L'\0';
                    bool bOk = false;
                    if (FCompareMemoryW(wCmdLine, g_wBufApp))
                    {
                        g_wDelimAppSave = g_wDelimApp;
                        g_wSecond = g_wBufApp+g_dwBufSecond+1;
                        dwTemp = g_wDelimApp-pBuf;
                        if (GetCurrentDirectoryW(dwTemp+1, g_wSecond) == dwTemp &&
                                (*g_wDelimApp = L'\0', FCompareMemoryW(g_wSecond, pBuf)) &&
                                GetEnvironmentVariableW(L"XRE_PROFILE_PATH", g_wSecond, MAX_PATH) > dwTemp &&
                                FIsStartWithW(g_wSecond, pBuf) &&
                                ((dwTemp = GetFileAttributesW(g_wSecond)) != INVALID_FILE_ATTRIBUTES && (dwTemp & FILE_ATTRIBUTE_DIRECTORY)))
                        {
                            *g_wDelimApp = L'\\';
                            g_wDelimSecond = g_wSecond+(g_wDelimApp-g_wBufApp);
                            *g_wDelimSecond = L'!';
                            *++g_wDelimSecond = L'\0';
                            if ((dwTemp = GetFileAttributesW(g_wSecond)) != INVALID_FILE_ATTRIBUTES && (dwTemp & FILE_ATTRIBUTE_DIRECTORY) &&
                                    FCreateHook(CreateProcessW, CreateProcessWStub, &CreateProcessWReal) &&
                                    FCreateHook(ShellExecuteExW, ShellExecuteExWStub, &ShellExecuteExWReal))
                            {
                                FCopyMemoryW(wIt, L" -contentproc --channel=\"");
                                g_wDelimApp = wIt;
                                g_wSecond[-1] = L'"';
                                FCopyMemoryW(g_wDelimSecond+1, L"XT\\Ext.exe\" \"");
                                g_szPathBytes = (g_wDelimSecond-g_wSecond+1)*sizeof(wchar_t);
                                bOk = true;
                            }
                        }
                    }
                    else if ((FCopyMemoryW(wIt, L" -contentproc --channel=\""), FIsStartWithW(wCmdLine, g_wBufApp)) &&
                             FCreateHook(CreateProcessW, CreateProcessStub) &&
                             FCreateHook(ShellExecuteExW, ShellExecuteExStub))
                    {
                        g_wSecond = pBuf;
                        *++g_wDelimApp = L'!';
                        *++g_wDelimApp = L'\0';
                        g_szPathBytes = (g_wDelimApp-pBuf+1)*sizeof(wchar_t);
                        bOk = true;
                    }
                    if (bOk &&
                            FCreateHook(RegOpenKeyA, RegOpenKeyStub) &&
                            FCreateHook(RegOpenKeyW, RegOpenKeyStub) &&
                            FCreateHook(RegOpenKeyExA, RegOpenKeyExAStub) &&
                            FCreateHook(RegOpenKeyExW, RegOpenKeyExWStub) &&
                            FCreateHook(RegQueryValueExW, RegQueryValueExWStub, &RegQueryValueExWReal) &&
                            FCreateHook(RegCreateKeyA, RegCreateKeyStub) &&
                            FCreateHook(RegCreateKeyW, RegCreateKeyStub) &&
                            FCreateHook(RegCreateKeyExA, RegCreateKeyExStub) &&
                            FCreateHook(RegCreateKeyExW, RegCreateKeyExStub) &&
                            FCreateHook(RegCloseKey, RegCloseKeyStub, &RegCloseKeyReal) &&
                            FCreateHook(CreateProcessA, CreateProcessStub) &&
                            FCreateHook(ShellExecuteA, ShellExecuteStub) &&
                            FCreateHook(ShellExecuteW, ShellExecuteStub) &&
                            FCreateHook(ShellExecuteExA, ShellExecuteExStub) &&
                            FCreateHook(gethostname, gethostnameStub) &&
                            FCreateHook(gethostbyname, gethostbynameStub) &&
                            FCreateHook(gethostbyaddr, gethostbyaddrStub) &&
                            FCreateHook(WSAAsyncGetHostByName, WSAAsyncGetHostByNameStub) &&
                            FCreateHook(WSAAsyncGetHostByAddr, WSAAsyncGetHostByAddrStub) &&
                            FCreateHook(GetComputerNameA, GetComputerNameStub) &&
                            FCreateHook(GetComputerNameW, GetComputerNameStub) &&
                            FCreateHook(GetComputerNameExA, GetComputerNameExStub) &&
                            FCreateHook(GetComputerNameExW, GetComputerNameExStub) &&
                            FCreateHook(SendARP, SendARPStub) &&
                            FCreateHook(SHGetFolderPathW, SHGetFolderPathWStub) &&
                            FCreateHook(SHGetSpecialFolderPathW, SHGetSpecialFolderPathWStub) &&
                            FCreateHook(SHGetKnownFolderPath, SHGetKnownFolderPathStub) &&
                            FCreateHook(SHGetSpecialFolderLocation, SHGetSpecialFolderLocationStub) &&
                            FCreateHook(GetTempPathW, GetTempPathWStub) &&
                            DisableThreadLibraryCalls(hInstDll))
                        return TRUE;
                }
            }
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH && g_wDelimAppSave)
    {
        WIN32_FIND_DATA findFileData;
        FILETIME ft;        //2001.01.01 00:00:00:000
        ft.dwHighDateTime = 29389701;
        ft.dwLowDateTime = 3365781504;
        *g_wDelimAppSave = L'\0';
        FSetTime(g_wBufApp+1, &ft, true);
        *g_wDelimAppSave = L'\\';
        *++g_wDelimAppSave = L'*';
        g_wDelimAppSave[1] = L'\0';
        FSubFolder(&findFileData, &ft, g_wBufApp+1, g_wDelimAppSave);
    }
    return FALSE;
}
#else
//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, DWORD, LPVOID)
{
    return FALSE;
}
#endif
