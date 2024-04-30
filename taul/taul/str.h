

#pragma once


#include <memory>
#include <string>
#include <string_view>
#include <format>
#include <functional>

#include "asserts.h"
#include "strings.h"
#include "encoding.h"


namespace taul {


    namespace internal {


        struct str_literal_ctor_tag final {};
    }


    // this is an custom string type to be used within TAUL, w/ 
    // features that play into what TAUL needs, including:
    //      1) immutability
    //      2) thread-safety
    //      3) hash precomputation
    //      4) length-based (ie. not null terminated)
    //      5) exploits string literals to avoid heap alloc

    // above, #2 is implied by #1

    // for #5 above, the idea is that since string literals are
    // put statically into read-only parts of system memory, such
    // that pointers to them remain good for the lifetime of the
    // process, this can be exploited by having these immutable
    // strings point to this memory, thus w/out needing to alloc


    template<typename Char>
    class basic_str;


    // remember that further down we define a ***_str custom string
    // literal for defining taul::basic_str

    using str       = basic_str<char>;
    using wstr      = basic_str<wchar_t>;
    using str8      = basic_str<char8_t>;
    using str16     = basic_str<char16_t>;
    using str32     = basic_str<char32_t>;


    template<typename Char>
    class basic_str final {
    public:

        using char_t                    = Char;

        using this_t                    = basic_str<char_t>;


        using iterator                  = std::basic_string_view<char_t>::const_iterator;
        using const_iterator            = iterator;
        using reverse_iterator          = std::basic_string_view<char_t>::const_reverse_iterator;
        using const_reverse_iterator    = reverse_iterator;


        // this is used to allow custom string literals to define their string
        // w/out alloc, and in a way that is length-based, rather than using
        // a C-string, so our custom string literals can contain stray nulls

        // I don't want the end-user to be able to do this w/ taul::str, as I
        // feeling adding in unowned memory strings other than via literals
        // to static memory would add unhelpful nuance to the considerations
        // end-users must make when reasoning about taul::str

        // internal, do not use
        inline basic_str(internal::str_literal_ctor_tag, const char_t* x, std::size_t len);


        // this initializes an empty string, w/out alloc

        inline basic_str();

        // initializes a string of length len, copying the
        // first len chars from ptr into it, w/ alloc

        inline basic_str(const char_t* ptr, std::size_t len);

        // initializes a string of length n, filling it
        // w/ the value chr, w/ alloc

        inline basic_str(std::size_t n, char_t chr);

        // converts a standard library string into this string type

        inline explicit basic_str(const std::basic_string<char_t>& x);

        // converts a standard library string view into this string type

        inline explicit basic_str(std::basic_string_view<char_t> x);

        inline basic_str(const this_t& x);
        inline basic_str(this_t&& x) noexcept;

        inline ~basic_str() noexcept;

        inline this_t& operator=(const this_t& rhs);
        inline this_t& operator=(this_t&& rhs) noexcept;


        // this accepts a pointer to a C-string literal at x, and 
        // returns a string which refers to the static memory of it, 
        // w/out heap alloc

        // I want this to be kept explicit as I worry that allowing
        // for implicit creation of such strings could result in
        // scenarios where C-strings to non-static memory could
        // quietly initialize strings, only for this memory to
        // be deinitialized prior to the end of string lifetime

        static inline this_t lit(const char_t* x) noexcept;


        inline std::size_t size() const noexcept;
        inline std::size_t length() const noexcept;

        inline bool has_size() const noexcept;
        inline bool has_length() const noexcept;
        inline bool empty() const noexcept;


        // data returns a pointer to the chars of the string

        // returns nullptr for an empty string

        inline const char_t* data() const noexcept;


        // at returns the char at ind in the string

        // throws std::out_of_range if ind is out-of-bounds

        inline char_t at(std::size_t ind) const;

        // at_unchecked returns the char at ind in the string

        // behaviour is undefined if ind is out-of-bounds

        inline char_t at_unchecked(std::size_t ind) const noexcept;

        inline char_t operator[](std::size_t ind) const noexcept;

