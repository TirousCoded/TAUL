

#pragma once


#include <cstdint>
#include <string>
#include <vector>

#include "../general.h"
#include "../asserts.h"
#include "../endian.h"

#include "util.h"


namespace taul::internal {


    // IMPORTANT: the below is designed to be used both for things like specs, but also for
    //            grammar serialization/deserialization, and so needs to be portable in terms
    //            of things like endianness


    using buff_t = std::vector<uint8_t>;

    template<typename T>
    concept buff_elem_type =
        std::integral<std::remove_cvref_t<T>> ||
        enum_type<std::remove_cvref_t<T>> ||
        std::same_as<std::remove_cvref_t<T>, std::string_view>;


    inline buff_t buff_concat(const buff_t& lhs, const buff_t& rhs) {
        buff_t result{};
        result.resize(lhs.size() + rhs.size());
        if (!result.empty()) {
            std::memcpy((void*)result.data(), (void*)lhs.data(), lhs.size());
            std::memcpy((void*)(result.data() + lhs.size()), (void*)rhs.data(), rhs.size());
        }
        return result;
    }


    template<std::integral T>
    inline void buff_write_one(buff_t& b, T x) {
        T temp = to_little_endian(x); // ensure portability
        // alloc and copy over temp
        b.resize(b.size() + sizeof(T));
        std::memcpy((void*)&b[b.size() - sizeof(T)], (const void*)&temp, sizeof(T));
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

    template<buff_elem_type Arg, buff_elem_type... Args>
    inline void buff_write(buff_t& b, Arg&& arg, Args&&... args) {
        buff_write_one(b, std::remove_cvref_t<Arg>(std::forward<Arg>(arg)));
        buff_write(b, std::forward<Args>(args)...);
    }


    // the len out param will be incremented by sizeof(T) upon read, w/ this being
    // used to simplify writing stuff that reads buffers

    template<std::integral T>
    inline T buff_read_one(const buff_t& b, size_t offset, size_t& len) noexcept {
        // assert [offset, offset + sizeof(T)) is in-bounds
        TAUL_IN_BOUNDS(offset, 0, b.size());
        if constexpr (sizeof(T) > 0) TAUL_IN_BOUNDS(offset + sizeof(T) - 1, 0, b.size());
        len += sizeof(T); // incr len
        // copy data to result
        T result{};
        std::memcpy((void*)&result, (const void*)(b.data() + offset), sizeof(T));
        return from_little_endian(result); // ensure portability
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

    // template overloads for 0 and 1 args

    inline std::tuple<> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
        return std::make_tuple();
    }

    template<buff_elem_type T>
    inline std::tuple<T> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
            return std::make_tuple(buff_read_one<T>(b, offset, len));
    }

    // template overload for 2 args

    // for exactly 2 template params, we can quality-of-life return a std::pair

    template<buff_elem_type T, buff_elem_type U>
    inline std::pair<T, U> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
        size_t our_len = 0;
        // NOTE: can't put below in std::make_pair due to C++ not having defined arg eval order!
        auto aa = buff_read_one<T>(b, offset, our_len);
        auto bb = buff_read_one<U>(b, offset + our_len, our_len);
        len += our_len;
        return std::make_pair(aa, bb);
    }

    // template overload for +3 args

    template<buff_elem_type Arg1, buff_elem_type Arg2, buff_elem_type Arg3, buff_elem_type... Args>
    inline std::tuple<Arg1, Arg2, Arg3, Args...> buff_read(const buff_t& b, size_t offset, size_t& len) noexcept {
        size_t our_len = 0;
        // NOTE: can't put below in std::tuple_cat due to C++ not having defined arg eval order!
        auto aa = buff_read<Arg1>(b, offset, our_len);
        auto bb = buff_read<Arg2, Arg3, Args...>(b, offset + our_len, our_len);
        len += our_len;
        return std::tuple_cat(aa, bb);
    }


    template<buff_elem_type T>
    inline T buff_peek_one(const buff_t& b, size_t offset) noexcept {
        size_t len = 0;
        return buff_read_one<T>(b, offset, len);
    }

    template<buff_elem_type... Args>
    inline std::tuple<Args...> buff_peek(const buff_t& b, size_t offset) noexcept {
        size_t len = 0;
        return buff_read<Args...>(b, offset, len);
    }
}

