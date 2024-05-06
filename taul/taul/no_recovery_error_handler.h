

#pragma once


#include "error_handler.h"


namespace taul {


    class no_recovery_error_handler final : public error_handler {
    public:

        no_recovery_error_handler(std::shared_ptr<logger> lgr = nullptr);


        void startup(base_parser*) override final;
        void shutdown() override final;
        void terminal_error(token_range, token) override final;
        void nonterminal_error(symbol_id, token) override final;
    };
}

