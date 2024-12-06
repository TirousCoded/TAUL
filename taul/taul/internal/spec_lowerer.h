

#pragma once


#include "../spec.h"

#include "llspec.h"


namespace taul::internal {


    class spec_lowerer final {
    public:

        spec_lowerer() = default;


        llspec_writer output;


        void on_startup();
        void on_shutdown();

        static_assert(spec_opcodes == 21);

        void on_pos(source_pos new_pos);
        void on_close();
        void on_alternative();
        void on_lpr_decl(std::string_view name);
        void on_ppr_decl(std::string_view name);
        void on_lpr(std::string_view name, qualifier qualifier);
        void on_ppr(std::string_view name, qualifier qualifier);

        void on_end();
        void on_any();
        void on_string(std::string_view s);
        void on_charset(std::string_view s);
        void on_token();
        void on_failure();
        void on_name(std::string_view name);
        void on_sequence();
        void on_lookahead();
        void on_lookahead_not();
        void on_not();
        void on_optional();
        void on_kleene_star();
        void on_kleene_plus();
    };
}

