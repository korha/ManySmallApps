//DowntownRunFix
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

#ifndef _WIN64
#define EXPORT //__declspec(dllexport)

static LARGE_INTEGER g_liInitStub, g_liInitReal;
static int g_iShift;

//-------------------------------------------------------------------------------------------------
EXPORT LONG WINAPI RegCloseKeyStub(HKEY)
{
    return ERROR_ACCESS_DENIED;
}

EXPORT LONG WINAPI RegOpenKeyExAStub(HKEY, LPCSTR, DWORD, REGSAM, PHKEY)
{
    return ERROR_ACCESS_DENIED;
}

EXPORT LONG WINAPI RegQueryValueExAStub(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD)
{
    return ERROR_ACCESS_DENIED;
}

EXPORT LONG WINAPI RegSetValueExAStub(HKEY, LPCSTR, DWORD, DWORD, CONST BYTE *, DWORD)
{
    return ERROR_ACCESS_DENIED;
}

//-------------------------------------------------------------------------------------------------
typedef WINBOOL (WINAPI *PQueryPerformanceCounter)(LARGE_INTEGER *lpPerformanceCount);
static PQueryPerformanceCounter QueryPerformanceCounterReal;
static WINBOOL WINAPI QueryPerformanceCounterStub(LARGE_INTEGER *lpPerformanceCount)
{
    //only one thread access (no need synchronizations)
    LARGE_INTEGER liCurrent;
    QueryPerformanceCounterReal(&liCurrent);
    liCurrent.QuadPart -= g_liInitReal.QuadPart;
    liCurrent.QuadPart >>= g_iShift;
    liCurrent.QuadPart += g_liInitStub.QuadPart;
    lpPerformanceCount->QuadPart = liCurrent.QuadPart;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
static inline void fInitSpeedHack(const int iShift)
{
    QueryPerformanceCounterStub(&g_liInitStub);
    QueryPerformanceCounterReal(&g_liInitReal);
    g_iShift = iShift;
}

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProcSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case '1': fInitSpeedHack(0); return 0;        //2^0=1
        case '2': fInitSpeedHack(1); return 0;        //2^1=2
        case '4': fInitSpeedHack(2); return 0;        //2^2=4
        }
        break;

    case WM_KEYUP:
        switch (wParam)
        {
        case '1':
        case '2':
        case '4':
            return 0;
        }
        break;

    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, WindowProcSubclass, uIdSubclass);
        fInitSpeedHack(0);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
typedef HWND (WINAPI *PCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static HWND WINAPI CreateWindowExAStub(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    if (lpClassName)
    {
        const char *pStr = "CityRacer";
        while (*lpClassName == *pStr && *pStr)
            ++lpClassName, ++pStr;
        if (*lpClassName == *pStr)
            if (const HWND hWndApp = CreateWindowExW(dwExStyle, L"CityRacer", nullptr, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))
                if (SetWindowSubclass(hWndApp, WindowProcSubclass, 1, 0))
                    return hWndApp;
    }
    return nullptr;
}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE hInstDll, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        constexpr const DWORD dwPatchSize = 1+sizeof(size_t);
        BYTE *pAddress = reinterpret_cast<BYTE*>(QueryPerformanceCounter)-dwPatchSize;
        DWORD dwOldProtect;
        if (VirtualProtect(pAddress, dwPatchSize+2, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            size_t szOffset = reinterpret_cast<size_t>(QueryPerformanceCounterStub)-(reinterpret_cast<size_t>(pAddress) + dwPatchSize);
            const BYTE *pByte = static_cast<const BYTE*>(static_cast<const void*>(&szOffset));
            pAddress[0] = 0xE9;        //jump near
            pAddress[1] = pByte[0];
            pAddress[2] = pByte[1];
            pAddress[3] = pByte[2];
            pAddress[4] = pByte[3];
            pAddress[5] = 0xEB;        //jump short
            pAddress[6] = 0xF9;        //-7
            DWORD dwTemp;
            if (VirtualProtect(pAddress, dwPatchSize+2, dwOldProtect, &dwTemp))
            {
                QueryPerformanceCounterReal = reinterpret_cast<PQueryPerformanceCounter>(pAddress+dwPatchSize+2);
                pAddress = reinterpret_cast<BYTE*>(CreateWindowExA);
                if (VirtualProtect(pAddress, dwPatchSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    szOffset = reinterpret_cast<size_t>(CreateWindowExAStub) - (reinterpret_cast<size_t>(pAddress) + dwPatchSize);
                    pByte = static_cast<const BYTE*>(static_cast<const void*>(&szOffset));
                    pAddress[0] = 0xE9;        //jump near
                    pAddress[1] = pByte[0];
                    pAddress[2] = pByte[1];
                    pAddress[3] = pByte[2];
                    pAddress[4] = pByte[3];
                    if (VirtualProtect(pAddress, dwPatchSize, dwOldProtect, &dwTemp) && DisableThreadLibraryCalls(hInstDll))
                        return TRUE;
                }
            }
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        BYTE *const pAddress = reinterpret_cast<BYTE*>(QueryPerformanceCounter);
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
