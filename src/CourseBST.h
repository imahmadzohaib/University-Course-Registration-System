#ifndef COURSEBST_H
#define COURSEBST_H

#include <string>
#include "Course.h"
#include "LinkedList.h"
using namespace std;

// Binary search tree of courses, ordered by courseId. Owns the Course objects.
class CourseBST {
private:
    struct Node {
        Course* course;
        Node*   left;
        Node*   right;
        Node(Course* c) : course(c), left(nullptr), right(nullptr) {}
    };

    Node* root;

    Node* insertNode(Node* node, Course* c) {
        if (node == nullptr) return new Node(c);
        if (c->courseId < node->course->courseId)
            node->left = insertNode(node->left, c);
        else if (c->courseId > node->course->courseId)
            node->right = insertNode(node->right, c);
        return node;
    }

    Node* findMin(Node* node) {
        while (node->left != nullptr) node = node->left;
        return node;
    }

    Node* removeNode(Node* node, const string& id, Course*& removed) {
        if (node == nullptr) return nullptr;
        if (id < node->course->courseId)
            node->left = removeNode(node->left, id, removed);
        else if (id > node->course->courseId)
            node->right = removeNode(node->right, id, removed);
        else {
            removed = node->course;
            if (node->left == nullptr) {
                Node* r = node->right;
                delete node;
                return r;
            }
            if (node->right == nullptr) {
                Node* l = node->left;
                delete node;
                return l;
            }
            // two children: replace with in-order successor
            Node* succ = findMin(node->right);
            node->course = succ->course;
            Course* dummy = nullptr;
            node->right = removeNode(node->right, succ->course->courseId, dummy);
        }
        return node;
    }

    void collect(Node* node, LinkedList<Course*>& out) const {
        if (node == nullptr) return;
        collect(node->left, out);
        out.pushBack(node->course);
        collect(node->right, out);
    }

    void destroy(Node* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node->course;
        delete node;
    }

public:
    CourseBST() : root(nullptr) {}
    ~CourseBST() { destroy(root); }

    CourseBST(const CourseBST&)            = delete;
    CourseBST& operator=(const CourseBST&) = delete;

    void insert(Course* c) { root = insertNode(root, c); }

    Course* find(const string& id) const {
        Node* cur = root;
        while (cur != nullptr) {
            if (id == cur->course->courseId) return cur->course;
            cur = (id < cur->course->courseId) ? cur->left : cur->right;
        }
        return nullptr;
    }

    // Unlinks a course and returns it; the caller then owns/frees it.
    Course* detach(const string& id) {
        Course* removed = nullptr;
        root = removeNode(root, id, removed);
        return removed;
    }

    void inOrder(LinkedList<Course*>& out) const { collect(root, out); }

    bool isEmpty() const { return root == nullptr; }
};

#endif
