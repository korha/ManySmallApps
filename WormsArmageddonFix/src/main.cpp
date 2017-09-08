//WormsArmageddonFix
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <ddraw.h>
#include <commctrl.h>

#ifndef _WIN64
#define EXPORT //__declspec(dllexport)

static LARGE_INTEGER g_liInitStub, g_liInitReal;
static bool g_bIsSpeedHack = false, g_bIsFast;

//-------------------------------------------------------------------------------------------------
static inline bool FCompareMemoryA(const char *pBuf1)
{
    const char *pBuf2 = "Worms2D";
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

//-------------------------------------------------------------------------------------------------
EXPORT HRESULT WINAPI DirectDrawCreateStub(GUID *, LPDIRECTDRAW *, IUnknown *)
{
    return DDERR_INVALIDPARAMS;
}

EXPORT HRESULT WINAPI DirectDrawCreateExStub(GUID *, LPVOID *, REFIID, IUnknown *)
{
    return DDERR_INVALIDPARAMS;
}

EXPORT HRESULT WINAPI DirectDrawEnumerateExAStub(LPDDENUMCALLBACKEXA, LPVOID, DWORD)
{
    return DDERR_INVALIDPARAMS;
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
    if (g_bIsSpeedHack)
    {
        if (g_bIsFast)
            liCurrent.QuadPart <<= 3;
        else
            liCurrent.QuadPart >>= 1;
    }
    liCurrent.QuadPart += g_liInitStub.QuadPart;
    lpPerformanceCount->QuadPart = liCurrent.QuadPart;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
static inline void fInitSpeedHack()
{
    QueryPerformanceCounterStub(&g_liInitStub);
    QueryPerformanceCounterReal(&g_liInitReal);
}

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProcSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case 'W': wParam = VK_UP; break;
        case 'A': wParam = VK_LEFT; break;
        case 'S': wParam = VK_DOWN; break;
        case 'D': wParam = VK_RIGHT; break;
        case VK_OEM_COMMA: fInitSpeedHack(); g_bIsSpeedHack = true; g_bIsFast = false; return 0;
        case VK_OEM_PERIOD: fInitSpeedHack(); g_bIsSpeedHack = true; g_bIsFast = true; return 0;
        case VK_OEM_2: fInitSpeedHack(); g_bIsSpeedHack = false; return 0;
        }
        break;

    case WM_KEYUP:
        switch (wParam)
        {
        case 'W': wParam = VK_UP; break;
        case 'A': wParam = VK_LEFT; break;
        case 'S': wParam = VK_DOWN; break;
        case 'D': wParam = VK_RIGHT; break;
        case VK_OEM_COMMA:
        case VK_OEM_PERIOD:
        case VK_OEM_2:
            return 0;
        }
        break;

    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, WindowProcSubclass, uIdSubclass);
        fInitSpeedHack();
        g_bIsSpeedHack = false;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
typedef HWND (WINAPI *PCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static PCreateWindowExA CreateWindowExAReal;
static HWND WINAPI CreateWindowExAStub(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    const HWND hWnd = CreateWindowExAReal(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && lpClassName && lpWindowName && FCompareMemoryA(lpClassName) && FCompareMemoryA(lpWindowName))
        SetWindowSubclass(hWnd, WindowProcSubclass, 1, 0);
    return hWnd;
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
                pAddress = reinterpret_cast<BYTE*>(CreateWindowExA)-dwPatchSize;
                if (VirtualProtect(pAddress, dwPatchSize+2, PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    szOffset = reinterpret_cast<size_t>(CreateWindowExAStub)-(reinterpret_cast<size_t>(pAddress) + dwPatchSize);
                    pByte = static_cast<const BYTE*>(static_cast<const void*>(&szOffset));
                    pAddress[0] = 0xE9;        //jump near
                    pAddress[1] = pByte[0];
                    pAddress[2] = pByte[1];
                    pAddress[3] = pByte[2];
                    pAddress[4] = pByte[3];
                    pAddress[5] = 0xEB;        //jump short
                    pAddress[6] = 0xF9;        //-7
                    if (VirtualProtect(pAddress, dwPatchSize+2, dwOldProtect, &dwTemp) && DisableThreadLibraryCalls(hInstDll))
                    {
                        CreateWindowExAReal = reinterpret_cast<PCreateWindowExA>(pAddress+dwPatchSize+2);
                        return TRUE;
                    }
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