        // back returns the final char in the string

        // throws std::out_of_range if string is empty

        inline char_t back() const;

        // front returns the first char in the string

        // throws std::out_of_range if string is empty

        inline char_t front() const;


        // substr returns a substring of this string, starting
        // at offset, and continuing for len chars

        // TODO: should we change below to provide guarantee?
        // TODO: if so, add tests for it

        // notice that while the substring will most likely share
        // the same underlying memory, it is not required to, especially
        // w/ regards to empty substrings

        // if offset >= length(), an empty string will be returned

        // if len exceeds the remaining length of the string, it
        // will be reduced to fit

        inline this_t substr(
            std::size_t offset, 
            std::size_t len = std::size_t(-1)) const;


        inline iterator begin() const noexcept;
        inline const_iterator cbegin() const noexcept;
        inline reverse_iterator rbegin() const noexcept;
        inline const_reverse_iterator crbegin() const noexcept;

        inline iterator end() const noexcept;
        inline const_iterator cend() const noexcept;
        inline reverse_iterator rend() const noexcept;
        inline const_reverse_iterator crend() const noexcept;


        // these strings can be converted explicitly into 
        // standard library strings and string views

        inline explicit operator std::basic_string<char_t>() const;

        // these strings can be converted implicitly into 
        // standard library strings

        inline operator std::basic_string_view<char_t>() const noexcept;


        // equal returns if a and b are equal

        static inline bool equal(const this_t& a, const this_t& b) noexcept;

        // TODO: maybe add versions of below two w/ left-hand side being non-string

        static inline bool equal(const this_t& a, std::basic_string_view<char_t> b) noexcept;
        static inline bool equal(const this_t& a, const char_t* b) noexcept;


        // concat returns the concatenation of strings a and b

        static inline this_t concat(const this_t& a, const this_t& b);

        // TODO: maybe add versions of below two w/ left-hand side being non-string

        static inline this_t concat(const this_t& a, std::basic_string_view<char_t> b);
        static inline this_t concat(const this_t& a, const char_t* b);


        inline std::string fmt(unicode_t err = default_err) const;


        inline std::size_t hash() const noexcept;


    private:
        
        std::shared_ptr<char_t[]> _storage = nullptr;
        const char_t* _start = nullptr;
        std::size_t _length = 0;
        std::size_t _hash = std::size_t(-1);


        struct no_precompute_hash_tag {};
        struct init_via_literal_tag {};
        struct init_via_substr_tag {};
        struct init_via_concat_tag {};

        inline basic_str(no_precompute_hash_tag);
        inline basic_str(init_via_literal_tag, const char_t* x);
        inline basic_str(init_via_substr_tag, const this_t& original, std::size_t offset, std::size_t len);
        inline basic_str(init_via_concat_tag, std::basic_string_view<char_t> a, std::basic_string_view<char_t> b);
        

        inline void _assert_default_init() const noexcept;

        inline void _alloc_storage(std::size_t len);

        inline void _init_via_storage_copy(const char_t* ptr);
        inline void _init_via_storage_fill(char_t chr);
        inline void _init_via_literal(const char_t* x);
        inline void _init_via_substr(const this_t& original, std::size_t offset, std::size_t len);
        inline void _init_via_concat(std::basic_string_view<char_t> a, std::basic_string_view<char_t> b);
        inline void _init_via_custom_literal(const char_t* x, std::size_t len);

        inline void _precompute_hash() noexcept;

        inline void _assign_fields(this_t& dest, const this_t& src);
        inline void _swap_fields(this_t& dest, this_t& src);

        inline std::basic_string_view<char_t> _get_string_view() const noexcept;
    };


    template<typename Char>
    inline bool operator==(const taul::basic_str<Char>& lhs, const taul::basic_str<Char>& rhs) noexcept {
        return taul::basic_str<Char>::equal(lhs, rhs);
    }

    template<typename Char>
    inline bool operator==(const taul::basic_str<Char>& lhs, std::basic_string_view<Char> rhs) noexcept {
        return taul::basic_str<Char>::equal(lhs, rhs);
    }

