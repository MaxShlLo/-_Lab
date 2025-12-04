#pragma once
#include <windows.h>
#include <vector>
#include "Event.h"

class Calendar
{
private:
    int month;
    int year;
    SYSTEMTIME currentDate;

    std::vector<Event> events;

public:
    Calendar();

    void nextMonth();
    void prevMonth();

    int getMonth() const { return month; }
    int getYear() const { return year; }

    int getFirstWeekday() const;
    int getDaysInMonth() const;



    bool isToday(int day) const;

    const wchar_t* getMonthName() const;
    const wchar_t** getWeekdayNames() const;

    // --- Події ---
    void addEvent(const Event& e);
    bool hasEvent(int day) const;
    const std::vector<Event>& getEvents() const { return events; }
    void loadEventsFromFile();
};
