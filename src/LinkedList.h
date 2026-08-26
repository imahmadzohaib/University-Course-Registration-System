#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Generic singly linked list. T must support operator==.
template <typename T>
class LinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    int   count;

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        for (Node* cur = other.head; cur != nullptr; cur = cur->next)
            pushBack(cur->data);
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            for (Node* cur = other.head; cur != nullptr; cur = cur->next)
                pushBack(cur->data);
        }
        return *this;
    }

    ~LinkedList() { clear(); }

    void pushBack(const T& value) {
        Node* node = new Node(value);
        if (head == nullptr) head = tail = node;
        else { tail->next = node; tail = node; }
        count++;
    }

    bool contains(const T& value) const {
        for (Node* cur = head; cur != nullptr; cur = cur->next)
            if (cur->data == value) return true;
        return false;
    }

    bool remove(const T& value) {
        Node* cur  = head;
        Node* prev = nullptr;
        while (cur != nullptr) {
            if (cur->data == value) {
                if (prev == nullptr) head = cur->next;
                else                 prev->next = cur->next;
                if (cur == tail) tail = prev;
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
        Node* cur = head;
        while (cur != nullptr) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head = tail = nullptr;
        count = 0;
    }

    bool  isEmpty() const { return count == 0; }
    int   size()    const { return count; }
    Node* getHead() const { return head; }
};

#endif
