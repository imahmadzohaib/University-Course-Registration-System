#ifndef COURSE_H
#define COURSE_H

#include <string>
#include "TimeSlot.h"
#include "Queue.h"
#include "LinkedList.h"
using namespace std;

class Course {
public:
    string   courseId;
    string   title;
    string   instructor;
    int      capacity;
    int      enrolled;
    TimeSlot slot;

    LinkedList<string> enrolledStudents;
    Queue<string>      waitlist;

    double ratingSum;
    int    ratingCount;
    int    totalRegistrations;

    Course()
        : capacity(0), enrolled(0),
          ratingSum(0.0), ratingCount(0), totalRegistrations(0) {}

    Course(const string& id, const string& t, const string& inst,
           int cap, const TimeSlot& s)
        : courseId(id), title(t), instructor(inst),
          capacity(cap), enrolled(0), slot(s),
          ratingSum(0.0), ratingCount(0), totalRegistrations(0) {}

    bool   isFull()        const { return enrolled >= capacity; }
    int    seatsLeft()     const { return capacity - enrolled; }
    double averageRating() const {
        return ratingCount == 0 ? 0.0 : ratingSum / ratingCount;
    }
};

#endif
