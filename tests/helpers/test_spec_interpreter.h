

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

    static_assert(taul::spec_opcodes == 22);

    inline void on_grammar_bias(taul::bias b) override final {
        output += std::format("grammar-bias {}\n", b);
    }

    inline void on_close() override final {
        output += "close\n";
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

    inline void on_begin() override final {
        output += std::format("begin\n");
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

    inline void on_range(char a, char b) override final {
        output += std::format("range \'{}\' \'{}\'\n", a, b);
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

    inline void on_set(taul::bias b) override final {
        output += std::format("set {}\n", b);
    }

    inline void on_modifier(std::uint16_t min, std::uint16_t max) override final {
        output += std::format("modifier {}, {}\n", min, max);
    }

    inline void on_assertion(taul::polarity p) override final {
        output += std::format("assertion {}\n", p);
    }

    inline void on_constraint(taul::polarity p) override final {
        output += std::format("constraint {}\n", p);
    }

    inline void on_junction() override final {
        output += std::format("junction\n");
    }

    inline void on_localend() override final {
        output += std::format("localend\n");
    }
};

