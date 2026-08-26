#ifndef TIMESLOT_H
#define TIMESLOT_H

#include <string>
using namespace std;

// Time is a 24-hour integer: 09:00 -> 900, 13:30 -> 1330.
struct TimeSlot {
    string day;
    int    start;
    int    end;

    TimeSlot() : day(""), start(0), end(0) {}
    TimeSlot(const string& d, int s, int e) : day(d), start(s), end(e) {}
};

// Same day and overlapping time ranges.
inline bool clash(const TimeSlot& a, const TimeSlot& b) {
    if (a.day != b.day) return false;
    return (a.start < b.end) && (b.start < a.end);
}

#endif
