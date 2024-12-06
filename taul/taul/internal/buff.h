

#pragma once


#include <cstdint>
#include <string>
#include <vector>

#include "../asserts.h"


namespace taul::internal {


    using buff_t = std::vector<uint8_t>;


    inline buff_t buff_concat(const buff_t& lhs, const buff_t& rhs) {
        buff_t result{};
        result.resize(lhs.size() + rhs.size());
        if (!result.empty()) {
            std::memcpy((void*)result.data(), (void*)lhs.data(), lhs.size());
            std::memcpy((void*)(result.data() + lhs.size()), (void*)rhs.data(), rhs.size());
        }
        return result;
    }


    template<typename T>
        requires std::is_trivially_copyable_v<T>
    inline void buff_write(buff_t& b, const T& x) {
        // alloc and copy over x
        b.resize(b.size() + sizeof(T));
        std::memcpy((void*)&b[b.size() - sizeof(T)], (const void*)&x, sizeof(T));
    }

    inline void buff_write_s(buff_t& b, std::string_view x) {
        TAUL_IN_BOUNDS(x.length(), 0, std::numeric_limits<uint32_t>::max());
        buff_write<uint32_t>(b, (uint32_t)x.length()); // write length
        // alloc and copy over x
        b.resize(b.size() + x.length());
        std::memcpy((void*)&b[b.size() - x.length()], (const void*)x.data(), x.length());
    }
    template<typename StringLike>
    inline void buff_write_s(buff_t& b, const StringLike& x) {
        buff_write_s(b, std::string_view(x));
    }


    // the len out param will be incremented by sizeof(T) upon read, w/ this being
    // used to simplify writing stuff that reads buffers

    template<typename T>
        requires std::is_default_constructible_v<T> && std::is_trivially_copyable_v<T>
    inline T buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
        // assert [offset, offset + sizeof(T)) is in-bounds
        TAUL_IN_BOUNDS(offset, 0, b.size());
        if constexpr (sizeof(T) > 0) TAUL_IN_BOUNDS(offset + sizeof(T) - 1, 0, b.size());
        len += sizeof(T); // incr len
        // copy data to result
        T result{};
        std::memcpy((void*)&result, (const void*)(b.data() + offset), sizeof(T));
        return result;
    }

    inline std::string_view buff_read_s(const buff_t& b, size_t offset, size_t& len) noexcept {
        // read length of string
        const auto l = buff_read<uint32_t>(b, offset, len);
        // assert [offset + sizeof(uint32_t), offset + sizeof(uint32_t) + l) is in-bounds
        TAUL_IN_BOUNDS(offset + sizeof(uint32_t), 0, b.size());
        if (l > 0) TAUL_IN_BOUNDS(offset + sizeof(uint32_t) + l - 1, 0, b.size());
        len += l; // incr len
        return std::string_view((const char*)b.data() + offset + sizeof(uint32_t), l);
    }


    template<typename T>
        requires std::is_default_constructible_v<T> && std::is_trivially_copyable_v<T>
    inline T buff_peek(const buff_t& b, size_t offset) noexcept {
        size_t len = 0;
        return buff_read<T>(b, offset, len);
    }

    inline std::string_view buff_peek_s(const buff_t& b, size_t offset) noexcept {
        size_t len = 0;
        return buff_read_s(b, offset, len);
    }
}

