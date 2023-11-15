

#include <gtest/gtest.h>

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

    static_assert(taul::spec_opcodes == 7);

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

    inline void on_lpr(std::string_view name) override final {
        output += std::format("lpr \"{}\"\n", name);
    }

    inline void on_ppr(std::string_view name) override final {
        output += std::format("ppr \"{}\"\n", name);
    }

    inline void on_char() override final {
        output += std::format("char\n");
    }
};


// we'll test spec writers/interpreters as a single unit, as neither can 
// be used in isolation

// likewise, our code is such that I feel it's fine if we stuff everything
// in a single fat unit test, as there's not too much, as it all fits
// together quite naturally


TEST(spec_tests, tests) {

    const auto lgr = taul::make_stderr_logger();

    EXPECT_TRUE(lgr);


    taul::spec_writer sw{};


    // test done w/ default ctor state

    const auto spec0 = sw
        .done();

    ASSERT_TRUE(spec0.empty());


    // test done w/ main usage

    static_assert(taul::spec_opcodes == 7);

    const auto spec1 = sw
        .grammar_bias(taul::bias::last_shortest)
        .lpr_decl("lpr0")
        .ppr_decl("ppr0")
        .lpr("lpr0")
        .char0()
        .close()
        .ppr("ppr0")
        .close()
        .done();

    ASSERT_FALSE(spec1.empty());


    // test done w/ post-main usage state

    const auto spec2 = sw
        .done();

    ASSERT_TRUE(spec2.empty());


    test_spec_interpreter tsi{};

    std::string expected{};


    // test interpret w/ spec0

    tsi.interpret(spec0);

    expected = "startup\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);


    // test interpret w/ spec1

    tsi.interpret(spec1);

    expected = "startup\n";
    expected += "grammar-bias last-shortest\n";
    expected += "lpr-decl \"lpr0\"\n";
    expected += "ppr-decl \"ppr0\"\n";
    expected += "lpr \"lpr0\"\n";
    expected += "char\n";
    expected += "close\n";
    expected += "ppr \"ppr0\"\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);


    // test interpret w/ spec2

    tsi.interpret(spec2);

    expected = "startup\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

