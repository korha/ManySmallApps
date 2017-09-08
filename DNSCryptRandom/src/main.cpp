//DNSCryptRandom
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//-------------------------------------------------------------------------------------------------
void FMain()
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\dnscrypt-proxy\\Parameters", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        constexpr const wchar_t *const wResolverNames[8] =
        {
            L"dnscrypt.eu-dk",
            L"dnscrypt.eu-nl",
            L"dnscrypt.nl-ns0",
            L"dnscrypt.org-fr",
            L"ipredator",
            L"ns0.dnscrypt.is",
            L"nxd.ist",
            L"ventricle.us"
        };
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        const wchar_t *const wName = wResolverNames[ft.dwLowDateTime & 0b111];
        const wchar_t *wIt = wName;
        while (*wIt++);
        const bool bOk = (RegSetValueExW(hKey, L"ResolverName", 0, REG_SZ, static_cast<const BYTE*>(static_cast<const void*>(wName)), (wIt-wName)*sizeof(wchar_t)) == ERROR_SUCCESS);
        if (RegCloseKey(hKey) == ERROR_SUCCESS && bOk)
        {
            PROCESS_INFORMATION pi;
            STARTUPINFO si;
            DWORD dwSize = sizeof(STARTUPINFO);
            BYTE *pDst = static_cast<BYTE*>(static_cast<void*>(&si));
            while (dwSize--)
                *pDst++ = '\0';
            si.cb = sizeof(STARTUPINFO);
            wchar_t wBuf[] = L"C:\\Windows\\System32\\net.exe stop  dnscrypt-proxy";
            if (CreateProcessW(nullptr, wBuf, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW, nullptr, L"C:\\Windows\\System32\\", &si, &pi))
            {
                CloseHandle(pi.hThread);
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                wBuf[30] = L'a';
                wBuf[31] = L'r';
                wBuf[32] = L't';
                if (CreateProcessW(nullptr, wBuf, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW, nullptr, L"C:\\Windows\\System32\\", &si, &pi))
                {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
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