    template<typename Char>
    inline bool operator==(const taul::basic_str<Char>& lhs, const Char* rhs) noexcept {
        TAUL_ASSERT(rhs);
        return taul::basic_str<Char>::equal(lhs, rhs);
    }

    template<typename Char>
    inline bool operator!=(const taul::basic_str<Char>& lhs, const taul::basic_str<Char>& rhs) noexcept {
        return !taul::basic_str<Char>::equal(lhs, rhs);
    }

    template<typename Char>
    inline bool operator!=(const taul::basic_str<Char>& lhs, std::basic_string_view<Char> rhs) noexcept {
        return !taul::basic_str<Char>::equal(lhs, rhs);
    }

    template<typename Char>
    inline bool operator!=(const taul::basic_str<Char>& lhs, const Char* rhs) noexcept {
        TAUL_ASSERT(rhs);
        return !taul::basic_str<Char>::equal(lhs, rhs);
    }

    template<typename Char>
    inline taul::basic_str<Char> operator+(const taul::basic_str<Char>& lhs, const taul::basic_str<Char>& rhs) {
        return taul::basic_str<Char>::concat(lhs, rhs);
    }

    template<typename Char>
    inline taul::basic_str<Char> operator+(const taul::basic_str<Char>& lhs, std::basic_string_view<Char> rhs) {
        return taul::basic_str<Char>::concat(lhs, rhs);
    }

    template<typename Char>
    inline taul::basic_str<Char> operator+(const taul::basic_str<Char>& lhs, const Char* rhs) {
        return taul::basic_str<Char>::concat(lhs, rhs);
    }


    template<typename Char>
    inline basic_str<Char>::basic_str(internal::str_literal_ctor_tag, const char_t* x, std::size_t len)
        : basic_str(no_precompute_hash_tag{}) {
        TAUL_ASSERT(x);
        _init_via_custom_literal(x, len);
        _precompute_hash();
    }

    template<typename Char>
    inline basic_str<Char>::basic_str()
        : basic_str(no_precompute_hash_tag{}) {
        _precompute_hash();
    }

    template<typename Char>
    inline basic_str<Char>::basic_str(const char_t* ptr, std::size_t len) 
        : basic_str(no_precompute_hash_tag{}) {
        TAUL_ASSERT(ptr);
        _alloc_storage(len);
        _init_via_storage_copy(ptr);
        _precompute_hash();
    }

    template<typename Char>
    inline basic_str<Char>::basic_str(std::size_t n, char_t chr) 
        : basic_str(no_precompute_hash_tag{}) {
        _alloc_storage(n);
        _init_via_storage_fill(chr);
        _precompute_hash();
    }

    template<typename Char>
    inline basic_str<Char>::basic_str(const std::basic_string<char_t>& x) 
        : basic_str(x.data(), x.length()) {}

    template<typename Char>
    inline basic_str<Char>::basic_str(std::basic_string_view<char_t> x) 
        : basic_str(x.data(), x.length()) {}

    template<typename Char>
    inline basic_str<Char>::basic_str(const this_t& x)
        : basic_str(no_precompute_hash_tag{}) {
        _assign_fields(*this, x);
    }

    template<typename Char>
    inline basic_str<Char>::basic_str(this_t&& x) noexcept
        : basic_str() { // <- precompute hash, then swap
        _swap_fields(*this, x);
    }

    template<typename Char>
    inline basic_str<Char>::~basic_str() noexcept {}

    template<typename Char>
    inline basic_str<Char>::this_t& basic_str<Char>::operator=(const this_t& rhs) {
        _assign_fields(*this, rhs);
        return *this;
    }
    
    template<typename Char>
    inline basic_str<Char>::this_t& basic_str<Char>::operator=(this_t&& rhs) noexcept {
        if (&rhs != this) {
            _swap_fields(*this, rhs);
        }
        return *this;
    }
    
    template<typename Char>
    inline basic_str<Char>::this_t basic_str<Char>::lit(const char_t* x) noexcept {
        TAUL_ASSERT(x);
        return this_t(init_via_literal_tag{}, x);
    }
    
