#ifndef _Queue_H
#define _Queue_H

#include <boost/lockfree/queue.hpp>

#include <memory>
#include <functional>

template <typename T>
class Queue {
public:
    explicit Queue(uint32_t size)
    : size_(size)
    , queue_(size_) {
    }

    Queue(uint32_t size, std::function<T* ()> create)
    : size_(size)
    , queue_(size_) {
        for (uint32_t i = 0; i < size_; ++i) {
            push(create());
        }
    }

    Queue(const Queue&) =delete;
    Queue& operator =(const Queue&) =delete;

    virtual ~Queue() {
        T* element;
        while (queue_.pop(element)) {
            delete element;
        }
    }

    void push(T* element) {
        if (element != nullptr) {
            queue_.push(element);
        }
    }

    T* pop() {
        T* element;
        if (queue_.pop(element)) {
            return element;
        }
        return nullptr;
    }

    uint32_t getSize() const {
        return size_;
    }

private:
    const uint32_t size_;
    boost::lockfree::queue<T*> queue_;
};

template <typename T>
using Pool = Queue<T>;

#endif  // _Queue_H
