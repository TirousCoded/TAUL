

#include <gtest/gtest.h>

#include <taul/all.h>


#include "helpers/test_spec_interpreter.h"


TEST(SpecTests, Association) {

    auto src0 = std::make_shared<taul::source_code>();
    auto src1 = std::make_shared<taul::source_code>();


    taul::spec spec0{};


    EXPECT_TRUE(spec0.associated(nullptr));
    EXPECT_FALSE(spec0.associated(src0));
    EXPECT_FALSE(spec0.associated(src1));


    spec0.associate(src0);


    EXPECT_FALSE(spec0.associated(nullptr));
    EXPECT_TRUE(spec0.associated(src0));
    EXPECT_FALSE(spec0.associated(src1));


    spec0.associate(src1);


    EXPECT_FALSE(spec0.associated(nullptr));
    EXPECT_FALSE(spec0.associated(src0));
    EXPECT_TRUE(spec0.associated(src1));


    spec0.associate(nullptr);


    EXPECT_TRUE(spec0.associated(nullptr));
    EXPECT_FALSE(spec0.associated(src0));
    EXPECT_FALSE(spec0.associated(src1));
}


// these tests are for spec writing/interpreting, w/ the semantics of
// loading being beyond its scope

// we'll test spec writers/interpreters as a single unit, as neither can 
// be used in isolation


// I think it's fine if we stuff testing the *core* behaviour of specs
// into a single fat unit test

TEST(SpecTests, Core) {

    const auto lgr = taul::make_stderr_logger();

    EXPECT_TRUE(lgr);


    taul::spec_writer sw{};


    // test done w/ default ctor state

    const auto spec0 = sw
        .done();

    ASSERT_TRUE(spec0.bin.empty());


    // test done w/ main usage

    static_assert(taul::spec_opcodes == 22);

    const auto spec1 = sw
        .grammar_bias(taul::bias::ls)
        .lpr_decl("test_none_qualifier")
        .lpr_decl("test_skip_qualifier")
        .lpr_decl("test_support_qualifier")
        .lpr("test_none_qualifier", taul::qualifier::none)
        .close()
        .lpr("test_skip_qualifier", taul::qualifier::skip)
        .close()
        .lpr("test_support_qualifier", taul::qualifier::support)
        .close()
        .lpr_decl("lpr0")
        .ppr_decl("ppr0")
        .lpr("lpr0")
        .begin()
        .end()
        .any()
        .string("abc")
        .charset("abc")
        .range('A', 'Z')
        .sequence()
        .set(taul::bias::ls)
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
        .ppr("ppr0", taul::qualifier::skip) // <- semantically illegal, but can still appear in syntax
        .begin()
        .end()
        .any()
        .string("abc")
        .token()
        .failure()
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

    ASSERT_FALSE(spec1.bin.empty());


    // test done w/ post-main usage state

    const auto spec2 = sw
        .done();

    ASSERT_TRUE(spec2.bin.empty());


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
    expected += "lpr-decl \"test_support_qualifier\"\n";
    expected += "lpr \"test_none_qualifier\" none\n";
    expected += "close\n";
    expected += "lpr \"test_skip_qualifier\" skip\n";
    expected += "close\n";
    expected += "lpr \"test_support_qualifier\" support\n";
    expected += "close\n";
    expected += "lpr-decl \"lpr0\"\n";
    expected += "ppr-decl \"ppr0\"\n";
    expected += "lpr \"lpr0\" none\n";
    expected += "begin\n";
    expected += "end\n";
    expected += "any\n";
    expected += "string \"abc\"\n";
    expected += "charset \"abc\"\n";
    expected += "range \'A\' \'Z\'\n";
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
    expected += "ppr \"ppr0\" skip\n";
    expected += "begin\n";
    expected += "end\n";
    expected += "any\n";
    expected += "string \"abc\"\n";
    expected += "token\n";
    expected += "failure\n";
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


TEST(SpecTests, Concat) {

    const auto lgr = taul::make_stderr_logger();

    EXPECT_TRUE(lgr);


    taul::spec_writer sw{};


    // the two specs we're to concatenate

    auto spec0 = sw
        .lpr_decl("ABC")
        .ppr_decl("Abc")
        .lpr("ABC")
        .string("abc")
        .close()
        .ppr("Abc")
        .name("ABC")
        .close()
        .done();

    auto spec1 = sw
        .lpr_decl("DEF")
        .ppr_decl("Def")
        .lpr("DEF")
        .string("def")
        .close()
        .ppr("Def")
        .name("DEF")
        .close()
        .done();

    ASSERT_FALSE(spec0.bin.empty());
    ASSERT_FALSE(spec1.bin.empty());


    // imbue spec0 and spec1 w/ source_code associations to test how
    // they are affected by concat

    const auto src0 = std::make_shared<taul::source_code>();
    const auto src1 = std::make_shared<taul::source_code>();

    spec0.associate(src0);
    spec1.associate(src1);

    ASSERT_TRUE(spec0.associated(src0));
    ASSERT_TRUE(spec1.associated(src1));


    // our concatenated spec

    const auto spec2 = taul::spec::concat(spec0, spec1);

    ASSERT_FALSE(spec2.bin.empty());


    // assert expected source_code association

    EXPECT_TRUE(spec2.associated(src0));


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(spec2);

    expected = "startup\n";
    expected += "lpr-decl \"ABC\"\n";
    expected += "ppr-decl \"Abc\"\n";
    expected += "lpr \"ABC\" none\n";
    expected += "string \"abc\"\n";
    expected += "close\n";
    expected += "ppr \"Abc\" none\n";
    expected += "name \"ABC\"\n";
    expected += "close\n";
    expected += "lpr-decl \"DEF\"\n";
    expected += "ppr-decl \"Def\"\n";
    expected += "lpr \"DEF\" none\n";
    expected += "string \"def\"\n";
    expected += "close\n";
    expected += "ppr \"Def\" none\n";
    expected += "name \"DEF\"\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

