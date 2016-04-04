#ifndef _BufferWriter_H
#define _BufferWriter_H

#include "ByteSwap.h"

#include <cstddef>
#include <stdexcept>
#include <memory>

class BufferWriter {
public:
    BufferWriter(std::size_t capacity)
    : head_(0)
    , capacity_(capacity)
    , data_(new char [capacity_]) {
    }

    ~BufferWriter() {
        delete [] data_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    std::size_t size() const {
        return head_;
    }

    void clear() {
        head_ = 0;
    }

    const char* data() const {
        return data_;
    }

    template<typename T>
    void write(T data) {
        const auto t = ByteSwap(data);
        write(&t, sizeof(data));
    }

    template<typename T>
    void write(const std::vector<T> &data) {
        const auto count = data.size();
        write(count);
        for (const T& element : data) {
            write(element);
        }
    }

    void write(const void *data, std::size_t size) {
        if (head_ + size > capacity_) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data_ + head_, data, size);
        head_ += size;
    }

private:
    std::size_t head_;
    std::size_t capacity_;
    char *data_;
};

#endif  // _BufferWriter_H
