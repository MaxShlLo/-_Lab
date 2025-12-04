#pragma once
#include <windows.h>
#include "Calendar.h"

class CalendarWindow
{
public:
    Calendar calendar;

    HWND editEventTitle;
    HWND editEventDescription;
    HWND editEventDate;
    HWND editEventTime;

    bool showAddEventInput = false;

    HWND btnPrev;
    HWND btnNext;
    HWND btnAddEvent;
    HWND btnSaveEvent;
    //HWND btnFindEvent;
    //HWND editFindEvent;
    //HWND btnSearchEvent;

    static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    void Draw(HDC hdc);
};
