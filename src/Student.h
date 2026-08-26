#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include "TimeSlot.h"
#include "LinkedList.h"
using namespace std;

// Entries compare equal by courseId so the list can find/remove by course.
struct TimetableEntry {
    string   courseId;
    string   title;
    string   instructor;
    TimeSlot slot;

    TimetableEntry() {}
    TimetableEntry(const string& id, const string& t,
                   const string& inst, const TimeSlot& s)
        : courseId(id), title(t), instructor(inst), slot(s) {}

    bool operator==(const TimetableEntry& other) const {
        return courseId == other.courseId;
    }
};

class Student {
public:
    string id;
    string name;
    int    year;   // 1..4, used for waitlist priority

    LinkedList<TimetableEntry> timetable;

    Student() : year(1) {}
    Student(const string& sid, const string& sname, int yr)
        : id(sid), name(sname), year(yr) {}
};

#endif
