#include <windows.h>
#include "CalendarWindow.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"CalendarApp - Maks Shilo";

    WNDCLASS wc = {};
    wc.lpfnWndProc = CalendarWindow::StaticWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    CalendarWindow* window = new CalendarWindow();

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"CalendarApp - Maks Shilo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        nullptr, nullptr, hInst, window
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete window;
    return 0;
}
