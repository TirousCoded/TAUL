

#pragma once


#include <memory>
#include <string_view>
#include <optional>

#include "logger.h"
#include "token.h"
#include "token_seq.h"


namespace taul {


    class node;
    class node_ctx;


    class parser_state;

    class parser;


    // the behaviour of parser function objects is defined by three pieces 
    // of information:
    //      1) a taul::parser_function function which defines the behaviour of
    //         the parser
    //      2) an (optional) taul::parser_state object which defines any additional 
    //         state which may be needed for the parser function object to operate
    //         (for thread-safety this should be immutable)

    typedef std::optional<node>(*parser_function)(
        node_ctx& ctx,
        const std::shared_ptr<parser_state>& state,
        const taul::token_seq& tkns,
        std::size_t offset,
        const std::shared_ptr<logger>& lgr);

    // TODO: in our *official* documentation, do be sure to remind the end-user to
    //       be careful about including grammar shared pointers in parser state,
    //       as the parsers themselves will be stored within grammars, so if we're
    //       not careful, this could create a strong reference cycle

    class parser_state {
    public:

        virtual ~parser_state() noexcept = default;
    };


    std::optional<node> default_parser_function(
        node_ctx&,
        const std::shared_ptr<parser_state>&,
        const taul::token_seq&,
        std::size_t,
        const std::shared_ptr<logger>&);


    namespace internal {


        parser_function get_parser_f(const parser& x) noexcept;
        
        parser_state* get_parser_state(const parser& x) noexcept;

        template<typename T>
        T* get_parser_state_as(const parser& x) noexcept {
            return (T*)get_parser_state(x);
        }
    }


    class parser final {
    public:

        friend parser_function internal::get_parser_f(const parser&) noexcept;
        friend parser_state* internal::get_parser_state(const parser& x) noexcept;


        // behaviour is undefined if f == nullptr

        parser(
            parser_function f, 
            const std::shared_ptr<parser_state>& state = nullptr);

        // default initialized parser function objects will be valid, but *trivial*, parser
        // function object which returns std::nullopt

        parser();

        parser(const parser& x);
        parser(parser&& x) noexcept;

        ~parser() noexcept = default;

        parser& operator=(const parser& rhs);
        parser& operator=(parser&& rhs) noexcept;


        std::optional<node> operator()(
            node_ctx& ctx,
            const taul::token_seq& tkns,
            const std::shared_ptr<logger>& lgr = nullptr) const;

        // the 'offset' below refers to where in tkns syntactic analysis is to begin

        // behaviour is undefined if offset > tkns.size() (notice that this allows
        // for the case where offset == tkns.size())

        std::optional<node> operator()(
            node_ctx& ctx,
            const taul::token_seq& tkns,
            std::size_t offset,
            const std::shared_ptr<logger>& lgr = nullptr) const;


    private:

        parser_function _f = nullptr;
        std::shared_ptr<parser_state> _state = nullptr;
    };
}

