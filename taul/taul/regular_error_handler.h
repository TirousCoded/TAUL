

#pragma once


#include "error_handler.h"


namespace taul {


    // the 'regular' error handler keeps advancing the upstream token stream
    // until a token which resolves the error is found, or until it reaches
    // the end-of-input


    class regular_error_handler final : public error_handler {
    public:

        regular_error_handler(std::shared_ptr<logger> lgr = nullptr);


        void startup(base_parser* client) override final;
        void shutdown() override final;
        void terminal_error(token_range ids, token input) override final;
        void nonterminal_error(symbol_id id, token input) override final;


    private:

        base_parser* _client = nullptr;
    };
}

