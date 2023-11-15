

#pragma once


#include <stddef.h>
#include <string>
#include <string_view>
#include <format>

#include "source_code.h"
#include "rules.h"

#include <stdexcept>


namespace taul {


    class failure_token_error final : public std::logic_error {
    public:

        inline explicit failure_token_error(const std::string& msg) : logic_error(msg) {}
        inline explicit failure_token_error(const char* msg) : logic_error(msg) {}
    };


    struct token final {

        token(
            const lexer_rule& lpr, 
            std::string_view str, 
            source_pos pos = 0) noexcept;
        
        token();
        token(const token& x);
        token(token&& x) noexcept;

        ~token() noexcept = default;

        token& operator=(const token& rhs);
        token& operator=(token&& rhs) noexcept;


        // failure tokens encapsulate a portion of matched string which did not conform 
        // to the pattern in question

        bool is_failure() const noexcept;

        inline explicit operator bool() const noexcept { return !is_failure(); }


        // throws taul::failure_token_error if the token is a failure token        
        
        const lexer_rule& lpr() const;

        // this returns the portion of string matched by the token
        
        std::string_view str() const noexcept;

        // this returns the source code position of the token's str() string in the
        // broader text from which it was matched

        source_pos pos() const noexcept;


        bool equal(const token& other) const noexcept;

        inline bool operator==(const token& rhs) const noexcept { return equal(rhs); }
        inline bool operator!=(const token& rhs) const noexcept { return !equal(rhs); }


        std::string fmt() const;


        // this is used for initialization of 'failure tokens'

        static token failure(
            std::string_view str, 
            source_pos pos = 0) noexcept;


    private:

        const lexer_rule* _lpr;
        std::string_view _str;
        source_pos _pos;
    };
}


template<>
struct std::formatter<taul::token> final : std::formatter<std::string> {
    auto format(const taul::token& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

