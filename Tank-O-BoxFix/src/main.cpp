//Tank-O-BoxFix
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

#ifndef _WIN64
#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
EXPORT HRESULT WINAPI DirectInput8CreateStub(HINSTANCE, DWORD, REFIID, LPVOID *, LPVOID)
{
    return S_FALSE;
}

static LONG WINAPI RegCreateKeyExAStub(HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD)
{
    return ERROR_ACCESS_DENIED;
}

//-------------------------------------------------------------------------------------------------
typedef int (WINAPI *PMultiByteToWideChar)(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
static PMultiByteToWideChar MultiByteToWideCharReal;
static int WINAPI MultiByteToWideCharStub(UINT, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
    return MultiByteToWideCharReal(1251, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProcSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR)
{
    static LONG iCxBorders, iCyBorders;

    switch (uMsg)
    {
    case WM_SIZING:
        if (const LONG iStyle = GetWindowLongPtrW(hWnd, GWL_STYLE))
            if (const LONG iExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE))
            {
                RECT rectAdjust; rectAdjust.left = 0; rectAdjust.top = 0; rectAdjust.right = 1024; rectAdjust.bottom = 768;
                if (AdjustWindowRectEx(&rectAdjust, iStyle, FALSE, iExStyle))
                {
                    iCxBorders = rectAdjust.right-rectAdjust.left-1024;
                    iCyBorders = rectAdjust.bottom-rectAdjust.top-768;

                    RECT *const rect = reinterpret_cast<RECT*>(lParam);
                    if (wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM)
                        rect->right = rect->left + (rect->bottom-rect->top-iCyBorders)*4/3 + iCxBorders;
                    else if (wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT)
                        rect->top = rect->bottom - (rect->right-rect->left-iCxBorders)*3/4 - iCyBorders;
                    else
                        rect->bottom = rect->top + (rect->right-rect->left-iCxBorders)*3/4 + iCyBorders;
                }
            }
        return TRUE;
    case WM_GETMINMAXINFO:
        if (const int iCx = GetSystemMetrics(SM_CXVIRTUALSCREEN))
            if (const int iCy = GetSystemMetrics(SM_CYVIRTUALSCREEN))
            {
                MINMAXINFO *const mmInfo = reinterpret_cast<MINMAXINFO*>(lParam);
                mmInfo->ptMaxTrackSize.x = (iCy-iCyBorders)*4/3+iCxBorders;
                mmInfo->ptMaxTrackSize.y = (iCx-iCxBorders)*3/4+iCyBorders;
            }
        return 0;
    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, WindowProcSubclass, uIdSubclass);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
        if (wParam == VK_SPACE)
            wParam = VK_TAB;
        else if (wParam == VK_SHIFT)
            wParam = VK_CONTROL;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
static HWND WINAPI CreateWindowExAStub(DWORD dwExStyle, LPCSTR, LPCSTR, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    if (const HWND hWndApp = CreateWindowExW(dwExStyle, L"liba::WindowsCreator", L"Loading...", dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))
        if (SetWindowSubclass(hWndApp, WindowProcSubclass, 1, 0))
            return hWndApp;
    return nullptr;
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

template <typename T>
inline bool FCreateHook(T *const pTarget, T *const pDetour, T **const ppOriginal = nullptr)
{return FPatch(reinterpret_cast<BYTE*>(pTarget), reinterpret_cast<size_t>(pDetour), reinterpret_cast<void**>(ppOriginal));}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE hInstDll, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        if (
                FCreateHook(RegCreateKeyExA, RegCreateKeyExAStub) &&
                FCreateHook(MultiByteToWideChar, MultiByteToWideCharStub, &MultiByteToWideCharReal) &&
                FCreateHook(CreateWindowExA, CreateWindowExAStub) &&
                DisableThreadLibraryCalls(hInstDll))
            return TRUE;
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        BYTE *const pAddress = reinterpret_cast<BYTE*>(MultiByteToWideChar);
        DWORD dwOldProtect;
        if (VirtualProtect(pAddress, 2, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            pAddress[0] = 0x8B;        //mov
            pAddress[1] = 0xFF;        //edi,edi
            DWORD dwTemp;
            VirtualProtect(pAddress, 2, dwOldProtect, &dwTemp);
        }
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
