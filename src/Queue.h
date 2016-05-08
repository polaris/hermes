#ifndef _Queue_H
#define _Queue_H

#include <boost/lockfree/queue.hpp>

#include <memory>
#include <functional>
#include <atomic>

template <typename T>
class Queue {
public:
    explicit Queue(uint32_t size)
    : size_(size)
    , queue_(size_)
    , count_(0) {
    }

    Queue(uint32_t size, std::function<T* ()> create)
    : Queue(size) {
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
            count_++;
        }
    }

    T* pop() {
        T* element;
        if (queue_.pop(element)) {
            count_--;
            return element;
        }
        return nullptr;
    }

    uint32_t getSize() const {
        return size_;
    }

    uint32_t getCount() const {
        return count_;
    }

private:
    const uint32_t size_;
    boost::lockfree::queue<T*> queue_;
    std::atomic<uint32_t> count_;
};

template <typename T>
using Pool = Queue<T>;

#endif  // _Queue_H
