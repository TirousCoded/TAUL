

#pragma once


#include <cstdint>
#include <string>
#include <vector>

#include "../asserts.h"

#include "util.h"


namespace taul::internal {


    using buff_t = std::vector<uint8_t>;

    template<typename T>
    concept buff_simple_elem =
        std::is_default_constructible_v<std::remove_reference_t<T>> &&
        std::is_trivially_copyable_v<std::remove_reference_t<T>> &&
        !enum_type<T> &&
        !std::same_as<std::remove_reference_t<T>, std::string_view>;

    template<typename T>
    concept buff_elem =
        buff_simple_elem<T> ||
        enum_type<T> ||
        std::same_as<std::remove_reference_t<T>, std::string_view>;


    inline buff_t buff_concat(const buff_t& lhs, const buff_t& rhs) {
        buff_t result{};
        result.resize(lhs.size() + rhs.size());
        if (!result.empty()) {
            std::memcpy((void*)result.data(), (void*)lhs.data(), lhs.size());
            std::memcpy((void*)(result.data() + lhs.size()), (void*)rhs.data(), rhs.size());
        }
        return result;
    }


    template<buff_simple_elem T>
    inline void buff_write_one(buff_t& b, T x) {
        // alloc and copy over x
        b.resize(b.size() + sizeof(T));
        std::memcpy((void*)&b[b.size() - sizeof(T)], (const void*)&x, sizeof(T));
    }
    
    template<enum_type T>
    inline void buff_write_one(buff_t& b, T x) {
        buff_write_one(b, to_underlying(x));
    }

    template<std::same_as<std::string_view> T>
    inline void buff_write_one(buff_t& b, T x) {
        TAUL_IN_BOUNDS(x.length(), 0, std::numeric_limits<uint32_t>::max());
        buff_write_one(b, (uint32_t)x.length()); // write length
        // alloc and copy over x
        b.resize(b.size() + x.length());
        std::memcpy((void*)&b[b.size() - x.length()], (const void*)x.data(), x.length());
    }

    inline void buff_write(buff_t& b) {
        // do nothing
    }

    template<buff_elem Arg, buff_elem... Args>
    inline void buff_write(buff_t& b, Arg&& arg, Args&&... args) {
        buff_write_one(b, std::remove_cvref_t<Arg>(std::forward<Arg>(arg)));
        buff_write(b, std::forward<Args>(args)...);
    }


    // the len out param will be incremented by sizeof(T) upon read, w/ this being
    // used to simplify writing stuff that reads buffers

    template<buff_simple_elem T>
    inline T buff_read_one(const buff_t& b, size_t offset, size_t& len) noexcept {
        // assert [offset, offset + sizeof(T)) is in-bounds
        TAUL_IN_BOUNDS(offset, 0, b.size());
        if constexpr (sizeof(T) > 0) TAUL_IN_BOUNDS(offset + sizeof(T) - 1, 0, b.size());
        len += sizeof(T); // incr len
        // copy data to result
        T result{};
        std::memcpy((void*)&result, (const void*)(b.data() + offset), sizeof(T));
        return result;
    }

    template<enum_type T>
    inline T buff_read_one(const buff_t& b, size_t offset, size_t& len) noexcept {
        return T(buff_read_one<std::underlying_type_t<T>>(b, offset, len));
    }

    template<std::same_as<std::string_view>>
    inline std::string_view buff_read_one(const buff_t& b, size_t offset, size_t& len) noexcept {
        // read length of string
        const auto l = buff_read_one<uint32_t>(b, offset, len);
        // assert [offset + sizeof(uint32_t), offset + sizeof(uint32_t) + l) is in-bounds
        TAUL_IN_BOUNDS(offset + sizeof(uint32_t), 0, b.size());
        if (l > 0) TAUL_IN_BOUNDS(offset + sizeof(uint32_t) + l - 1, 0, b.size());
        len += l; // incr len
        return std::string_view((const char*)b.data() + offset + sizeof(uint32_t), l);
    }

    inline std::tuple<> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
        return std::make_tuple();
    }

    template<buff_elem T>
    inline std::tuple<T> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
            return std::make_tuple(buff_read_one<T>(b, offset, len));
    }

    // for exactly 2 template params, we can quality-of-life return a std::pair

    template<buff_elem T, buff_elem U>
    inline std::pair<T, U> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
        size_t our_len = 0;
        auto aa = buff_read_one<T>(b, offset, our_len);
        auto bb = buff_read_one<U>(b, offset + our_len, our_len);
        len += our_len;
        return std::make_pair(aa, bb);
    }

    template<buff_elem Arg1, buff_elem Arg2, buff_elem Arg3, buff_elem... Args>
    inline std::tuple<Arg1, Arg2, Arg3, Args...> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
        size_t our_len = 0;
        auto aa = buff_read<Arg1>(b, offset, our_len);
        auto bb = buff_read<Arg2, Arg3, Args...>(b, offset + our_len, our_len);
        len += our_len;
        return std::tuple_cat(aa, bb);
    }


    template<buff_elem T>
    inline T buff_peek_one(const buff_t& b, size_t offset) noexcept {
        size_t len = 0;
        return buff_read_one<T>(b, offset, len);
    }

    template<buff_elem... Args>
    inline std::tuple<Args...> buff_peek(const buff_t& b, size_t offset) noexcept {
        size_t len = 0;
        return buff_read<Args...>(b, offset, len);
    }
}

