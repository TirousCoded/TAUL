

#pragma once


#include <string_view>

#include "source_code.h"
#include "token.h"


namespace taul {


    // TODO: add in a notion of 'virtual tokens' later on

    // in order for syntactic nodes to be able to have a section of source string
    // associated w/ them which encompasses the direct/indirect tokens of that node,
    // and any unused source string between them, we need the token sequence provided
    // to the parser to obey certain invariants

    // these invariants are as follows:
    
    //      1) all tokens involved must share the same underlying source string

    //      2) tokens in the sequence must be organized such that their source
    //         string positions are in ascending order

    // taul::token_seq defines a token sequence for a source string which strictly
    // adheres to these invariants


    class token_seq final {
    public:

        using iterator = std::vector<token>::const_iterator;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        // initializes a new empty token sequence w/ as its underlying source string

        // TODO: maybe change below?

        // the token sequence does NOT allocate an internal string encapsulating s, and
        // thus it is the end-user's responsibility to ensure the memory of s is valid

        token_seq(std::string_view s);

        token_seq() = delete;
        token_seq(const token_seq& other);
        token_seq(token_seq&& other) noexcept;

        ~token_seq() noexcept;

        token_seq& operator=(const token_seq& rhs);
        token_seq& operator=(token_seq&& rhs) noexcept;


        // str returns the source string of the token sequence

        std::string_view str() const noexcept;


        // size returns the number of tokens in the token sequence

        std::size_t size() const noexcept;

        // has_size/empty return if the token sequence isn't/is empty, respectively

        bool has_size() const noexcept;
        bool empty() const noexcept;


        // data returns a pointer to the internal token array of the token sequence

        // data returns nullptr if empty

        const token* data() const;

        // at/operator[] returns the token at index ind in the token sequence

        // throws std::out_of_range if ind is out-of-bounds of the token sequence,
        // providing a strong guarantee

        const token& at(std::size_t ind) const;

        const token& operator[](std::size_t ind) const;

        // back returns the token at the back of the token sequence

        // throws std::out_of_range if the token sequence is empty,
        // providing a strong guarantee

        const token& back() const;


        // range_str returns a string view of the portion of source string starting
        // at the source string position of the first token specified, and ending
        // at the end of the source string portion of the last token specified

        // range_str specifies the above token range via a starting index ind, and 
        // a number of tokens n

        // throws std::out_of_range if the range of tokens specified is invalid,
        // providing a strong guarantee

        std::string_view range_str(std::size_t ind, std::size_t n) const;


        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;

        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crend() const noexcept;


        // read_pos returns the current read position of the token sequence

        // the read position determines the location the next token pushed will be
        // located at, or where the next skip operation will be performed

        source_pos read_pos() const noexcept;

        // end_pos returns the value read_pos() will return when the token sequence
        // has reached the end of its source string

        source_pos end_pos() const noexcept;

        // at_end returns if currently read_pos() == end_pos()

        bool at_end() const noexcept;


        // advances the read position by len, w/out pushing a token

        // throws std::out_of_range if specified len would exceed the source string,
        // providing a strong guarantee

        void skip(
            std::uint32_t len);

        // rewinds the read position back to the end of back token of the token 
        // sequence, or to the beginning of the source string, if there is no
        // such back token

        void undo_skips() noexcept;

        // pushes a new token of the portion of source string at the current read
        // position, of length len, to the back of the token sequence, advancing the 
        // read position by len

        // throws std::out_of_range if specified len would exceed the source string,
        // providing a strong guarantee

        void push(
            const lexer_rule& lpr,
            std::uint32_t len);
        
        // pushes a failure token of the portion of source string at the current read
        // position, of length len, to the back of the token sequence, advancing the 
        // read position by len

        // throws std::out_of_range if specified len would exceed the source string,
        // providing a strong guarantee

        void push_failure(
            std::uint32_t len);

        // pops n tokens from the back of the token sequence, rewinding the read
        // position back to the position of the final token popped

        // if n exceeds the number of available tokens to pop, n will be reduced

        void pop(
            std::size_t n = 1) noexcept;

        // pops all tokens from the token sequence, rewinding the read position
        // back to the beginning of the source string

        void pop_all() noexcept;

        // reinitializes the state of the token sequence using source string s, w/
        // this method ensuring that no internal reallocation occurs

        void reset(
            std::string_view s);


        std::string fmt(const char* tab = "    ") const;


    private:

        std::string_view _s;
        std::vector<token> _tkns;
        source_pos _readpos;


        bool _in_bounds(std::size_t ind, std::size_t n) const noexcept;
    };
}


template<>
struct std::formatter<taul::token_seq> final : std::formatter<std::string> {
    auto format(const taul::token_seq& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::token_seq& x) {
        return stream << x.fmt();
    }
}

