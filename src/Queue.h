#ifndef _Queue_H
#define _Queue_H

#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>

#include <memory>

template <typename T>
class Queue {
public:
    Queue(unsigned int size)
    : Queue(size, 0) {
    }

    Queue(unsigned int size, unsigned int items)
    : queue(size) {
        for (std::size_t i = 0; i < items; ++i) {
            queue.push(new T{});
        }
    }

    Queue(const Queue&) =delete;
    Queue& operator =(const Queue&) =delete;

    virtual ~Queue() {
        T* element;
        while (queue.pop(element)) {
            delete element;
        }
    }

    void push(T* element) {
        if (element != nullptr) {
            queue.push(element);
        }
    }

    T* pop() {
        T* element;
        if (queue.pop(element)) {
            return element;
        }
        return nullptr;
    }

private:
    boost::lockfree::queue<T*> queue;
};

template <typename T>
using Pool = Queue<T>;

#endif  // _Queue_H