    template<typename Char>
    inline std::size_t basic_str<Char>::size() const noexcept {
        return _length;
    }
    
    template<typename Char>
    inline std::size_t basic_str<Char>::length() const noexcept {
        return size();
    }
    
    template<typename Char>
    inline bool basic_str<Char>::has_size() const noexcept {
        return size() > 0;
    }
    
    template<typename Char>
    inline bool basic_str<Char>::has_length() const noexcept {
        return has_size();
    }
    
    template<typename Char>
    inline bool basic_str<Char>::empty() const noexcept {
        return !has_length();
    }

    template<typename Char>
    inline const basic_str<Char>::char_t* basic_str<Char>::data() const noexcept {
        return _start;
    }
    
    template<typename Char>
    inline basic_str<Char>::char_t basic_str<Char>::at(std::size_t ind) const {
        return _get_string_view().at(ind);
    }

    template<typename Char>
    inline basic_str<Char>::char_t basic_str<Char>::at_unchecked(std::size_t ind) const noexcept {
        return _get_string_view()[ind];
    }

    template<typename Char>
    inline basic_str<Char>::char_t basic_str<Char>::operator[](std::size_t ind) const noexcept {
        return at_unchecked(ind);
    }

    template<typename Char>
    inline basic_str<Char>::char_t basic_str<Char>::back() const {
        return at(length() - 1);
    }

    template<typename Char>
    inline basic_str<Char>::char_t basic_str<Char>::front() const {
        return at(0);
    }

    template<typename Char>
    inline basic_str<Char>::this_t basic_str<Char>::substr(std::size_t offset, std::size_t len) const {
        return this_t(init_via_substr_tag{}, *this, offset, len);
    }

    template<typename Char>
    inline basic_str<Char>::iterator basic_str<Char>::begin() const noexcept {
        return _get_string_view().begin();
    }

    template<typename Char>
    inline basic_str<Char>::const_iterator basic_str<Char>::cbegin() const noexcept {
        return begin();
    }

    template<typename Char>
    inline basic_str<Char>::reverse_iterator basic_str<Char>::rbegin() const noexcept {
        return _get_string_view().rbegin();
    }

    template<typename Char>
    inline basic_str<Char>::const_reverse_iterator basic_str<Char>::crbegin() const noexcept {
        return rbegin();
    }

    template<typename Char>
    inline basic_str<Char>::iterator basic_str<Char>::end() const noexcept {
        return _get_string_view().end();
    }

    template<typename Char>
    inline basic_str<Char>::const_iterator basic_str<Char>::cend() const noexcept {
        return end();
    }

    template<typename Char>
    inline basic_str<Char>::reverse_iterator basic_str<Char>::rend() const noexcept {
        return _get_string_view().rend();
    }

    template<typename Char>
    inline basic_str<Char>::const_reverse_iterator basic_str<Char>::crend() const noexcept {
        return rend();
    }

    template<typename Char>
    inline basic_str<Char>::operator std::basic_string<typename basic_str<Char>::char_t>() const {
        return (std::basic_string<char_t>)_get_string_view();
    }

    template<typename Char>
    inline basic_str<Char>::operator std::basic_string_view<typename basic_str<Char>::char_t>() const noexcept {
        return _get_string_view();
    }
    
    template<typename Char>
    inline bool basic_str<Char>::equal(const this_t& a, const this_t& b) noexcept {
        return equal(a, b._get_string_view());
    }

    template<typename Char>
    inline bool basic_str<Char>::equal(const this_t& a, std::basic_string_view<char_t> b) noexcept {
        return
            a.length() == b.length() &&
            std::equal(a.begin(), a.end(), b.begin());
    }

    template<typename Char>
    inline bool basic_str<Char>::equal(const this_t& a, const char_t* b) noexcept {
        TAUL_ASSERT(b);
        return equal(a, std::basic_string_view<char_t>(b, strlen(b)));
    }

