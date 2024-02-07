

#pragma once


// IMPORTANT: this was part of the frontend, but I've moved this to the backend


#include <memory>
#include <string_view>

#include "../../logger.h"
#include "../../token.h"

#include "../../asserts.h"


namespace taul::internal {


    class lexer_state;

    class lexer;


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
        const std::shared_ptr<logger>& lgr);

    // TODO: in our *official* documentation, do be sure to remind the end-user to
    //       be careful about including grammar shared pointers in lexer state,
    //       as the lexers themselves will be stored within grammars, so if we're
    //       not careful, this could create a strong reference cycle

    class lexer_state {
    public:

        virtual ~lexer_state() noexcept = default;
    };


    token default_lexer_function(
        const std::shared_ptr<lexer_state>&,
        std::string_view,
        source_pos,
        const std::shared_ptr<logger>&);


    lexer_function get_lexer_f(const lexer& x) noexcept;


    class lexer final {
    public:

        friend lexer_function get_lexer_f(const lexer&) noexcept;


        // behaviour is undefined if f == nullptr

        lexer(
            lexer_function f, 
            const std::shared_ptr<lexer_state>& state = nullptr);

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
            const std::shared_ptr<logger>& lgr = nullptr) const;

        // the 'offset' below refers to where in txt lexical analysis is to begin

        // behaviour is undefined if offset > txt.length() (notice that this allows
        // for the case where offset == txt.length())

        token operator()(
            std::string_view txt,
            source_pos offset,
            const std::shared_ptr<logger>& lgr = nullptr) const;


    private:

        lexer_function _f = nullptr;
        std::shared_ptr<lexer_state> _state = nullptr;
    };
}

