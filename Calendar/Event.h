#pragma once
#include <string>

class Event
{
public:
    std::wstring title;
    std::wstring description;
    int day;
    int month;
    int year;
    int hour;
    int minute;

    Event(const std::wstring& t,
        const std::wstring& d,
        int day, int month, int year,
        int hour = -1, int minute = -1)
        : title(t), description(d),
        day(day), month(month), year(year),
        hour(hour), minute(minute)
    {
    }
    Event() {}
};
