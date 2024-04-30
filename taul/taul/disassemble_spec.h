

#pragma once


#include "spec.h"


namespace taul {


    // TODO: decide upon some unified aesthetic for our diagnostics


    // disassemble_spec returns a human-readable disassembly of spec

    std::string disassemble_spec(const spec& spec);
}

namespace taul::internal {


    class disassemble_spec_interpreter final : public taul::spec_interpreter {
    public:
        std::string result{};
        size_t instruction = 0;


        disassemble_spec_interpreter() = default;


        void on_startup() override final;
        void on_shutdown() override final;

        static_assert(spec_opcodes == 20);

        void on_close() override final;
        void on_alternative() override final;
        void on_lpr_decl(std::string_view name) override final;
        void on_ppr_decl(std::string_view name) override final;
        void on_lpr(std::string_view name, qualifier qualifier) override final;
        void on_ppr(std::string_view name, qualifier qualifier) override final;

        void on_end() override final;
        void on_any() override final;
        void on_string(std::string_view s) override final;
        void on_charset(std::string_view s) override final;
        void on_token() override final;
        void on_failure() override final;
        void on_name(std::string_view name) override final;
        void on_sequence() override final;
        void on_lookahead() override final;
        void on_lookahead_not() override final;
        void on_not() override final;
        void on_optional() override final;
        void on_kleene_star() override final;
        void on_kleene_plus() override final;
    };
}

