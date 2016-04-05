#ifndef _BufferReader_H
#define _BufferReader_H

#include "ByteSwap.h"
#include "Buffer.h"

class BufferReader {
public:
    explicit BufferReader(Buffer &buffer)
    : head_(0)
    , buffer_(buffer) {
    }

    std::size_t capacity() const {
        return buffer_.capacity();
    }

    std::size_t position() const {
        return head_;
    }

    void reset() {
        head_ = 0;
    }

    template<typename T>
    void read(T &data) {
        const auto size = sizeof(T);
        T t{};
        buffer_.read(&t, head_, size);
        head_ += size;
        data = ByteSwap(t);
    }

    template<typename T>
    void read(std::vector<T> &data) {
        std::size_t count{0};
        read(count);
        data.reserve(count);
        T t{};
        for (std::size_t i = 0; i < count; ++i) {
            read(t);
            data.push_back(t);
        }
    }

private:
    std::size_t head_;
    Buffer &buffer_;
};

#endif  // _BufferReader_H
