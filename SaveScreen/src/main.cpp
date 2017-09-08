//SaveScreen
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

                HWND hWndDc = nullptr;
                RECT rect;
                rect.right = 0;
                if (*wArg == L'f')
                {
                    rect.left = 0;
                    rect.top = 0;
                    rect.right = GetSystemMetrics(SM_CXSCREEN);
                    rect.bottom = GetSystemMetrics(SM_CYSCREEN);
                }
                else if (*wArg == L'w')
                {
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        WINDOWPLACEMENT wndPlace;
                        wndPlace.length = sizeof(WINDOWPLACEMENT);
                        if (GetWindowPlacement(hWnd, &wndPlace))
                        {
                            if (wndPlace.showCmd == SW_SHOWMAXIMIZED)
                            {
                                RECT rectWindow;
                                if (GetWindowRect(hWnd, &rectWindow))
                                {
                                    const int iWidth = GetSystemMetrics(SM_CXSCREEN);
                                    if (rectWindow.left < 0 && rectWindow.right > iWidth)        //fullscreen
                                    {
                                        rect.left = 0;
                                        rect.top = 0;
                                        rect.right = iWidth;
                                        rect.bottom = GetSystemMetrics(SM_CYSCREEN);
                                    }
                                    else
                                        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0);        //maximized, but not fullscreen (need exclude taskbar)
                                }
                            }
                            else if (GetWindowRect(hWnd, &rect))
                            {
                                const int iWidth = GetSystemMetrics(SM_CXSCREEN),
                                        iHeight = GetSystemMetrics(SM_CYSCREEN);
                                if (rect.left < 0)
                                    rect.left = 0;
                                if (rect.top < 0)
                                    rect.top = 0;
                                if (rect.right > iWidth)
                                    rect.right = iWidth;
                                if (rect.bottom > iHeight)
                                    rect.bottom = iHeight;
                                rect.right -= rect.left;
                                rect.bottom -= rect.top;
                            }
                        }
                    }
                }
                else if (*wArg == L'a')        //client area of window
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        hWndDc = hWnd;
                        GetClientRect(hWnd, &rect);
                    }

                if (rect.right)
                {
                    *wCmdLine = L'\0';
                    ++wArg;
                    const wchar_t *wIt = wArg;
                    while (*wIt++);
                    const DWORD dwLen = wIt-wArg-1;
                    if (dwLen >= 21)        //"yyyyMMddhhmmsszzz.bmp"
                        if (const HDC hDc = GetDC(hWndDc))
                        {
                            if (const HDC hDcMem = CreateCompatibleDC(hDc))
                            {
                                if (const HANDLE hProcHeap = GetProcessHeap())
                                {
                                    BITMAPFILEHEADER bitmapFileHeader;
                                    BITMAPINFOHEADER bitmapInfoHeader;
                                    void *pBytes = nullptr;
                                    if (const HBITMAP hBitmap = CreateCompatibleBitmap(hDc, rect.right, rect.bottom))
                                    {
                                        const HGDIOBJ hObjSave = SelectObject(hDcMem, hBitmap);
                                        const bool bBitBlt = BitBlt(hDcMem, 0, 0, rect.right, rect.bottom, hDc, rect.left, rect.top, SRCCOPY);
                                        ReleaseDC(hWndDc, hDc);
                                        if (bBitBlt)
                                        {
                                            BITMAP bitmap;
                                            if (GetObjectW(hBitmap, sizeof(BITMAP), &bitmap) == sizeof(BITMAP) && bitmap.bmType == 0 &&
                                                    bitmap.bmWidth > 1 && bitmap.bmHeight > 1 &&
                                                    bitmap.bmPlanes == 1 && bitmap.bmBitsPixel == 32)
                                            {
                                                bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
                                                bitmapInfoHeader.biWidth = bitmap.bmWidth;
                                                bitmapInfoHeader.biHeight = bitmap.bmHeight;
                                                bitmapInfoHeader.biPlanes = 1;
                                                bitmapInfoHeader.biBitCount = 24;
                                                bitmapInfoHeader.biCompression = BI_RGB;        //uncompressed
                                                bitmapInfoHeader.biSizeImage = ((bitmapInfoHeader.biWidth*24U + 31) & ~31/*[/32*32]*/)/8*bitmapInfoHeader.biHeight;        //The width must be DWORD aligned
                                                bitmapInfoHeader.biXPelsPerMeter = 0;
                                                bitmapInfoHeader.biYPelsPerMeter = 0;
                                                bitmapInfoHeader.biClrUsed = 0;
                                                bitmapInfoHeader.biClrImportant = 0;        //all colors

                                                if ((pBytes = HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, bitmapInfoHeader.biSizeImage)))
                                                    if (GetDIBits(hDcMem, hBitmap, 0, bitmapInfoHeader.biHeight, pBytes, static_cast<BITMAPINFO*>(static_cast<void*>(&bitmapInfoHeader)), DIB_RGB_COLORS) != bitmapInfoHeader.biHeight)
                                                    {
                                                        HeapFree(hProcHeap, HEAP_NO_SERIALIZE, pBytes);
                                                        pBytes = nullptr;
                                                    }
                                            }
                                        }
                                        SelectObject(hDcMem, hObjSave);
                                        DeleteObject(hBitmap);
                                    }
                                    else
                                        ReleaseDC(hWndDc, hDc);
                                    DeleteDC(hDcMem);

                                    if (pBytes)
                                    {
                                        SYSTEMTIME systime;
                                        GetLocalTime(&systime);
                                        wCmdLine = wArg;
                                        wArg = wArg+dwLen-21;
                                        WORD wTemp = systime.wYear/1000;
                                        *wArg = L'0' + wTemp;
                                        systime.wYear -= wTemp*1000;
                                        wTemp = systime.wYear/100;
                                        *++wArg = L'0' + wTemp;
                                        systime.wYear -= wTemp*100;
                                        wTemp = systime.wYear/10;
                                        *++wArg = L'0' + wTemp;
                                        systime.wYear -= wTemp*10;
                                        *++wArg = L'0' + systime.wYear;
                                        *++wArg = L'0' + systime.wMonth/10;
                                        *++wArg = L'0' + systime.wMonth%10;
                                        *++wArg = L'0' + systime.wDay/10;
                                        *++wArg = L'0' + systime.wDay%10;
                                        *++wArg = L'0' + systime.wHour/10;
                                        *++wArg = L'0' + systime.wHour%10;
                                        *++wArg = L'0' + systime.wMinute/10;
                                        *++wArg = L'0' + systime.wMinute%10;
                                        *++wArg = L'0' + systime.wSecond/10;
                                        *++wArg = L'0' + systime.wSecond%10;
                                        wTemp = systime.wMilliseconds/100;
                                        *++wArg = L'0' + wTemp;
                                        systime.wMilliseconds -= wTemp*100;
                                        wTemp = systime.wMilliseconds/10;
                                        *++wArg = L'0' + wTemp;
                                        systime.wMilliseconds -= wTemp*10;
                                        *++wArg = L'0' + systime.wMilliseconds;
                                        const HANDLE hFile = CreateFileW(wCmdLine, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                                        if (hFile != INVALID_HANDLE_VALUE)
                                        {
                                            bitmapFileHeader.bfType = 0x4D42;        //BM signature
                                            bitmapFileHeader.bfReserved1 = 0;
                                            bitmapFileHeader.bfReserved2 = 0;
                                            bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + bitmapInfoHeader.biSize;
                                            bitmapFileHeader.bfSize = bitmapFileHeader.bfOffBits + bitmapInfoHeader.biSizeImage;
                                            DWORD dwBytes;
                                            if (WriteFile(hFile, &bitmapFileHeader, sizeof(BITMAPFILEHEADER), &dwBytes, nullptr) && dwBytes == sizeof(BITMAPFILEHEADER) &&
                                                    WriteFile(hFile, &bitmapInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytes, nullptr) && dwBytes == sizeof(BITMAPINFOHEADER))
                                                WriteFile(hFile, pBytes, bitmapInfoHeader.biSizeImage, &dwBytes, nullptr);
                                            CloseHandle(hFile);
                                        }
                                        HeapFree(hProcHeap, HEAP_NO_SERIALIZE, pBytes);
                                    }
                                }
                            }
                            else
                                ReleaseDC(hWndDc, hDc);
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
