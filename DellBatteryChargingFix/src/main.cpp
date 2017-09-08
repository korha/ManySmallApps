//DellBatteryChargingFix
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <shlobj.h>
#include <shlwapi.h>
#include <oledlg.h>
#include <commctrl.h>
#include "MinHook/MinHook.h"

#define EXPORT //__declspec(dllexport)

static constexpr const DWORD g_dwPathMargin = 26;        //"\Dell\QuickSet\QSEBLSHARE`"
static constexpr const int g_iIdOff = 1108;
static wchar_t g_wBuf[MAX_PATH+1];
static HWND g_hWndApp, g_hWndSysTabControl, g_hWndOk;
static UINT g_iState;

//-------------------------------------------------------------------------------------------------
EXPORT UINT OleUIBusyWStub(LPOLEUIBUSYW)
{
    return OLEUI_FALSE;
}

//-------------------------------------------------------------------------------------------------
static LONG WINAPI RegOpenKeyWStub(HKEY, LPCWSTR, PHKEY)
{
    return ERROR_ACCESS_DENIED;
}

static LONG WINAPI RegOpenKeyExWStub(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY)
{
    return ERROR_ACCESS_DENIED;
}

static LONG WINAPI RegCreateKeyExWStub(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD)
{
    return ERROR_ACCESS_DENIED;
}

static DWORD WINAPI SHGetValueWStub(HKEY, LPCWSTR, LPCWSTR, DWORD *, void *, DWORD *)
{
    return ERROR_ACCESS_DENIED;
}

static WINBOOL WINAPI SHGetSpecialFolderPathWStub(HWND, LPWSTR pszPath, int, WINBOOL)
{
    const wchar_t *pSrc = g_wBuf;
    while ((*pszPath++ = *pSrc++));
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
typedef HWND (WINAPI *PCreateDialogIndirectParamW)(HINSTANCE hInstance, LPCDLGTEMPLATEW lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
static PCreateDialogIndirectParamW CreateDialogIndirectParamWReal;
static HWND WINAPI CreateDialogIndirectParamWStub(HINSTANCE hInstance, LPCDLGTEMPLATEW lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
    const HWND hWnd = CreateDialogIndirectParamWReal(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
    if (!g_hWndApp)
    {
        g_hWndApp = hWndParent;
        g_hWndOk = GetDlgItem(hWndParent, 1);
        g_hWndSysTabControl = FindWindowExW(hWndParent, nullptr, L"SysTabControl32", L"");
    }
    return hWnd;
}

//-------------------------------------------------------------------------------------------------
typedef WINBOOL (WINAPI *PGetMessageW)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
static PGetMessageW GetMessageWReal;
static WINBOOL WINAPI GetMessageWStub(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    const WINBOOL bResult = GetMessageWReal(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    static bool bIsFired = false;
    if (!bIsFired && lpMsg->message == WM_TIMER)
    {
        bIsFired = true;
        if (g_hWndSysTabControl && g_hWndOk && SendMessageW(g_hWndSysTabControl, TCM_SETCURSEL, 1, 0) == 0)
        {
            NMHDR nmhdr;
            nmhdr.hwndFrom = g_hWndSysTabControl;
            nmhdr.idFrom = 0;
            nmhdr.code = TCN_SELCHANGE;
            SendMessageW(g_hWndSysTabControl, WM_NOTIFY, TCN_SELCHANGE, reinterpret_cast<LPARAM>(&nmhdr));
            if (PostMessageW(g_hWndOk, BM_CLICK, 0, 0))
                return bResult;
        }
        MessageBoxW(nullptr, L"Error", L"DellBatteryChargingOff", MB_ICONERROR);
    }
    return bResult;
}

//-------------------------------------------------------------------------------------------------
typedef WINBOOL (WINAPI *PShowWindow)(HWND hWnd, int nCmdShow);
static PShowWindow ShowWindowReal;
static WINBOOL WINAPI ShowWindowStub(HWND hWnd, int nCmdShow)
{
    if (hWnd == g_hWndApp)
        nCmdShow = SW_HIDE;
    return ShowWindowReal(hWnd, nCmdShow);
}

//-------------------------------------------------------------------------------------------------
static UINT WINAPI IsDlgButtonCheckedStub(HWND, int nIDButton)
{
    return nIDButton == g_iIdOff ? g_iState : BST_UNCHECKED;
}

//-------------------------------------------------------------------------------------------------
static UINT fGetArgv()
{
    UINT iState = BST_INDETERMINATE;
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
                        return iState;
                    ++wCmdLine;
                }
                ++wCmdLine;
                if (*wCmdLine != L' ' && *wCmdLine != L'\t')
                    return iState;
            }
            else
                while (*wCmdLine != L' ' && *wCmdLine != L'\t')
                {
                    if (*wCmdLine == L'\0' || *wCmdLine == L'\"')
                        return iState;
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
                            return iState;
                        ++wCmdLine;
                    }
                    if (wCmdLine[1] != L' ' && wCmdLine[1] != L'\t' && wCmdLine[1] != L'\0')
                        return iState;
                    ++wArg;
                }
                else
                    while (*wCmdLine != L' ' && *wCmdLine != L'\t' && *wCmdLine != L'\0')
                        ++wCmdLine;
                *wCmdLine = L'\0';

                if (wArg[0] == L'/' && wArg[1] == L'o')
                {
                    if (wArg[2] == L'f' && wArg[3] == L'f' && wArg[4] == L'\0')
                        iState = BST_CHECKED;
                    else if (wArg[2] == L'n' && wArg[3] == L'\0')
                        iState = BST_UNCHECKED;
                }
            }
        }
    }
    return iState;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
