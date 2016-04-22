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
    , head_(0)
    , data_(new char [capacity_]) {
        memset(data_, 0, capacity_);
    }

    Packet(const Packet&) = delete;

    ~Packet() {
        delete [] data_;
    }

    void reset() {
        head_ = 0;
    }

    void clear() {
        size_ = 0;
    }

    std::size_t getCapacity() const {
        return capacity_;
    }

    void setSize(std::size_t size) {
        if (size > capacity_) {
            throw std::out_of_range("size is out of range");
        }
        size_ = size;
    }

    std::size_t getSize() const {
        return size_;
    }

    std::size_t getHead() const {
        return head_;
    }

    char* getData() {
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

    void read(void *out, std::size_t size) {
        if (head_ + size > size_) {
            throw std::out_of_range("size is out of range");
        }
        memcpy(out, data_ + head_, size);
        head_ += size;
    }

private:
    const std::size_t capacity_;
    std::size_t size_;
    std::size_t head_;
    char* data_;

    boost::asio::ip::udp::endpoint endpoint_;
};

#endif  // _Packet_H
