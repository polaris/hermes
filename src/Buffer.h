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
    : size_(0)
    , data_(new char [N]) {
    }

    ~Buffer() {
        delete [] data_;
    }

    void reset() {
        size_ = 0;
    }

    void clear() {
        memset(data_, 0, N);
    }

    std::size_t capacity() const {
        return N;
    }

    void size(std::size_t size) {
        if (size > N) {
            throw std::out_of_range("size is out of range");
        }
        size_ = size;
    }

    std::size_t size() const {
        return size_;
    }

    char* data() {
        return data_;
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

    void write(const void *data, std::size_t size) {
        if (size_ + size > N) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data_ + size_, data, size);
        size_ += size;
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

    void read(void *data, std::size_t size) {
        if (size_ + size > N) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data, data_ + size_, size);
        size_ += size;
    }

private:
    std::size_t size_;
    char *data_;
};

#endif  // _Buffer_H
