// ---------------------------------------------------------------------------
// Web entry point for the University Course Registration System.
//
// This is the browser build. It reuses the exact same core logic from
// ../src/RegistrationSystem.h (and the data structures behind it) WITHOUT any
// changes. The ONLY difference from src/main.cpp is the input layer: a browser
// cannot block on std::cin, so input is read asynchronously, one line at a
// time, from an xterm.js terminal via Emscripten's Asyncify.
//
// Native builds are unaffected — this file is compiled only for WebAssembly.
//
// Build:  see web/build.sh   (requires the Emscripten SDK / em++)
// ---------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <cstdio>     // setvbuf
#include <cstdlib>    // free
#include <emscripten.h>
#include "../src/RegistrationSystem.h"
using namespace std;

// Write text to the terminal immediately. Used for prompts that have no
// trailing newline (line-buffered stdout would otherwise hide them until the
// next '\n'). Module.termWrite is defined in index.html.
EM_JS(void, term_write, (const char* s), {
    Module.termWrite(UTF8ToString(s));
});

// Await a single line of input from the browser terminal.
// Module.readLine() (see index.html) returns a Promise<string> that resolves
// when the user presses Enter. Asyncify suspends the C++ call until then.
EM_ASYNC_JS(char*, read_line_async, (), {
    const line = await Module.readLine();
    return stringToNewUTF8(line);   // caller frees
});

static string readStr(const string& prompt) {
    term_write(prompt.c_str());
    char* p = read_line_async();
    string s(p ? p : "");
    if (p) free(p);
    return s;
}

static int readInt(const string& prompt) {
    term_write(prompt.c_str());
    for (;;) {
        char* p = read_line_async();
        string line(p ? p : "");
        if (p) free(p);
        try {
            return stoi(line);
        } catch (...) {
            term_write("   Please enter a number: ");
        }
    }
}

static void printMenu() {
    cout << "\n==================== MAIN MENU ====================\n";
    cout << "  1.  Add student\n";
    cout << "  2.  Add course\n";
    cout << "  3.  Register for a course\n";
    cout << "  4.  Drop a course\n";
    cout << "  5.  Cancel a course (notify everyone)\n";
    cout << "  6.  View a student's timetable\n";
    cout << "  7.  List available courses (free seats)\n";
    cout << "  8.  List all courses\n";
    cout << "  9.  View course details (enrolled + waitlist)\n";
    cout << " 10.  Give course feedback (rating 1-5)\n";
    cout << " 11.  Popularity report\n";
    cout << " 12.  Export courses to CSV\n";
    cout << " 13.  Export courses to JSON\n";
    cout << " 14.  Load sample data\n";
    cout << "  0.  Exit\n";
    cout << "==================================================\n";
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);   // don't hold output back in the browser

    RegistrationSystem system;

    cout << "University Course Registration System\n";
    cout << "(Hash Map + BST + Queue + Linked List)\n";
    cout << "Tip: choose 14 to load sample data, then explore.\n";

    bool running = true;
    while (running) {
        printMenu();
        int choice = readInt("Select an option: ");

        switch (choice) {
            case 1: {
                string id   = readStr("   Student ID  : ");
                string name = readStr("   Student name: ");
                int    year = readInt("   Year (1-4)  : ");
                system.addStudent(id, name, year);
                break;
            }
            case 2: {
                string id    = readStr("   Course ID   : ");
                string title = readStr("   Title       : ");
                string inst  = readStr("   Instructor  : ");
                int    cap   = readInt("   Capacity    : ");
                string day   = readStr("   Day (Mon..) : ");
                int    start = readInt("   Start (e.g. 900) : ");
                int    end   = readInt("   End   (e.g. 1030): ");
                system.addCourse(id, title, inst, cap, TimeSlot(day, start, end));
                break;
            }
            case 3: {
                string sid = readStr("   Student ID : ");
                string cid = readStr("   Course ID  : ");
                system.registerCourse(sid, cid);
                break;
            }
            case 4: {
                string sid = readStr("   Student ID : ");
                string cid = readStr("   Course ID  : ");
                system.dropCourse(sid, cid);
                break;
            }
            case 5: {
                string cid = readStr("   Course ID to cancel: ");
                system.cancelCourse(cid);
                break;
            }
            case 6: {
                string sid = readStr("   Student ID : ");
                system.viewTimetable(sid);
                break;
            }
            case 7:
                system.listAvailableCourses();
                break;
            case 8:
                system.listAllCourses();
                break;
            case 9: {
                string cid = readStr("   Course ID : ");
                system.viewCourseDetail(cid);
                break;
            }
            case 10: {
                string sid    = readStr("   Student ID : ");
                string cid    = readStr("   Course ID  : ");
                int    rating = readInt("   Rating 1-5 : ");
                system.giveFeedback(sid, cid, rating);
                break;
            }
            case 11:
                system.popularityReport();
                break;
            case 12: {
                string f = readStr("   CSV filename (e.g. courses.csv): ");
                system.exportCSV(f);
                break;
            }
            case 13: {
                string f = readStr("   JSON filename (e.g. courses.json): ");
                system.exportJSON(f);
                break;
            }
            case 14:
                system.seedSampleData();
                break;
            case 0:
                running = false;
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid option, try again.\n";
        }
    }
    return 0;
}