    template<typename Char>
    inline basic_str<Char>::this_t basic_str<Char>::concat(const this_t& a, const this_t& b) {
        return concat(a, b._get_string_view());
    }

    template<typename Char>
    inline basic_str<Char>::this_t basic_str<Char>::concat(const this_t& a, std::basic_string_view<char_t> b) {
        return basic_str(init_via_concat_tag{}, a._get_string_view(), b);
    }

    template<typename Char>
    inline basic_str<Char>::this_t basic_str<Char>::concat(const this_t& a, const char_t* b) {
        TAUL_ASSERT(b);
        return concat(a, std::basic_string_view<char_t>(b, strlen(b)));
    }

    namespace internal {
        // this ad hoc traits-like type is used by fmt to decide how formatted strings
        // should be prepared, such that code for one char type doesn't have to be 
        // compiled in situations where a differently sized char type is used

        template<typename Char>
        struct _fmt_helper {};

        template<>
        struct _fmt_helper<char> {
            inline std::string operator()(std::string_view x, unicode_t) const {
                return std::string(x);
            }
        };

        template<>
        struct _fmt_helper<char8_t> {
            inline std::string operator()(std::u8string_view x, unicode_t) const {
                std::string result(x.length(), ' ');
                std::memcpy((void*)result.data(), (const void*)x.data(), x.length());
                return result;
            }
        };

        template<>
        struct _fmt_helper<char16_t> {
            inline std::string operator()(std::u16string_view x, unicode_t err) const {
                return taul::convert_encoding<char>(encoding::utf16, encoding::utf8, x, err).value();
            }
        };

        template<>
        struct _fmt_helper<char32_t> {
            inline std::string operator()(std::u32string_view x, unicode_t err) const {
                return taul::convert_encoding<char>(encoding::utf32, encoding::utf8, x, err).value();
            }
        };

        template<>
        struct _fmt_helper<wchar_t> {
            inline std::string operator()(std::wstring_view x, unicode_t err) const {
                return taul::convert_encoding<char>(wchar_encoding(encoding::utf16), encoding::utf8, x, err).value();
            }
        };
    }

    template<typename Char>
    inline std::string basic_str<Char>::fmt(unicode_t err) const {
        return internal::_fmt_helper<Char>{}(_get_string_view(), err);
    }
    
    template<typename Char>
    inline std::size_t basic_str<Char>::hash() const noexcept {
        return _hash;
    }

    template<typename Char>
    inline basic_str<Char>::basic_str(basic_str<Char>::no_precompute_hash_tag) {}
    
    template<typename Char>
    inline basic_str<Char>::basic_str(basic_str<Char>::init_via_literal_tag, const char_t* x)
        : basic_str(no_precompute_hash_tag{}) {
        _init_via_literal(x);
        _precompute_hash();
    }
    
    template<typename Char>
    inline basic_str<Char>::basic_str(init_via_substr_tag, const this_t& original, std::size_t offset, std::size_t len)
        : basic_str(no_precompute_hash_tag{}) {
        _init_via_substr(original, offset, len);
        _precompute_hash();
    }

    template<typename Char>
    inline basic_str<Char>::basic_str(init_via_concat_tag, std::basic_string_view<char_t> a, std::basic_string_view<char_t> b)
        : basic_str(no_precompute_hash_tag{}) {
        _alloc_storage(a.length() + b.length());
        _init_via_concat(a, b);
        _precompute_hash();
    }

    template<typename Char>
    inline void basic_str<Char>::_assert_default_init() const noexcept {
        TAUL_ASSERT(!_storage);
        TAUL_ASSERT(!_start);
        TAUL_ASSERT(_length == 0);
    }

    template<typename Char>
    inline void basic_str<Char>::_alloc_storage(std::size_t len) {
        _assert_default_init();
        if (len > 0) {
            _storage = std::make_shared<char_t[]>(len);
            _start = _storage.get();
            _length = len;
        }
    }
    
