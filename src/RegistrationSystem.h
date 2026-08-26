#ifndef REGISTRATIONSYSTEM_H
#define REGISTRATIONSYSTEM_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "CourseBST.h"
#include "Student.h"
#include "Course.h"
#include "TimeSlot.h"
using namespace std;

class RegistrationSystem {
private:
    unordered_map<string, Student*> students;   // studentId -> Student
    CourseBST                       courses;     // courseId  -> Course

    static string formatTime(int t) {
        int hh = t / 100, mm = t % 100;
        string s;
        if (hh < 10) s += "0";
        s += to_string(hh);
        s += ":";
        if (mm < 10) s += "0";
        s += to_string(mm);
        return s;
    }

    Student* findStudent(const string& id) const {
        auto it = students.find(id);
        return it == students.end() ? nullptr : it->second;
    }

    bool hasClash(Student* st, const TimeSlot& slot, string& clashWith) const {
        for (LinkedList<TimetableEntry>::Node* n = st->timetable.getHead();
             n != nullptr; n = n->next) {
            if (clash(n->data.slot, slot)) {
                clashWith = n->data.courseId;
                return true;
            }
        }
        return false;
    }

    static double popularityScore(Course* c) {
        return c->enrolled + c->waitlist.size() + c->averageRating();
    }

    void promoteFromWaitlist(Course* c) {
        while (!c->isFull() && !c->waitlist.isEmpty()) {
            string nextId;
            if (!c->waitlist.dequeue(nextId)) break;

            Student* ns = findStudent(nextId);
            if (ns == nullptr) continue;

            string clashWith;
            if (hasClash(ns, c->slot, clashWith)) {
                cout << "   [waitlist] " << ns->name << " skipped for "
                     << c->courseId << " (clash with " << clashWith << ").\n";
                continue;
            }
            c->enrolledStudents.pushBack(nextId);
            c->enrolled++;
            c->totalRegistrations++;
            ns->timetable.pushBack(
                TimetableEntry(c->courseId, c->title, c->instructor, c->slot));
            cout << "   [waitlist] " << ns->name << " auto-enrolled into "
                 << c->courseId << ".\n";
        }
    }

public:
    ~RegistrationSystem() {
        for (auto& pair : students) delete pair.second;
    }

    bool addStudent(const string& id, const string& name, int year) {
        if (findStudent(id) != nullptr) {
            cout << "Student " << id << " already exists.\n";
            return false;
        }
        students[id] = new Student(id, name, year);
        cout << "Added student " << id << " (" << name << ", year " << year << ").\n";
        return true;
    }

    bool addCourse(const string& id, const string& title, const string& inst,
                   int cap, const TimeSlot& slot) {
        if (courses.find(id) != nullptr) {
            cout << "Course " << id << " already exists.\n";
            return false;
        }
        courses.insert(new Course(id, title, inst, cap, slot));
        cout << "Added course " << id << " (" << title << ", capacity " << cap << ").\n";
        return true;
    }

    void registerCourse(const string& sid, const string& cid) {
        Student* st = findStudent(sid);
        if (st == nullptr) { cout << "No such student: " << sid << ".\n"; return; }
        Course* c = courses.find(cid);
        if (c == nullptr) { cout << "No such course: " << cid << ".\n"; return; }

        if (c->enrolledStudents.contains(sid) || c->waitlist.contains(sid)) {
            cout << st->name << " is already registered/waitlisted in " << cid << ".\n";
            return;
        }

        string clashWith;
        if (hasClash(st, c->slot, clashWith)) {
            cout << "Cannot register " << cid << ": time clash with "
                 << clashWith << ".\n";
            return;
        }

        if (!c->isFull()) {
            c->enrolledStudents.pushBack(sid);
            c->enrolled++;
            c->totalRegistrations++;
            st->timetable.pushBack(
                TimetableEntry(c->courseId, c->title, c->instructor, c->slot));
            cout << "Registered " << st->name << " in " << cid
                 << " (" << c->seatsLeft() << " seats left).\n";
        } else {
            c->waitlist.enqueueWithPriority(sid, st->year);
            cout << cid << " is full. " << st->name
                 << " added to waitlist (year-" << st->year << " priority, "
                 << c->waitlist.size() << " waiting).\n";
        }
    }

    void dropCourse(const string& sid, const string& cid) {
        Student* st = findStudent(sid);
        if (st == nullptr) { cout << "No such student: " << sid << ".\n"; return; }
        Course* c = courses.find(cid);
        if (c == nullptr) { cout << "No such course: " << cid << ".\n"; return; }

        if (c->enrolledStudents.remove(sid)) {
            c->enrolled--;
            st->timetable.remove(TimetableEntry(cid, "", "", TimeSlot()));
            cout << st->name << " dropped " << cid << ".\n";
            promoteFromWaitlist(c);
        } else if (c->waitlist.remove(sid)) {
            cout << st->name << " removed from the " << cid << " waitlist.\n";
        } else {
            cout << st->name << " is not in " << cid << ".\n";
        }
    }

