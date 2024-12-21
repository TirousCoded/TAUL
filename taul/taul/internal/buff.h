

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


    template<typename T>
    concept buff_elem_type =
        std::integral<std::remove_cvref_t<T>> ||
        enum_type<std::remove_cvref_t<T>> ||
        std::same_as<std::remove_cvref_t<T>, std::string_view>;


    struct buff final {
        std::vector<uint8_t> bytes;


        inline size_t size() const noexcept { return bytes.size(); }
        inline bool has_size() const noexcept { return !bytes.empty(); }
        inline bool empty() const noexcept { return bytes.empty(); }

        inline uint8_t* data() noexcept { return bytes.data(); }
        inline const uint8_t* data() const noexcept { return bytes.data(); }


        template<std::integral T>
        inline buff& write_one(T x) {
            T temp = to_little_endian(x); // ensure portability
            _write_back(&temp, 1);
            return *this;
        }

        template<enum_type T>
        inline buff& write_one(T x) {
            return write_one(to_underlying(x));
        }

        template<std::same_as<std::string_view> T>
        inline buff& write_one(T x) {
            TAUL_IN_BOUNDS(x.length(), 0, std::numeric_limits<uint32_t>::max());
            write_one((uint32_t)x.length()); // write length
            _write_back(x.data(), x.length());
            return *this;
        }


        inline buff& write() {
            return *this; // do nothing
        }

        template<buff_elem_type Arg, buff_elem_type... Args>
        inline buff& write(Arg&& arg, Args&&... args) {
            write_one(std::remove_cvref_t<Arg>(std::forward<Arg>(arg)));
            return write(std::forward<Args>(args)...);
        }


        static inline buff concat(const buff& lhs, const buff& rhs) {
            buff result{};
            result.bytes.resize(lhs.size() + rhs.size());
            if (result.has_size()) {
                copy_bytes(lhs.data(), lhs.size(), result.data());
                copy_bytes(rhs.data(), rhs.size(), result.data() + lhs.size());
            }
            return result;
        }


    private:

        template<trivially_copyable_type T>
        inline void _write_back(const T* x, size_t n) {
            bytes.resize(size() + n * sizeof(T));
            copy_bytes((const uint8_t*)x, n * sizeof(T), data() + (size() - n * sizeof(T)));
        }
    };


    struct buff_reader final {
        const buff* target = nullptr;
        size_t offset = 0;


        inline buff_reader(const buff& target, size_t offset = 0)
            : target(&target),
            offset(offset) {}

        buff_reader() = delete;
        buff_reader(const buff_reader&) = default;
        buff_reader(buff_reader&&) noexcept = default;

        ~buff_reader() noexcept = default;

        buff_reader& operator=(const buff_reader&) = default;
        buff_reader& operator=(buff_reader&&) noexcept = default;


        // when parsing some input, it's useful to clone the buff_reader, use
        // that clone for the parse, and then assign this clone back onto *this,
        // as this method lets us easily undo buff_reader state changes in the
        // event that the parse fails

        // we'll use two methods to encapsulate this pattern: 'fork' for the
        // copy itself, and 'commit' for the assigning back onto *this

        // we can also use fork for impl of 'peek' methods, by simply never
        // committing the forked buff_reader

        inline buff_reader fork() const noexcept { return *this; }
        inline void commit(buff_reader x) noexcept { *this = std::move(x); }


        // returns number of bytes remaining until end of buffer

        inline size_t remaining() const noexcept {
            TAUL_ASSERT(offset <= deref_assert(target).size());
            return deref_assert(target).size() - offset;
        }


        template<std::integral T>
        inline std::optional<T> read_one() noexcept {
            return _next_one<T>();
        }

        template<enum_type T>
        inline std::optional<T> read_one() noexcept {
            auto a = read_one<std::underlying_type_t<T>>();
            return
                a
                ? std::make_optional((T)*a)
                : std::nullopt;
        }

        template<std::same_as<std::string_view>>
        inline std::optional<std::string_view> read_one() noexcept {
            auto rdr = fork();
            std::optional<std::string_view> result{};
            if (const auto length = rdr.read_one<uint32_t>()) {
                if (const auto data = rdr._next_span(length.value())) {
                    commit(rdr);
                    result = std::string_view((const char*)data->data(), data->size());
                }
            }
            return result;
        }


        inline std::optional<std::tuple<>> read() noexcept {
            return std::make_tuple();
        }

        template<buff_elem_type T>
        inline std::optional<std::tuple<T>> read() noexcept {
            auto rdr = fork();
            std::optional<std::tuple<T>> result{};
            if (const auto a = rdr.read_one<T>()) {
                commit(rdr);
                result = std::make_tuple(*a);
            }
            return result;
        }

        template<buff_elem_type Arg1, buff_elem_type Arg2, buff_elem_type... Args>
        inline std::optional<std::tuple<Arg1, Arg2, Args...>> read() noexcept {
            auto rdr = fork();
            std::optional<std::tuple<Arg1, Arg2, Args...>> result{};
            if (const auto a = rdr.read<Arg1>()) {
                if (const auto b = rdr.read<Arg2, Args...>()) {
                    commit(rdr);
                    result = std::tuple_cat(*a, *b);
                }
            }
            return result;
        }


        template<buff_elem_type T>
        inline std::optional<T> peek_one() noexcept {
            return fork().read_one<T>();
        }

        template<buff_elem_type... Args>
        inline std::optional<std::tuple<Args...>> peek() noexcept {
            return fork().read<Args...>();
        }


    private:

        template<trivially_copyable_type T>
        inline std::optional<T> _next_one() noexcept {
            if (remaining() < sizeof(T)) return std::nullopt;
            T result{};
            copy_bytes((const uint8_t*)deref_assert(target).data() + offset, sizeof(T), (uint8_t*)&result);
            offset += sizeof(T);
            return from_little_endian(result); // ensure portability
        }

        inline std::optional<std::span<const uint8_t>> _next_span(size_t n) noexcept {
            if (remaining() < n) return std::nullopt;
            const auto result = std::span<const uint8_t>((const uint8_t*)deref_assert(target).data() + offset, n);
            offset += n;
            return result;
        }
    };
}

