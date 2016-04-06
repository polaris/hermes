#ifndef _Buffer_H
#define _Buffer_H

#include "ByteSwap.h"

#include <cstring>
#include <stdexcept>
#include <vector>

template<std::size_t N>
class Buffer {
public:
    Buffer()
    : head_(0)
    , data_(new char [N]) {
    }

    ~Buffer() {
        delete [] data_;
    }

    void reset() {
        head_ = 0;
    }

    void clear() {
        memset(data_, 0, N);
    }

    std::size_t capacity() const {
        return N;
    }

    std::size_t position() const {
        return head_;
    }

    const char* data() const {
        return data_;
    }

    template<typename T>
    void write(T data) {
        const auto size = sizeof(T);
        const auto t = ByteSwap(data);
        write(&t, size);
    }

    template<typename T>
    void write(const std::vector<T> &data) {
        const auto count = data.size();
        write(count);
        for (const T& element : data) {
            write(element);
        }
    }

    template<typename T>
    void read(T &data) {
        const auto size = sizeof(T);
        T t{};
        read(&t, size);
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

    void write(const void *data, std::size_t size) {
        if (head_ + size > N) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data_ + head_, data, size);
        head_ += size;
    }

    void read(void *data, std::size_t size) {
        if (head_ + size > N) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data, data_ + head_, size);
        head_ += size;
    }

private:
    std::size_t head_;
    char *data_;
};

#endif  // _Buffer_H
