//ExternalRun
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
                PROCESS_INFORMATION pi;
                STARTUPINFO si;
                DWORD dwSize = sizeof(STARTUPINFO);
                BYTE *pDst = static_cast<BYTE*>(static_cast<void*>(&si));
                while (dwSize--)
                    *pDst++ = '\0';
                si.cb = sizeof(STARTUPINFO);
                if (CreateProcessW(nullptr, wCmdLine, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT, nullptr, nullptr, &si, &pi))
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
