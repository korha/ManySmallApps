//BloodyFix
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//#define USER_TEST

#ifdef USER_TEST
#include <climits>
//-------------------------------------------------------------------------------------------------
static void FTest(const WORD wYear, const WORD wMonth, const WORD wDay, const WORD wHour, const WORD wMinute, const WORD wSecond)
{
    if (wYear >= 2015 && wYear < 2038 && wMonth >= 1 && wMonth <= 12 && wDay >= 1 && wDay <= 31 && wMinute < 60 && wSecond < 60)
    {
        SYSTEMTIME sysTime;
        sysTime.wYear = wYear;
        sysTime.wMonth = wMonth;
        sysTime.wDayOfWeek = 0;
        sysTime.wDay = wDay;
        sysTime.wHour = wHour;
        sysTime.wMinute = wMinute;
        sysTime.wSecond = wSecond;
        sysTime.wMilliseconds = 0;
        FILETIME fileTime;
        if (SystemTimeToFileTime(&sysTime, &fileTime))
        {
            constexpr const wchar_t *const wHex = L"0123456789ABCDEF";
            constexpr const DWORD dwSize = (2+sizeof(DWORD)*2+2)*2;
            wchar_t wBuf[dwSize];        //e.g.: "0xFFFFFFFFU|0xFFFFFFFFU`"
            wchar_t *wIt = wBuf + dwSize;
            *--wIt = L'\0';
            *--wIt = L'U';
            wchar_t *wBeg = wIt-sizeof(DWORD)*2;
            do
            {
                *--wIt = wHex[fileTime.dwLowDateTime & 0xF];
                fileTime.dwLowDateTime >>= CHAR_BIT/2;
            } while (wIt > wBeg);
            *--wIt = L'x';
            *--wIt = L'0';
            *--wIt = L'|';
            *--wIt = L'U';
            wBeg = wIt-sizeof(DWORD)*2;
            do
            {
                *--wIt = wHex[fileTime.dwHighDateTime & 0xF];
                fileTime.dwHighDateTime >>= CHAR_BIT/2;
            } while (wIt > wBeg);
            *--wIt = L'x';
            *--wIt = L'0';

            if (wIt == wBuf)
                MessageBoxW(nullptr, wBuf, L"BloodyFix", MB_ICONINFORMATION);
        }
    }
}
#endif

//-------------------------------------------------------------------------------------------------
void FMain()
{
#ifdef USER_TEST
    FTest(2016, 7, 22, 16, 42, 19);
#else
    bool bOk = false;
    wchar_t wBuf[MAX_PATH+1];
    const DWORD dwLen = GetModuleFileNameW(nullptr, wBuf, MAX_PATH+1);
    if (dwLen >= 4 && dwLen < MAX_PATH)
    {
        wchar_t *pDelim = wBuf+dwLen;
        do
        {
            if (*--pDelim == L'\\')
                break;
        } while (pDelim > wBuf);
        if (pDelim >= wBuf+2 && pDelim <= wBuf+MAX_PATH-13)        //"\Bloody?.exe`"
        {
            FILETIME ftLastWrite, ftLastWriteLocal;
            ftLastWrite.dwHighDateTime = 0x01D1E438U;
            ftLastWrite.dwLowDateTime = 0x02FEA780U;
            if (LocalFileTimeToFileTime(&ftLastWrite, &ftLastWriteLocal))
            {
                *++pDelim = L'B';
                *++pDelim = L'l';
                *++pDelim = L'o';
                *++pDelim = L'o';
                *++pDelim = L'd';
                *++pDelim = L'y';
                *++pDelim = L'6';
                *++pDelim = L'.';
                *++pDelim = L'e';
                *++pDelim = L'x';
                *++pDelim = L'e';
                *++pDelim = L'\0';
                const HANDLE hFile = CreateFileW(wBuf, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    if (SetFileTime(hFile, nullptr, nullptr, &ftLastWriteLocal))
                        bOk = true;
                    CloseHandle(hFile);
                }
            }
        }
    }
    if (bOk)
        MessageBoxW(nullptr, L"Success", L"BloodyFix", MB_ICONINFORMATION);
    else
        MessageBoxW(nullptr, L"Fail", L"BloodyFix", MB_ICONERROR);
#endif
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