static inline bool FCreateHook(T *const pTarget, T *const pDetour, T **const ppOriginal = nullptr)
{return MH_CreateHook(reinterpret_cast<LPVOID>(pTarget), reinterpret_cast<LPVOID>(pDetour), reinterpret_cast<LPVOID*>(ppOriginal)) == MH_OK;}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE hInstDll, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        const DWORD dwLen = GetModuleFileNameW(nullptr, g_wBuf, MAX_PATH+1);
        if (dwLen >= 6 && dwLen < MAX_PATH)
        {
            wchar_t *pDelim = g_wBuf+dwLen;
            do
            {
                if (*--pDelim == L'\\')
                    break;
            } while (pDelim > g_wBuf);
            if (pDelim >= g_wBuf+4 && pDelim <= g_wBuf+MAX_PATH-g_dwPathMargin &&
                    MH_Initialize() == MH_OK &&
                    FCreateHook(RegOpenKeyW, RegOpenKeyWStub) &&
                    FCreateHook(RegOpenKeyExW, RegOpenKeyExWStub) &&
                    FCreateHook(RegCreateKeyExW, RegCreateKeyExWStub) &&
                    FCreateHook(SHGetValueW, SHGetValueWStub) &&
                    FCreateHook(SHGetSpecialFolderPathW, SHGetSpecialFolderPathWStub) &&
                    ((g_iState = fGetArgv()) == BST_INDETERMINATE ||
                     (FCreateHook(CreateDialogIndirectParamW, CreateDialogIndirectParamWStub, &CreateDialogIndirectParamWReal) &&
                      FCreateHook(GetMessageW, GetMessageWStub, &GetMessageWReal) &&
                      FCreateHook(ShowWindow, ShowWindowStub, &ShowWindowReal) &&
                      FCreateHook(IsDlgButtonChecked, IsDlgButtonCheckedStub))) &&
                    MH_EnableHook(MH_ALL_HOOKS) == MH_OK &&
                    DisableThreadLibraryCalls(hInstDll))
            {
                *pDelim = L'\0';
                return TRUE;
            }
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
        MH_Uninitialize();
    return FALSE;
}
