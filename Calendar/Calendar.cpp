#include "Calendar.h"
#include "Event.h"
#include <string>
#include <windows.h>
#include <vector>
#include <iostream> 
#include <sstream>

Calendar::Calendar()
{
    GetLocalTime(&currentDate);
    month = currentDate.wMonth;
    year = currentDate.wYear;

    loadEventsFromFile();
}

void Calendar::nextMonth()
{
    month++;
    if (month > 12) { month = 1; year++; }
}

void Calendar::prevMonth()
{
    month--;
    if (month < 1) { month = 12; year--; }
}

const wchar_t* Calendar::getMonthName() const
{
    static const wchar_t* months[] =
    {
        L"Січень", L"Лютий", L"Березень", L"Квітень",
        L"Травень", L"Червень", L"Липень", L"Серпень",
        L"Вересень", L"Жовтень", L"Листопад", L"Грудень"
    };
    return months[month - 1];
}

const wchar_t** Calendar::getWeekdayNames() const
{
    static const wchar_t* days[7] =
    { L"Пн", L"Вт", L"Ср", L"Чт", L"Пт", L"Сб", L"Нд" };
    return days;
}

int Calendar::getFirstWeekday() const
{
    SYSTEMTIME st = {};
    st.wYear = year;
    st.wMonth = month;
    st.wDay = 1;

    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);

    SYSTEMTIME conv;
    FileTimeToSystemTime(&ft, &conv);

    int w = conv.wDayOfWeek;
    if (w == 0) w = 7;

    return w;
}

int Calendar::getDaysInMonth() const
{
    static const int days[12] =
    { 31,28,31,30,31,30,31,31,30,31,30,31 };

    if (month == 2)
    {
        if ((year % 400 == 0) ||
            (year % 4 == 0 && year % 100 != 0))
            return 29;
    }
    return days[month - 1];
}

bool Calendar::isToday(int day) const
{
    return (year == currentDate.wYear &&
        month == currentDate.wMonth &&
        day == currentDate.wDay);
}

// =====================
// Події
// =====================
void Calendar::addEvent(const Event& e)
{
    events.push_back(e);
}

bool Calendar::hasEvent(int day) const
{
    for (const auto& e : events)
        if (e.day == day && e.month == month && e.year == year)
            return true;

    return false;
}


void Calendar::loadEventsFromFile()
{
    events.clear();

    HANDLE file = CreateFileW(
        L"events.txt",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE)
        return;

    DWORD fileSizeBytes = GetFileSize(file, NULL);
    if (fileSizeBytes == 0 || fileSizeBytes == INVALID_FILE_SIZE)
    {
        CloseHandle(file);
        return;
    }

    // Для wchar_t — байти треба ділити на 2
    int wcharCount = fileSizeBytes / sizeof(wchar_t);

    // ДИВИСЬ! Ми робимо buffer НЕ const
    std::wstring buffer;
    buffer.resize(wcharCount);

    DWORD bytesRead = 0;

    // ReadFile хоче void*, тому даємо &buffer[0]
    BOOL ok = ReadFile(
        file,
        &buffer[0],
        fileSizeBytes,
        &bytesRead,
        NULL
    );

    CloseHandle(file);

    if (!ok)
        return;

    // ---------------------------
    //     ПАРСИНГ ФАЙЛУ
    // ---------------------------
    std::wstring line;
    Event temp;

    for (int i = 0; i < wcharCount; i++)
    {
        wchar_t c = buffer[i];

        if (c == L'\r' || c == L'\n')
        {
            if (line.empty())
            {
                continue;
            }

            // Title:
            if (line.rfind(L"Title:", 0) == 0)
            {
                temp.title = line.substr(6);
                if (!temp.title.empty() && temp.title[0] == L' ')
                    temp.title.erase(0, 1);
            }
            // Description:
            else if (line.rfind(L"Description:", 0) == 0)
            {
                temp.description = line.substr(12);
                if (!temp.description.empty() && temp.description[0] == L' ')
                    temp.description.erase(0, 1);
            }
            // Date:
            else if (line.rfind(L"Date:", 0) == 0)
            {
                int d = 0, m = 0;
                swscanf_s(line.c_str() + 5, L"%d.%d", &d, &m);
                temp.day = d;
                temp.month = m;
                temp.year = year;
            }
            // Time:
            else if (line.rfind(L"Time:", 0) == 0)
            {
                int h = -1, mn = -1;
                swscanf_s(line.c_str() + 5, L"%d:%d", &h, &mn);
                temp.hour = h;
                temp.minute = mn;
            }
            // Separator:
            else if (line.find(L"-------------") != std::wstring::npos)
            {
                events.push_back(temp);
                temp = Event();
            }

            line.clear();
        }
        else
        {
            line.push_back(c);
        }
    }
}