    void cancelCourse(const string& cid) {
        Course* c = courses.find(cid);
        if (c == nullptr) { cout << "No such course: " << cid << ".\n"; return; }

        cout << "Cancelling " << cid << " - sending notifications:\n";

        for (LinkedList<string>::Node* n = c->enrolledStudents.getHead();
             n != nullptr; n = n->next) {
            Student* st = findStudent(n->data);
            if (st != nullptr) {
                st->timetable.remove(TimetableEntry(cid, "", "", TimeSlot()));
                cout << "   notify " << st->name
                     << " (enrolled): " << cid << " has been cancelled.\n";
            }
        }
        for (Queue<string>::Node* w = c->waitlist.getFront();
             w != nullptr; w = w->next) {
            Student* st = findStudent(w->data);
            if (st != nullptr)
                cout << "   notify " << st->name
                     << " (waitlist): " << cid << " has been cancelled.\n";
        }

        Course* removed = courses.detach(cid);
        delete removed;
        cout << cid << " removed from the catalog.\n";
    }

    void giveFeedback(const string& sid, const string& cid, int rating) {
        Student* st = findStudent(sid);
        if (st == nullptr) { cout << "No such student: " << sid << ".\n"; return; }
        Course* c = courses.find(cid);
        if (c == nullptr) { cout << "No such course: " << cid << ".\n"; return; }
        if (rating < 1 || rating > 5) { cout << "Rating must be 1-5.\n"; return; }
        if (!c->enrolledStudents.contains(sid)) {
            cout << "Only students enrolled in " << cid << " can rate it.\n";
            return;
        }
        c->ratingSum += rating;
        c->ratingCount++;
        cout << "Thanks! " << cid << " average rating is now "
             << c->averageRating() << ".\n";
    }

    void viewTimetable(const string& sid) const {
        Student* st = findStudent(sid);
        if (st == nullptr) { cout << "No such student: " << sid << ".\n"; return; }
        cout << "\nTimetable for " << st->name
             << " (" << st->id << ", year " << st->year << "):\n";
        if (st->timetable.isEmpty()) { cout << "   (no courses)\n"; return; }
        cout << "   --------------------------------------------------------\n";
        for (LinkedList<TimetableEntry>::Node* n = st->timetable.getHead();
             n != nullptr; n = n->next) {
            const TimetableEntry& e = n->data;
            cout << "   " << e.courseId << " | " << e.title
                 << " | " << e.instructor << " | " << e.slot.day << " "
                 << formatTime(e.slot.start) << "-" << formatTime(e.slot.end) << "\n";
        }
        cout << "   --------------------------------------------------------\n";
    }

    void listAvailableCourses() const {
        LinkedList<Course*> list;
        courses.inOrder(list);
        cout << "\nAvailable courses (with free seats, sorted by ID):\n";
        bool any = false;
        for (LinkedList<Course*>::Node* n = list.getHead(); n != nullptr; n = n->next) {
            Course* c = n->data;
            if (!c->isFull()) {
                any = true;
                cout << "   " << c->courseId << " | " << c->title
                     << " | seats " << c->seatsLeft() << "/" << c->capacity
                     << " | " << c->slot.day << " "
                     << formatTime(c->slot.start) << "-" << formatTime(c->slot.end) << "\n";
            }
        }
        if (!any) cout << "   (none)\n";
    }

    void listAllCourses() const {
        LinkedList<Course*> list;
        courses.inOrder(list);
        cout << "\nAll courses (sorted by ID):\n";
        if (list.isEmpty()) { cout << "   (none)\n"; return; }
        for (LinkedList<Course*>::Node* n = list.getHead(); n != nullptr; n = n->next) {
            Course* c = n->data;
            cout << "   " << c->courseId << " | " << c->title
                 << " | " << c->instructor
                 << " | " << c->enrolled << "/" << c->capacity
                 << " | waitlist " << c->waitlist.size()
                 << " | " << c->slot.day << " "
                 << formatTime(c->slot.start) << "-" << formatTime(c->slot.end)
                 << " | avg " << c->averageRating() << "\n";
        }
    }

    void viewCourseDetail(const string& cid) const {
        Course* c = courses.find(cid);
        if (c == nullptr) { cout << "No such course: " << cid << ".\n"; return; }
        cout << "\nCourse " << c->courseId << " - " << c->title << "\n";
        cout << "   Instructor : " << c->instructor << "\n";
        cout << "   Time       : " << c->slot.day << " "
             << formatTime(c->slot.start) << "-" << formatTime(c->slot.end) << "\n";
        cout << "   Seats      : " << c->enrolled << "/" << c->capacity << "\n";
        cout << "   Rating     : " << c->averageRating()
             << " (" << c->ratingCount << " votes)\n";

        cout << "   Enrolled:\n";
        if (c->enrolledStudents.isEmpty()) cout << "      (none)\n";
        for (LinkedList<string>::Node* n = c->enrolledStudents.getHead();
             n != nullptr; n = n->next) {
            Student* st = findStudent(n->data);
            if (st != nullptr)
                cout << "      - " << st->id << " " << st->name << "\n";
        }
        cout << "   Waitlist (front = next to enroll):\n";
        if (c->waitlist.isEmpty()) cout << "      (empty)\n";
        int pos = 1;
        for (Queue<string>::Node* w = c->waitlist.getFront();
             w != nullptr; w = w->next) {
            Student* st = findStudent(w->data);
            if (st != nullptr)
                cout << "      " << pos++ << ". " << st->id << " "
                     << st->name << " (year " << st->year << ")\n";
        }
    }

