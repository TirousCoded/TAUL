

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

    static_assert(taul::spec_opcodes == 19);

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

    inline void on_ppr(std::string_view name) override final {
        output += std::format("ppr \"{}\"\n", name);
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


// these tests are for spec writing/interpreting, w/ the semantics of
// loading being beyond its scope

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

    static_assert(taul::spec_opcodes == 19);

    const auto spec1 = sw
        .grammar_bias(taul::bias::last_shortest)
        .lpr_decl("test_none_qualifier")
        .lpr_decl("test_skip_qualifier")
        .lpr_decl("test_exclude_qualifier")
        .lpr("test_none_qualifier", taul::qualifier::none)
        .close()
        .lpr("test_skip_qualifier", taul::qualifier::skip)
        .close()
        .lpr("test_exclude_qualifier", taul::qualifier::exclude)
        .close()
        .lpr_decl("lpr0")
        .ppr_decl("ppr0")
        .lpr("lpr0")
        .begin()
        .end()
        .any()
        .string("abc")
        .charset("abc")
        .sequence()
        .set(taul::bias::last_shortest)
        .any()
        .name("lpr0")
        .close()
        .close()
        .assertion(taul::polarity::negative)
        .modifier(3, 4)
        .any()
        .close()
        .close()
        .constraint(taul::polarity::negative)
        .any()
        .junction()
        .sequence()
        .string("a")
        .localend()
        .string("bc")
        .close()
        .close()
        .close()
        .ppr("ppr0")
        .begin()
        .end()
        .any()
        .string("abc")
        .charset("abc")
        .sequence()
        .set()
        .any()
        .name("lpr0")
        .name("ppr0")
        .close()
        .close()
        .assertion()
        .modifier(3, 0)
        .any()
        .close()
        .close()
        .constraint()
        .any()
        .junction()
        .sequence()
        .string("a")
        .localend()
        .string("bc")
        .close()
        .close()
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
    expected += "lpr-decl \"test_none_qualifier\"\n";
    expected += "lpr-decl \"test_skip_qualifier\"\n";
    expected += "lpr-decl \"test_exclude_qualifier\"\n";
    expected += "lpr \"test_none_qualifier\" none\n";
    expected += "close\n";
    expected += "lpr \"test_skip_qualifier\" skip\n";
    expected += "close\n";
    expected += "lpr \"test_exclude_qualifier\" exclude\n";
    expected += "close\n";
    expected += "lpr-decl \"lpr0\"\n";
    expected += "ppr-decl \"ppr0\"\n";
    expected += "lpr \"lpr0\" none\n";
    expected += "begin\n";
    expected += "end\n";
    expected += "any\n";
    expected += "string \"abc\"\n";
    expected += "charset \"abc\"\n";
    expected += "sequence\n";
    expected += "set last-shortest\n";
    expected += "any\n";
    expected += "name \"lpr0\"\n";
    expected += "close\n";
    expected += "close\n";
    expected += "assertion negative\n";
    expected += "modifier 3, 4\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "constraint negative\n";
    expected += "any\n";
    expected += "junction\n";
    expected += "sequence\n";
    expected += "string \"a\"\n";
    expected += "localend\n";
    expected += "string \"bc\"\n";
    expected += "close\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"ppr0\"\n";
    expected += "begin\n";
    expected += "end\n";
    expected += "any\n";
    expected += "string \"abc\"\n";
    expected += "charset \"abc\"\n";
    expected += "sequence\n";
    expected += "set first-longest\n";
    expected += "any\n";
    expected += "name \"lpr0\"\n";
    expected += "name \"ppr0\"\n";
    expected += "close\n";
    expected += "close\n";
    expected += "assertion positive\n";
    expected += "modifier 3, 0\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "constraint positive\n";
    expected += "any\n";
    expected += "junction\n";
    expected += "sequence\n";
    expected += "string \"a\"\n";
    expected += "localend\n";
    expected += "string \"bc\"\n";
    expected += "close\n";
    expected += "close\n";
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

