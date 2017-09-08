//ExitWindows
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <powrprof.h>

//-------------------------------------------------------------------------------------------------
static bool FCompareMemoryW(const wchar_t *pBuf1, const wchar_t *pBuf2)
{
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

//-------------------------------------------------------------------------------------------------
void FMain()
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
                        return;
                    ++wCmdLine;
                }
                ++wCmdLine;
                if (*wCmdLine != L' ' && *wCmdLine != L'\t')
                    return;
            }
            else
                while (*wCmdLine != L' ' && *wCmdLine != L'\t')
                {
                    if (*wCmdLine == L'\0' || *wCmdLine == L'\"')
                        return;
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
                            return;
                        ++wCmdLine;
                    }
                    if (wCmdLine[1] != L' ' && wCmdLine[1] != L'\t' && wCmdLine[1] != L'\0')
                        return;
                    ++wArg;
                }
                else
                    while (*wCmdLine != L' ' && *wCmdLine != L'\t' && *wCmdLine != L'\0')
                        ++wCmdLine;

                if (*wArg == L'/')
                {
                    *wCmdLine = L'\0';
                    ++wArg;

                    if (FCompareMemoryW(wArg, L"lock"))
                        LockWorkStation();
                    else if (const HANDLE hProcess = GetCurrentProcess())
                    {
                        HANDLE hToken;
                        if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
                        {
                            bool bOk = false;
                            TOKEN_PRIVILEGES tokprv;
                            if (LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &tokprv.Privileges[0].Luid))
                            {
                                tokprv.PrivilegeCount = 1;
                                tokprv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                                if (AdjustTokenPrivileges(hToken, FALSE, &tokprv, 0, nullptr, nullptr))
                                    bOk = true;
                            }
                            CloseHandle(hToken);

                            if (bOk)
                            {
                                constexpr const DWORD dwReason = SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED;
                                if (FCompareMemoryW(wArg, L"sleep"))
                                    SetSuspendState(FALSE, FALSE, TRUE);
                                else if (FCompareMemoryW(wArg, L"hibernate"))
                                    SetSuspendState(TRUE, FALSE, TRUE);
                                else if (FCompareMemoryW(wArg, L"shutdown"))
                                    ExitWindowsEx(EWX_SHUTDOWN, dwReason);
                                else if (FCompareMemoryW(wArg, L"reboot"))
                                    ExitWindowsEx(EWX_REBOOT, dwReason);
                                else if (FCompareMemoryW(wArg, L"shutdown-force"))
                                    ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, dwReason);
                                else if (FCompareMemoryW(wArg, L"reboot-force"))
                                    ExitWindowsEx(EWX_REBOOT | EWX_FORCE, dwReason);
                                else if (FCompareMemoryW(wArg, L"logoff"))
                                    ExitWindowsEx(EWX_LOGOFF, dwReason);
                                else if (FCompareMemoryW(wArg, L"logoff-force"))
                                    ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, dwReason);
                                else if (FCompareMemoryW(wArg, L"hybrid"))
                                    ExitWindowsEx(EWX_SHUTDOWN | EWX_HYBRID_SHUTDOWN, dwReason);
                                else if (FCompareMemoryW(wArg, L"hybrid-force"))
                                    ExitWindowsEx(EWX_SHUTDOWN | EWX_HYBRID_SHUTDOWN | EWX_FORCE, dwReason);
                                else if (FCompareMemoryW(wArg, L"sleep-awake"))
                                    SetSuspendState(FALSE, FALSE, FALSE);
                                else if (FCompareMemoryW(wArg, L"hibernate-awake"))
                                    SetSuspendState(TRUE, FALSE, FALSE);
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