    template<typename Char>
    inline void basic_str<Char>::_init_via_storage_copy(const char_t* ptr) {
        TAUL_ASSERT(ptr);
        if (_length == 0) return;
        TAUL_ASSERT(_start);
        std::memcpy((void*)_start, (const void*)ptr, _length * sizeof(char_t));
    }
    
    template<typename Char>
    inline void basic_str<Char>::_init_via_storage_fill(char_t chr) {
        if (_length == 0) return;
        TAUL_ASSERT(_start);
        std::fill((char_t*)_start, (char_t*)(_start + _length), chr);
    }

    template<typename Char>
    inline void basic_str<Char>::_init_via_literal(const char_t* x) {
        TAUL_ASSERT(x);
        _assert_default_init();
        _storage = nullptr;
        _start = x;
        _length = strlen(x);
    }

    template<typename Char>
    inline void basic_str<Char>::_init_via_substr(const this_t& original, std::size_t offset, std::size_t len) {
        _assert_default_init();
        const auto s = original._get_string_view().substr(offset, len);
        _storage = original._storage;
        _start = s.data();
        _length = s.length();
    }

    template<typename Char>
    inline void basic_str<Char>::_init_via_concat(std::basic_string_view<char_t> a, std::basic_string_view<char_t> b) {
        std::memcpy((void*)_start, (const void*)a.data(), a.length() * sizeof(char_t));
        // don't multiply a.length() by sizeof(char_t) below, since sizeof(_start) may be >1 bytes
        std::memcpy((void*)(_start + a.length()), (const void*)b.data(), b.length() * sizeof(char_t));
    }

    template<typename Char>
    inline void basic_str<Char>::_init_via_custom_literal(const char_t* x, std::size_t len) {
        TAUL_ASSERT(x);
        _assert_default_init();
        _storage = nullptr;
        _start = x;
        _length = len;
    }

    template<typename Char>
    inline void basic_str<Char>::_precompute_hash() noexcept {
        _hash = std::hash<std::basic_string_view<char_t>>{}(_get_string_view());
    }

    template<typename Char>
    inline void basic_str<Char>::_assign_fields(this_t& dest, const this_t& src) {
        dest._storage = src._storage;
        dest._start = src._start;
        dest._length = src._length;
        dest._hash = src._hash;
    }
    
    template<typename Char>
    inline void basic_str<Char>::_swap_fields(this_t& dest, this_t& src) {
        std::swap(dest._storage, src._storage);
        std::swap(dest._start, src._start);
        std::swap(dest._length, src._length);
        std::swap(dest._hash, src._hash);
    }

    template<typename Char>
    inline std::basic_string_view<typename basic_str<Char>::char_t> basic_str<Char>::_get_string_view() const noexcept {
        return 
            _start
            ? std::basic_string_view<char_t>(_start, _length) 
            : std::basic_string_view<char_t>{};
    }


    namespace string_literals {


        inline str operator""_str(const char* x, std::size_t len) { return str(internal::str_literal_ctor_tag{}, x, len); }
        inline wstr operator""_str(const wchar_t* x, std::size_t len) { return wstr(internal::str_literal_ctor_tag{}, x, len); }
        inline str8 operator""_str(const char8_t* x, std::size_t len) { return str8(internal::str_literal_ctor_tag{}, x, len); }
        inline str16 operator""_str(const char16_t* x, std::size_t len) { return str16(internal::str_literal_ctor_tag{}, x, len); }
        inline str32 operator""_str(const char32_t* x, std::size_t len) { return str32(internal::str_literal_ctor_tag{}, x, len); }
    }

    namespace literals {


        using namespace taul::string_literals;
    }
}


template<typename Char>
struct std::formatter<taul::basic_str<Char>> final : std::formatter<std::string> {
    auto format(const taul::basic_str<Char>& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    template<typename Char>
    inline std::ostream& operator<<(std::ostream& stream, const taul::basic_str<Char>& x) {
        return stream << x.fmt();
    }
}

namespace std {
    template<typename Char>
    struct std::hash<taul::basic_str<Char>> {
        inline std::size_t operator()(const taul::basic_str<Char>& s) const noexcept {
            return s.hash();
        }
    };
}

