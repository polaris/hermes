#ifndef _Packet_H
#define _Packet_H

#include "ByteSwap.h"

#include <boost/asio.hpp>
#include <boost/format.hpp>

#include <cstring>
#include <stdexcept>
#include <vector>

class Packet {
public:
    explicit Packet(uint32_t capacity)
    : capacity_(capacity)
    , size_(0)
    , head_(0)
    , data_(new char [capacity_])
    , endpoint_() {
    }

    Packet(const Packet&) = delete;
    Packet& operator= (const Packet&) = delete;

    ~Packet() {
        delete [] data_;
    }

    void copyDataFrom(const Packet& packet) {
        if (capacity_ >= packet.size_) {
            head_ = 0;
            size_ = packet.size_;
            memcpy(data_, packet.data_, packet.size_);
        } else {
            throw std::out_of_range("capacity is smaller than size");
        }
    }

    void reset() {
        head_ = 0;
    }

    void clear() {
        using namespace boost::asio::ip;
        reset();
        size_ = 0;
        endpoint_ = udp::endpoint(address::from_string("0.0.0.0"), 0);
    }

    uint32_t getCapacity() const {
        return capacity_;
    }

    void setSize(uint32_t size) {
        if (size > capacity_) {
            throw std::out_of_range(boost::str(boost::format("%1%:%2%: size is out of range") % __FILE__ % __LINE__));
        }
        size_ = size;
    }

    uint32_t getSize() const {
        return size_;
    }

    uint32_t getHead() const {
        return head_;
    }

    char* getData() {
        return data_;
    }

    const char* getData() const {
        return data_;
    }

    void setEndpoint(const char* address, uint16_t port) {
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

    void write(const std::string& s) {
        const std::vector<char> v(s.begin(), s.end());
        write(v);
    }

    void write(const void *data, uint32_t size) {
        if (size_ + size > capacity_) {
            throw std::out_of_range(boost::str(boost::format("%1%:%2%: size is out of range") % __FILE__ % __LINE__));
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
        typename std::vector<T>::size_type count{0};
        read(count);
        data.reserve(count);
        T t{};
        for (uint32_t i = 0; i < count; ++i) {
            read(t);
            data.push_back(t);
        }
    }

    void read(std::string& s) {
        std::vector<char> v;
        read(v);
        s = std::string(v.begin(), v.end());
    }

    void read(void *out, uint32_t size) {
        if (head_ + size > size_) {
            throw std::out_of_range(boost::str(boost::format("%1%:%2%: size is out of range") % __FILE__ % __LINE__));
        }
        memcpy(out, data_ + head_, size);
        head_ += size;
    }

private:
    const uint32_t capacity_;
    uint32_t size_;
    uint32_t head_;
    char* data_;

    boost::asio::ip::udp::endpoint endpoint_;
};

#endif  // _Packet_H
