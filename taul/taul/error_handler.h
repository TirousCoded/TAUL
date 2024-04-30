

#pragma once


#include "api_component.h"
#include "symbol_range.h"
#include "symbols.h"


namespace taul {


    class parser;


    class error_handler : public api_component {
    public:

        error_handler(std::shared_ptr<logger> lgr = nullptr);

        virtual ~error_handler() noexcept = default;


        // begin_session and end_session are used to specify the beginning,
        // and ending, of a round of parsing, during which syntax errors
        // may arise which the error handler is responsible for attempting
        // to recover from

        // behaviour is undefined if these methods are used outside of the
        // context of a parser impl

        virtual void begin_session(parser& client) = 0;
        virtual void end_session() = 0;


        // report_terminal_error and report_nonterminal_error report syntax
        // errors to the error handler, requesting that the error handler
        // attempts to recover from the error

        // these methods employ the eh_*** methods of the client parser in
        // order to try and recover from the error

        // these methods return if the error handler was able to recover 
        // from the error

        // behaviour is undefined if these methods are used outside of the
        // context of a parser impl

        virtual bool report_terminal_error(token_range ids, token input) = 0;
        virtual bool report_nonterminal_error(symbol_id id, token input) = 0;
    };
}

