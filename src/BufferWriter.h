#ifndef _BufferWriter_H
#define _BufferWriter_H

#include "ByteSwap.h"
#include "Buffer.h"

class BufferWriter {
public:
    explicit BufferWriter(Buffer &buffer)
    : head_(0)
    , buffer_(buffer) {
    }

    std::size_t capacity() const {
        return buffer_.capacity();
    }

    std::size_t position() const {
        return head_;
    }

    void clear() {
        buffer_.clear();
        head_ = 0;
    }

    template<typename T>
    void write(T data) {
        const auto size = sizeof(T);
        const auto t = ByteSwap(data);
        buffer_.write(&t, head_, size);
        head_ += size;
    }

    template<typename T>
    void write(const std::vector<T> &data) {
        const auto count = data.size();
        write(count);
        for (const T& element : data) {
            write(element);
        }
    }

private:
    std::size_t head_;
    Buffer &buffer_;
};

#endif  // _BufferWriter_H