    void popularityReport() const {
        LinkedList<Course*> list;
        courses.inOrder(list);
        int n = list.size();
        if (n == 0) { cout << "No courses to rank.\n"; return; }

        Course** arr = new Course*[n];
        int i = 0;
        for (LinkedList<Course*>::Node* nd = list.getHead(); nd != nullptr; nd = nd->next)
            arr[i++] = nd->data;

        for (int a = 0; a < n - 1; a++) {
            int best = a;
            for (int b = a + 1; b < n; b++)
                if (popularityScore(arr[b]) > popularityScore(arr[best]))
                    best = b;
            Course* tmp = arr[a]; arr[a] = arr[best]; arr[best] = tmp;
        }

        cout << "\nCourse popularity ranking:\n";
        cout << "   Rank  Course  Enrolled  Waitlist  AvgRating  Score\n";
        for (int k = 0; k < n; k++) {
            Course* c = arr[k];
            cout << "   " << (k + 1) << "     " << c->courseId
                 << "    " << c->enrolled
                 << "         " << c->waitlist.size()
                 << "         " << c->averageRating()
                 << "       " << popularityScore(c) << "\n";
        }
        delete[] arr;
    }

    void exportCSV(const string& filename) const {
        ofstream out(filename.c_str());
        if (!out) { cout << "Could not open " << filename << " for writing.\n"; return; }

        out << "CourseID,Title,Instructor,Capacity,Enrolled,Waitlist,"
               "Day,Start,End,AvgRating\n";
        LinkedList<Course*> list;
        courses.inOrder(list);
        for (LinkedList<Course*>::Node* n = list.getHead(); n != nullptr; n = n->next) {
            Course* c = n->data;
            out << c->courseId << "," << c->title << "," << c->instructor << ","
                << c->capacity << "," << c->enrolled << "," << c->waitlist.size() << ","
                << c->slot.day << "," << c->slot.start << "," << c->slot.end << ","
                << c->averageRating() << "\n";
        }
        out.close();
        cout << "Exported course data to " << filename << ".\n";
    }

    void exportJSON(const string& filename) const {
        ofstream out(filename.c_str());
        if (!out) { cout << "Could not open " << filename << " for writing.\n"; return; }

        LinkedList<Course*> list;
        courses.inOrder(list);
        out << "[\n";
        for (LinkedList<Course*>::Node* n = list.getHead(); n != nullptr; n = n->next) {
            Course* c = n->data;
            out << "  {\n";
            out << "    \"courseId\": \""   << c->courseId   << "\",\n";
            out << "    \"title\": \""      << c->title      << "\",\n";
            out << "    \"instructor\": \"" << c->instructor << "\",\n";
            out << "    \"capacity\": "     << c->capacity   << ",\n";
            out << "    \"enrolled\": "     << c->enrolled   << ",\n";
            out << "    \"waitlist\": "     << c->waitlist.size() << ",\n";
            out << "    \"avgRating\": "    << c->averageRating() << ",\n";
            out << "    \"day\": \""        << c->slot.day   << "\",\n";
            out << "    \"start\": "        << c->slot.start << ",\n";
            out << "    \"end\": "          << c->slot.end   << "\n";
            out << "  }" << (n->next != nullptr ? "," : "") << "\n";
        }
        out << "]\n";
        out.close();
        cout << "Exported course data to " << filename << ".\n";
    }

    void seedSampleData() {
        addStudent("S001", "Ayesha Khan",  4);
        addStudent("S002", "Bilal Ahmed",  2);
        addStudent("S003", "Sara Ali",     4);
        addStudent("S004", "Usman Tariq",  1);

        addCourse("CS101", "Intro to Programming", "Dr. Hassan", 2, TimeSlot("Mon",  900, 1030));
        addCourse("CS201", "Data Structures",      "Dr. Naveed", 3, TimeSlot("Tue", 1100, 1230));
        addCourse("MA101", "Calculus I",           "Dr. Farah",  2, TimeSlot("Mon",  900, 1030));
        addCourse("EE210", "Digital Logic",        "Dr. Kamran", 2, TimeSlot("Wed", 1330, 1500));

        cout << "Sample data loaded.\n";
    }
};

#endif
