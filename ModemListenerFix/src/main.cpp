//ModemListenerFix
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef _WIN64
#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
EXPORT HGDIOBJ WINAPI GetStockObjectStub(int)
{
    return nullptr;
}

//-------------------------------------------------------------------------------------------------
typedef WINBOOL (WINAPI *PGetMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
static PGetMessageA GetMessageAReal;
static WINBOOL WINAPI GetMessageAStub(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    static bool bIsFirst = true;
    WINBOOL bResult = GetMessageAReal(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    if (lpMsg->message == WM_TIMER)
    {
        if (bIsFirst)
            bIsFirst = false;
        else
            bResult = FALSE;
    }
    return bResult;
}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        constexpr const DWORD dwPatchSize = 1+sizeof(size_t);
        BYTE *const pAddress = reinterpret_cast<BYTE*>(GetMessageA)-dwPatchSize;
        DWORD dwOldProtect;
        if (VirtualProtect(pAddress, dwPatchSize+2, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            const size_t szOffset = reinterpret_cast<size_t>(GetMessageAStub)-(reinterpret_cast<size_t>(pAddress) + dwPatchSize);
            const BYTE *const pByte = static_cast<const BYTE*>(static_cast<const void*>(&szOffset));
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
                GetMessageAReal = reinterpret_cast<PGetMessageA>(pAddress+dwPatchSize+2);
                return TRUE;
            }
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
