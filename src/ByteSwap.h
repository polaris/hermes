#ifndef _ByteSwap_H
#define _ByteSwap_H

#include <inttypes.h>
#include <vector>
#include <cstddef>

/** Swaps the bytes in a word.

  \param data the word that should be swapped.
 */
inline uint16_t ByteSwap2(uint16_t data) {
    return static_cast<uint16_t>(((data >> 8) & 0x00ff) | ((data << 8) & 0xff00));
}

/** Swaps the bytes in a double word.

    \param data the double word to be swapped.
 */
inline uint32_t ByteSwap4(uint32_t data) {
    return ((data >> 24) & 0x000000ff) |
           ((data >>  8) & 0x0000ff00) |
           ((data <<  8) & 0x00ff0000) |
           ((data << 24) & 0xff000000);
}

/** Swaps the bytes in a quadruple word.

    \param data the quadruple word to be swapped.
 */
inline uint64_t ByteSwap8(uint64_t data) {
    return ((data >> 56 ) & 0x00000000000000ff) |
           ((data >> 40 ) & 0x000000000000ff00) |
           ((data >> 24 ) & 0x0000000000ff0000) |
           ((data >>  8 ) & 0x00000000ff000000) |
           ((data <<  8 ) & 0x000000ff00000000) |
           ((data << 24 ) & 0x0000ff0000000000) |
           ((data << 40 ) & 0x00ff000000000000) |
           ((data << 56 ) & 0xff00000000000000);
}

/** A type aliaser to help with swapping.
 */
template <typename F, typename T>
class TypeAliaser {
public:
    explicit TypeAliaser(F from)
    : from_(from) {
    }

    T& get() {
        return to_;
    }
    
    union {
        F from_;
        T to_;
    };
};

/** Template class to help with sapping.
 */
template <typename T, std::size_t SIZE>
class ByteSwapper;

/** Template class to help with sapping.
 */
template <typename T>
class ByteSwapper<T, 1> {
public:
    T Swap(T data) const {
        return data;
    }
};

/** Template class to help with sapping.
 */
template <typename T>
class ByteSwapper<T, 2> {
public:
    T Swap(T data) const {
        uint16_t result = ByteSwap2(TypeAliaser<T, uint16_t>(data).get());
        return TypeAliaser<uint16_t, T>(result).get();
    }
};

/** Template class to help with sapping.
 */
template <typename T>
class ByteSwapper<T, 4> {
public:
    T Swap(T data) const {
        uint32_t result = ByteSwap4(TypeAliaser<T, uint32_t>(data).get());
        return TypeAliaser<uint32_t, T>(result).get();
    }
};

/** Template class to help with sapping.
 */
template <typename T>
class ByteSwapper<T, 8> {
public:
    T Swap(T data) const {
        uint64_t result = ByteSwap8(TypeAliaser<T, uint64_t>(data).get());
        return TypeAliaser<uint64_t, T>(result).get();
    }
};

/** Swaps the bytes in data.

    \param data the data to be swapped.
    \return the swapped data.
 */
template <typename T>
T ByteSwap(T data) {
    return ByteSwapper<T, sizeof(T)>().Swap(data);
}

#endif  // _ByteSwap_H
