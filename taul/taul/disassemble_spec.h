

#pragma once


#include "source_code.h"
#include "spec.h"
#include "internal/llspec.h"


namespace taul {


    // TODO: decide upon some unified aesthetic for our diagnostics


    // disassemble_spec returns a human-readable disassembly of spec

    std::string disassemble_spec(const spec& spec);
    std::string disassemble_spec(const spec& spec, const source_code& src);
}

namespace taul::internal {


    // just gonna put llspec disassembler here too

    std::string disassemble_llspec(const llspec& spec);
    std::string disassemble_llspec(const llspec& spec, const source_code& src);


    class disassemble_spec_interpreter final : public taul::spec_interpreter {
    public:
        std::string result{};
        size_t instruction = 0;

        const source_code* src = nullptr;


        std::string fmt_here() const;


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


    class disassemble_llspec_interpreter final : public llspec_interpreter {
    public:
        std::string result{};
        size_t instruction = 0;

        const source_code* src = nullptr;


        std::string fmt_here() const;


        disassemble_llspec_interpreter() = default;


        void on_startup() override final;
        void on_shutdown() override final;

        static_assert(llspec_opcodes == 22);

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
        void on_name(std::string_view name, preced_t preced_val) override final;
        void on_sequence() override final;
        void on_lookahead() override final;
        void on_lookahead_not() override final;
        void on_not() override final;
        void on_optional() override final;
        void on_kleene_star() override final;
        void on_kleene_plus() override final;

        void on_preced_pred(preced_t preced_max, preced_t preced_val) override final;
        void on_pylon() override final;
    };
}

