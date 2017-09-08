//IdnToUnicode
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//-------------------------------------------------------------------------------------------------
static inline bool FIsStartWithW(const wchar_t *pFullStr, const wchar_t *pBeginStr)
{
    while (*pBeginStr)
        if (*pFullStr++ != *pBeginStr++)
            return false;
    return true;
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
                *wCmdLine = L'\0';

                while (*wArg == L' ' || *wArg == L'\t')
                    ++wArg;
                if (*wArg != L'\0')
                {
                    if (FIsStartWithW(wArg, L"https://"))
                        wArg += 8;
                    else if (FIsStartWithW(wArg, L"http://"))
                        wArg += 7;
                    else if (FIsStartWithW(wArg, L"ftp://"))
                        wArg += 6;
                    else if (FIsStartWithW(wArg, L"wss://"))
                        wArg += 6;
                    else if (FIsStartWithW(wArg, L"ws://"))
                        wArg += 5;
                    wCmdLine = wArg;
                    while (true)
                    {
                        if (*wCmdLine == L'\0' || *wCmdLine == L'/' || *wCmdLine == L':' || *wCmdLine == L' ' || *wCmdLine == L'\t')
                        {
                            *wCmdLine++ = L'\0';
                            break;
                        }
                        ++wCmdLine;
                    }
                    if (IdnToUnicode(IDN_USE_STD3_ASCII_RULES, wArg, wCmdLine-wArg, wArg, wCmdLine-wArg) > 0)
                        MessageBoxW(nullptr, wArg, L"IdnToUnicode", MB_ICONINFORMATION);
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
