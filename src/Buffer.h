#ifndef _Buffer_H
#define _Buffer_H

#include <cstring>
#include <stdexcept>

class Buffer {
public:
    explicit Buffer(std::size_t capacity)
    : capacity_(capacity)
    , data_(new char [capacity_]) {
    }

    ~Buffer() {
        delete [] data_;
    }

    void clear() {
        memset(data_, 0, capacity_);
    }

    std::size_t capacity() const {
        return capacity_;
    }

    const char* data() const {
        return data_;
    }

    void write(const void *data, std::size_t position, std::size_t size) {
        if (position + size > capacity_) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data_ + position, data, size);
    }

    void read(void *data, std::size_t position, std::size_t size) {
        if (position + size > capacity_) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data, data_ + position, size);
    }

private:
    std::size_t capacity_;
    char *data_;
};

#endif  // _Buffer_H
