

#pragma once


#include <taul/all.h>


class test_spec_interpreter final : public taul::spec_interpreter {
public:

    std::string output;


    test_spec_interpreter() = default;
    test_spec_interpreter(const test_spec_interpreter&) = delete;
    test_spec_interpreter(test_spec_interpreter&&) noexcept = delete;

    ~test_spec_interpreter() noexcept = default;

    test_spec_interpreter& operator=(const test_spec_interpreter&) = delete;
    test_spec_interpreter& operator=(test_spec_interpreter&&) noexcept = delete;


protected:

    inline void on_startup() override final {
        output.clear();
        output += "startup\n";
    }

    inline void on_shutdown() override final {
        output += "shutdown\n";
    }

    static_assert(taul::spec_opcodes == 21);

    inline void on_close() override final {
        output += "close\n";
    }
    
    inline void on_alternative() override final {
        output += "alternative\n";
    }
    
    inline void on_right_assoc() override final {
        output += "right-assoc\n";
    }

    inline void on_lpr_decl(std::string_view name) override final {
        output += std::format("lpr-decl \"{}\"\n", name);
    }

    inline void on_ppr_decl(std::string_view name) override final {
        output += std::format("ppr-decl \"{}\"\n", name);
    }

    inline void on_lpr(std::string_view name, taul::qualifier qualifier) override final {
        output += std::format("lpr \"{}\" {}\n", name, qualifier);
    }

    inline void on_ppr(std::string_view name, taul::qualifier qualifier) override final {
        output += std::format("ppr \"{}\" {}\n", name, qualifier);
    }

    inline void on_end() override final {
        output += std::format("end\n");
    }

    inline void on_any() override final {
        output += std::format("any\n");
    }

    inline void on_string(std::string_view s) override final {
        output += std::format("string \"{}\"\n", s);
    }

    inline void on_charset(std::string_view s) override final {
        output += std::format("charset \"{}\"\n", s);
    }

    inline void on_token() override final {
        output += std::format("token\n");
    }

    inline void on_failure() override final {
        output += std::format("failure\n");
    }

    inline void on_name(std::string_view name) override final {
        output += std::format("name \"{}\"\n", name);
    }

    inline void on_sequence() override final {
        output += std::format("sequence\n");
    }

    inline void on_lookahead() override final {
        output += std::format("lookahead\n");
    }

    inline void on_lookahead_not() override final {
        output += std::format("lookahead-not\n");
    }

    inline void on_not() override final {
        output += std::format("not\n");
    }

    inline void on_optional() override final {
        output += std::format("optional\n");
    }

    inline void on_kleene_star() override final {
        output += std::format("kleene-star\n");
    }

    inline void on_kleene_plus() override final {
        output += std::format("kleene-plus\n");
    }
};

