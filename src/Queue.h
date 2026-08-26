#ifndef QUEUE_H
#define QUEUE_H

// FIFO queue with optional priority insertion. Used as a course waitlist:
// higher priority (student year) leaves first; equal priorities stay FIFO.
template <typename T>
class Queue {
public:
    struct Node {
        T     data;
        int   priority;
        Node* next;
        Node(const T& d, int p) : data(d), priority(p), next(nullptr) {}
    };

private:
    Node* frontPtr;
    Node* rearPtr;
    int   count;

public:
    Queue() : frontPtr(nullptr), rearPtr(nullptr), count(0) {}

    Queue(const Queue& other) : frontPtr(nullptr), rearPtr(nullptr), count(0) {
        for (Node* cur = other.frontPtr; cur != nullptr; cur = cur->next)
            enqueueWithPriority(cur->data, cur->priority);
    }
    Queue& operator=(const Queue& other) {
        if (this != &other) {
            clear();
            for (Node* cur = other.frontPtr; cur != nullptr; cur = cur->next)
                enqueueWithPriority(cur->data, cur->priority);
        }
        return *this;
    }
    ~Queue() { clear(); }

    void enqueue(const T& value) { enqueueWithPriority(value, 0); }

    void enqueueWithPriority(const T& value, int priority) {
        Node* node = new Node(value, priority);
        if (frontPtr == nullptr) {
            frontPtr = rearPtr = node;
            count++;
            return;
        }
        if (priority > frontPtr->priority) {
            node->next = frontPtr;
            frontPtr = node;
            count++;
            return;
        }
        Node* cur = frontPtr;
        while (cur->next != nullptr && cur->next->priority >= priority)
            cur = cur->next;
        node->next = cur->next;
        cur->next  = node;
        if (node->next == nullptr) rearPtr = node;
        count++;
    }

    bool dequeue(T& out) {
        if (frontPtr == nullptr) return false;
        Node* tmp = frontPtr;
        out = tmp->data;
        frontPtr = frontPtr->next;
        if (frontPtr == nullptr) rearPtr = nullptr;
        delete tmp;
        count--;
        return true;
    }

    bool peek(T& out) const {
        if (frontPtr == nullptr) return false;
        out = frontPtr->data;
        return true;
    }

    bool contains(const T& value) const {
        for (Node* cur = frontPtr; cur != nullptr; cur = cur->next)
            if (cur->data == value) return true;
        return false;
    }

    bool remove(const T& value) {
        Node* cur  = frontPtr;
        Node* prev = nullptr;
        while (cur != nullptr) {
            if (cur->data == value) {
                if (prev == nullptr) frontPtr = cur->next;
                else                 prev->next = cur->next;
                if (cur == rearPtr) rearPtr = prev;
                delete cur;
                count--;
                return true;
            }
            prev = cur;
            cur  = cur->next;
        }
        return false;
    }

    void clear() {
        Node* cur = frontPtr;
        while (cur != nullptr) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        frontPtr = rearPtr = nullptr;
        count = 0;
    }

    bool  isEmpty()  const { return count == 0; }
    int   size()     const { return count; }
    Node* getFront() const { return frontPtr; }
};

#endif
