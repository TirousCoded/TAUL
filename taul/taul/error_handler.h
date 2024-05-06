

#pragma once


#include "api_component.h"
#include "symbol_range.h"
#include "symbols.h"


namespace taul {


    class base_parser;


    class error_handler : public api_component {
    public:

        error_handler(std::shared_ptr<logger> lgr = nullptr);

        virtual ~error_handler() noexcept = default;


        // startup and shutdown are used to specify the beginning,
        // and ending, of a round of parsing, during which syntax errors
        // may arise which the error handler is responsible for attempting
        // to recover from

        // the impl should expect client != nullptr, and should presume
        // that client == nullptr means the parser impl is defective

        // behaviour is undefined if these methods are used outside of the
        // context of a parser impl

        virtual void startup(base_parser* client) = 0;
        virtual void shutdown() = 0;


        // terminal_error and nonterminal_error report syntax errors to
        // the error handler, requesting that the error handler attempts
        // to recover from the error

        // these methods employ the eh_*** methods of the client parser in
        // order to try and recover from the error

        // when these return, the parser will attempt the operation that
        // arose the error, and if it succeeds, the parser will continue on,
        // but if it fails even after attempted recovery, the parser will
        // abort execution

        // behaviour is undefined if these methods are used outside of the
        // context of a parser impl

        virtual void terminal_error(token_range ids, token input) = 0;
        virtual void nonterminal_error(symbol_id id, token input) = 0;
    };
}

