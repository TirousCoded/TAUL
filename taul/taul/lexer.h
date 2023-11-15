

#pragma once


#include <cstdint>
#include <memory>
#include <string_view>

#include "logger.h"
#include "token.h"


namespace taul {


    class lexer_state;


    // the behaviour of lexer function objects is defined by two pieces 
    // of information:
    //      1) a taul::lexer_function function which defines the behaviour of
    //         the lexer
    //      2) an (optional) taul::lexer_state object which defines any additional 
    //         state which may be needed for the lexer function object to operate
    //         (for thread-safety this should be immutable)

    typedef token(*lexer_function)(
        const std::shared_ptr<lexer_state>& state,
        std::string_view txt,
        source_pos offset,
        const logger_ref& lgr);

    class lexer_state {
    public:

        virtual ~lexer_state() noexcept = default;
    };


    inline token default_lexer_function(
        const std::shared_ptr<lexer_state>&,
        std::string_view,
        source_pos,
        const logger_ref&) {
        return token{};
    }


    class lexer final {
    public:

        // behaviour is undefined if f == nullptr

        lexer(lexer_function f, const std::shared_ptr<lexer_state>& state = nullptr);

        // default initialized lexer function objects will be valid, but *trivial*, lexer
        // function object which returns taul::token{}

        lexer();

        lexer(const lexer& x);
        lexer(lexer&& x) noexcept;

        ~lexer() noexcept = default;

        lexer& operator=(const lexer& rhs);
        lexer& operator=(lexer&& rhs) noexcept;


        token operator()(
            std::string_view txt,
            const logger_ref& lgr = nullptr) const;

        // the 'offset' below refers to where in txt lexical analysis is to begin

        // behaviour is undefined if offset > txt.length()

        token operator()(
            std::string_view txt,
            source_pos offset,
            const logger_ref& lgr = nullptr) const;


    private:

        lexer_function _f = nullptr;
        std::shared_ptr<lexer_state> _state = nullptr;
    };
}

