#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>
#include <functional>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;

        // Constructor for sentinel node (no data)
        node() : data(nullptr), prev(nullptr), next(nullptr) {}

        // Constructor with data
        node(const T &value) : data(new T(value)), prev(nullptr), next(nullptr) {}

        // Destructor
        ~node() {
            if (data != nullptr) {
                delete data;
            }
        }
    };

protected:
    node *head;  // sentinel node
    node *tail;  // sentinel node
    size_t len;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }

    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
        friend class list<T>;
        friend class const_iterator;
    private:
        node *ptr;
        const list<T> *container;

    public:
        iterator() : ptr(nullptr), container(nullptr) {}
        iterator(node *p, const list<T> *c) : ptr(p), container(c) {}

        /**
         * iter++
         */
        iterator operator++(int) {
            if (ptr == nullptr || (container != nullptr && ptr == container->tail)) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        /**
         * ++iter
         */
        iterator & operator++() {
            if (ptr == nullptr || (container != nullptr && ptr == container->tail)) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        /**
         * iter--
         */
        iterator operator--(int) {
            if (ptr == nullptr || (container != nullptr && ptr->prev == container->head)) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        /**
         * --iter
         */
        iterator & operator--() {
            if (ptr == nullptr || (container != nullptr && ptr->prev == container->head)) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };

    /**
     * TODO
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
        friend class list<T>;
        friend class iterator;
    private:
        node *ptr;
        const list<T> *container;

    public:
        const_iterator() : ptr(nullptr), container(nullptr) {}
        const_iterator(node *p, const list<T> *c) : ptr(p), container(c) {}
        const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

        const_iterator operator++(int) {
            if (ptr == nullptr || (container != nullptr && ptr == container->tail)) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        const_iterator & operator++() {
            if (ptr == nullptr || (container != nullptr && ptr == container->tail)) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        const_iterator operator--(int) {
            if (ptr == nullptr || (container != nullptr && ptr->prev == container->head)) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        const_iterator & operator--() {
            if (ptr == nullptr || (container != nullptr && ptr->prev == container->head)) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        const T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        const T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };

    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() : len(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
    }

    list(const list &other) : len(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;

        for (node *p = other.head->next; p != other.tail; p = p->next) {
            push_back(*(p->data));
        }
    }

    /**
     * TODO Destructor
     */
    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }

    /**
     * TODO Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;

        clear();
        for (node *p = other.head->next; p != other.tail; p = p->next) {
            push_back(*(p->data));
        }
        return *this;
    }

    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (empty()) throw container_is_empty();
        return *(head->next->data);
    }

    const T & back() const {
        if (empty()) throw container_is_empty();
        return *(tail->prev->data);
    }

    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(head->next, this);
    }

    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }

    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(tail, this);
    }

    const_iterator cend() const {
        return const_iterator(tail, this);
    }

    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const {
        return len == 0;
    }

    /**
     * returns the number of elements
     */
    virtual size_t size() const {
        return len;
    }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *p = head->next;
        while (p != tail) {
            node *tmp = p;
            p = p->next;
            delete tmp;
        }
        head->next = tail;
        tail->prev = head;
        len = 0;
    }

    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) throw invalid_iterator();

        node *new_node = new node(value);
        insert(pos.ptr, new_node);
        len++;
        return iterator(new_node, this);
    }

    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (empty()) throw container_is_empty();
        if (pos.container != this || pos.ptr == tail || pos.ptr->data == nullptr) {
            throw invalid_iterator();
        }

        node *next_node = pos.ptr->next;
        erase(pos.ptr);
        delete pos.ptr;
        len--;
        return iterator(next_node, this);
    }

    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        node *new_node = new node(value);
        insert(tail, new_node);
        len++;
    }

    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (empty()) throw container_is_empty();
        node *target = tail->prev;
        erase(target);
        delete target;
        len--;
    }

    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        node *new_node = new node(value);
        insert(head->next, new_node);
        len++;
    }

    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (empty()) throw container_is_empty();
        node *target = head->next;
        erase(target);
        delete target;
        len--;
    }

    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (len <= 1) return;

        // Allocate raw memory for array
        char *buffer = new char[len * sizeof(T)];
        T *arr = reinterpret_cast<T*>(buffer);

        // Use placement new to copy construct elements (using copy constructor, not default)
        node *p = head->next;
        for (size_t i = 0; i < len; i++) {
            new (&arr[i]) T(*(p->data));
            p = p->next;
        }

        // Sort using the provided sort function
        std::function<bool(const T&, const T&)> cmp = [](const T &a, const T &b) { return a < b; };
        sjtu::sort(arr, arr + len, cmp);

        // Copy back sorted data using assignment operator
        p = head->next;
        for (size_t i = 0; i < len; i++) {
            *(p->data) = arr[i];
            p = p->next;
        }

        // Manually destroy elements
        for (size_t i = 0; i < len; i++) {
            arr[i].~T();
        }

        delete[] buffer;
    }

    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        if (this == &other) return;

        node *p1 = head->next;
        node *p2 = other.head->next;

        while (p2 != other.tail) {
            if (p1 == tail || !(*(p1->data) < *(p2->data))) {
                // Insert p2 before p1
                node *next_p2 = p2->next;
                other.erase(p2);
                insert(p1, p2);
                p2 = next_p2;
            } else {
                p1 = p1->next;
            }
        }

        len += other.len;
        other.len = 0;
    }

    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (len <= 1) return;

        node *p = head;
        while (p != nullptr) {
            node *tmp = p->next;
            p->next = p->prev;
            p->prev = tmp;
            p = tmp;
        }

        node *tmp = head;
        head = tail;
        tail = tmp;
    }

    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (len <= 1) return;

        node *p = head->next;
        while (p != tail && p->next != tail) {
            if (*(p->data) == *(p->next->data)) {
                node *dup = p->next;
                erase(dup);
                delete dup;
                len--;
            } else {
                p = p->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
