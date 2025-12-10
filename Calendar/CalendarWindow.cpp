#include "CalendarWindow.h"
#include "Event.h"
#include <string>

LRESULT CALLBACK CalendarWindow::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CalendarWindow* self;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        self = (CalendarWindow*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
    }

    self = (CalendarWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!self)
        return DefWindowProc(hwnd, msg, wParam, lParam);

    return self->WndProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK CalendarWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        btnPrev = CreateWindow(L"BUTTON", L"<",
            WS_VISIBLE | WS_CHILD,
            250, 20, 40, 30,
            hwnd, (HMENU)1, nullptr, nullptr);

        btnNext = CreateWindow(L"BUTTON", L">",
            WS_VISIBLE | WS_CHILD,
            510, 20, 40, 30,
            hwnd, (HMENU)2, nullptr, nullptr);

        btnAddEvent = CreateWindow(L"BUTTON", L"Add Event",
            WS_VISIBLE | WS_CHILD,
            650, 20, 120, 30,
            hwnd, (HMENU)3, nullptr, nullptr);

        btnSaveEvent = CreateWindow(L"BUTTON", L"Save",
            WS_VISIBLE | WS_CHILD,
            650, 80, 80, 30,
            hwnd, (HMENU)4, nullptr, nullptr);

        break;

    case WM_COMMAND:
        if (wParam == 1) calendar.prevMonth();
        else if (wParam == 2) calendar.nextMonth();
        else if (wParam == 3) // Add Event
        {
            if (!showAddEventInput)
            {
                showAddEventInput = true;

                // ===== Поле назви події =====
                editEventTitle = CreateWindow(L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                    250, 400, 200, 25, hwnd, nullptr, nullptr, nullptr);

                // ===== Поле опису події (3 рядки) =====
                editEventDescription = CreateWindow(L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
                    250, 440, 250, 60, hwnd, nullptr, nullptr, nullptr);

                // ===== Поле дати =====
                editEventDate = CreateWindow(L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                    470, 400, 100, 25, hwnd, nullptr, nullptr, nullptr);

                // ===== Поле часу =====
                editEventTime = CreateWindow(L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                    590, 400, 80, 25, hwnd, nullptr, nullptr, nullptr);
            }
        }
        else if (wParam == 4) // Save Event
        {
            wchar_t titleBuf[256];
            wchar_t descBuf[512];
            wchar_t dateBuf[32];
            wchar_t timeBuf[32];

            // зчитуємо введене
            GetWindowTextW(editEventTitle, titleBuf, 256);
            GetWindowTextW(editEventDescription, descBuf, 512);
            GetWindowTextW(editEventDate, dateBuf, 32);
            GetWindowTextW(editEventTime, timeBuf, 32);

            // ----------------------------------------------------
            // Парсинг дати
            // ----------------------------------------------------
            int day = 0, month = 0;
            swscanf_s(dateBuf, L"%d.%d", &day, &month);

            // ----------------------------------------------------
            // Парсинг часу
            // ----------------------------------------------------
            int hour = -1, minute = -1;
            swscanf_s(timeBuf, L"%d:%d", &hour, &minute);

            // ----------------------------------------------------
            // Створюємо подію
            // ----------------------------------------------------
            Event e(titleBuf, descBuf,
                day, month, calendar.getYear(),
                hour, minute);

            // додаємо в календар
            calendar.addEvent(e);

            // ----------------------------------------------------
            // Запис у файл
            // ----------------------------------------------------
            HANDLE file = CreateFile(L"events.txt", GENERIC_WRITE, 0, nullptr,
                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (file != INVALID_HANDLE_VALUE)
            {
                SetFilePointer(file, 0, nullptr, FILE_END);

                // Додаємо порожній рядок перед новою подією
                const wchar_t* separator = L"\r\n";
                DWORD written;
                WriteFile(file, separator, wcslen(separator) * sizeof(wchar_t), &written, nullptr);

                std::wstring line =
                    L"Title: " + std::wstring(titleBuf) + L"\r\n" +
                    L"Description: " + std::wstring(descBuf) + L"\r\n" +
                    L"Date: " + std::wstring(dateBuf) + L"\r\n" +
                    L"Time: " + std::wstring(timeBuf) + L"\r\n" +
                    L"-------------\r\n";

                //DWORD written;
                WriteFile(file, line.c_str(),
                    line.size() * sizeof(wchar_t),
                    &written, nullptr);

                CloseHandle(file);
            }

            MessageBox(hwnd, L"Подію збережено!", L"OK", MB_OK);
        }


        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Draw(hdc);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CalendarWindow::Draw(HDC hdc)
{
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER);

    wchar_t header[64];
    wsprintf(header, L"%s %d", calendar.getMonthName(), calendar.getYear());
    TextOut(hdc, 400, 20, header, wcslen(header));

    const wchar_t** days = calendar.getWeekdayNames();
    for (int i = 0; i < 7; i++)
        TextOut(hdc, 100 + i * 80, 60, days[i], 2);

    int first = calendar.getFirstWeekday();
    int total = calendar.getDaysInMonth();

    for (int d = 1; d <= total; ++d)
    {
        int idx = d + first - 2;
        int row = idx / 7;
        int col = idx % 7;

        int x = 100 + col * 80;
        int y = 100 + row * 50;

        if (calendar.isToday(d))
        {
            HPEN pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
            HGDIOBJ old = SelectObject(hdc, pen);
            Rectangle(hdc, x - 10, y - 5, x + 25, y + 20);
            SelectObject(hdc, old);
            DeleteObject(pen);
        }

        // ======= Позначка події ========
        if (calendar.hasEvent(d))
        {
            HBRUSH brush = CreateSolidBrush(RGB(0, 0, 255));
            HGDIOBJ oldBrush = SelectObject(hdc, brush);

            Ellipse(hdc, x + 5, y + 15, x + 15, y + 25);

            SelectObject(hdc, oldBrush);
            DeleteObject(brush);
        }

        if (showAddEventInput)
        {
            SetTextColor(hdc, RGB(0, 0, 255));

            TextOut(hdc, 200, 400, L"Назва:", wcslen(L"Назва:"));
            TextOut(hdc, 200, 440, L"Опис:", wcslen(L"Опис:"));
            TextOut(hdc, 520, 380, L"Дата (дд.мм):", wcslen(L"Дата (дд.мм):"));
            TextOut(hdc, 630, 380, L"Час (гг:хх):", wcslen(L"Час (гг:хх):"));

            SetTextColor(hdc, RGB(0, 0, 0));
        }

        wchar_t buf[4];
        wsprintf(buf, L"%d", d);
        TextOut(hdc, x, y, buf, wcslen(buf));

// ============================
// Додати підпис події (назву)
// ============================
        if (calendar.hasEvent(d))
        {
            const auto& evs = calendar.getEvents();

            for (const auto& e : evs)
            {
                if (e.day == d && e.month == calendar.getMonth() && e.year == calendar.getYear())
                {
                    std::wstring cleanTitle = e.title;

                    // прибираємо переводи рядків на початку
                    while (!cleanTitle.empty() &&
                        (cleanTitle[0] == L'\r' || cleanTitle[0] == L'\n'))
                        cleanTitle.erase(0, 1);

                    std::wstring label = cleanTitle.substr(0, 8);
                    if (cleanTitle.size() > 8)
                        label += L"...";

                    SetTextColor(hdc, RGB(0, 0, 200));
                    TextOut(hdc, x, y + 20, label.c_str(), label.size());
                    SetTextColor(hdc, RGB(0, 0, 0));

                    break;
                }
            }
        }
    }
}
