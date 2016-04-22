#ifndef _Packet_H
#define _Packet_H

#include "ByteSwap.h"

#include <boost/asio.hpp>

#include <cstring>
#include <stdexcept>
#include <vector>

class Packet {
public:
    explicit Packet(std::size_t capacity)
    : capacity_(capacity)
    , size_(0)
    , data_(new char [capacity_]) {
        memset(data_, 0, capacity_);
    }

    Packet(const Packet&) = delete;

    ~Packet() {
        delete [] data_;
    }

    void reset() {
        size_ = 0;
    }

    void clear() {
        memset(data_, 0, capacity_);
    }

    std::size_t capacity() const {
        return capacity_;
    }

    void size(std::size_t size) {
        if (size > capacity_) {
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

    void setEndpoint(const char* address, unsigned short port) {
        using namespace boost::asio::ip;
        endpoint_ = udp::endpoint(address::from_string(address), port);
    }

    void setEndpoint(const boost::asio::ip::udp::endpoint& endpoint) {
        endpoint_ = endpoint;
    }

    boost::asio::ip::udp::endpoint& getEndpoint() {
        return endpoint_;
    }

    const boost::asio::ip::udp::endpoint& getEndpoint() const {
        return endpoint_;
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
        if (size_ + size > capacity_) {
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
        if (size_ + size > capacity_) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(data, data_ + size_, size);
        size_ += size;
    }

private:
    const std::size_t capacity_;
    std::size_t size_;
    char* data_;

    boost::asio::ip::udp::endpoint endpoint_;
};

#endif  // _Packet_H
