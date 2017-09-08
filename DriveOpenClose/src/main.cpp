//DriveOpenClose
#define _WIN32_WINNT _WIN32_IE_WINBLUE
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
                *wCmdLine = L'\0';

                if (*wArg >= L'a' && *wArg <= L'z' && wArg[1] == L'\0')
                {
                    wchar_t wCommand[] = L"set cdaudio!x: door open wait";
                    wCommand[12] = *wArg;
                    mciSendStringW(wCommand, nullptr, 0, nullptr);
                }
                else if (*wArg >= L'A' && *wArg <= L'Z' && wArg[1] == L'\0')
                {
                    wchar_t wCommand[] = L"set cdaudio!x: door closed wait";
                    wCommand[12] = *wArg+(L'a'-L'A');
                    mciSendStringW(wCommand, nullptr, 0, nullptr);
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
