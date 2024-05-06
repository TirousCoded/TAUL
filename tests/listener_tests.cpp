

#include <gtest/gtest.h>

#include <taul/spec.h>
#include <taul/listener.h>
#include <taul/load.h>


#include "helpers/test_listener.h"


using namespace taul::string_literals;


std::string build_expected_output(taul::grammar gram) {
    TAUL_ASSERT(gram.has_lpr("a"_str));
    TAUL_ASSERT(gram.has_lpr("b"_str));
    TAUL_ASSERT(gram.has_lpr("c"_str));
    TAUL_ASSERT(gram.has_ppr("A"_str));
    TAUL_ASSERT(gram.has_ppr("B"_str));
    test_listener lstnr{};
    lstnr.on_startup();
    lstnr.on_syntactic(gram.ppr("A"_str).value(), 0);
    lstnr.on_lexical(taul::token::normal(gram, "a"_str, 0, 1));
    lstnr.on_syntactic(gram.ppr("B"_str).value(), 1);
    lstnr.on_lexical(taul::token::normal(gram, "a"_str, 1, 1));
    lstnr.on_lexical(taul::token::normal(gram, "b"_str, 2, 1));
    lstnr.on_lexical(taul::token::normal(gram, "c"_str, 3, 1));
    lstnr.on_close();
    lstnr.on_lexical(taul::token::normal(gram, "c"_str, 4, 1));
    lstnr.on_lexical(taul::token::failure(5));
    lstnr.on_close();
    lstnr.on_abort();
    lstnr.on_shutdown();
    return lstnr.output;
}


TEST(ListenerTests, Playback) {
    auto lgr = taul::make_stderr_logger();
    auto spec =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("A"_str)
        .ppr_decl("B"_str)
        .lpr("a"_str)
        .close()
        .lpr("b"_str)
        .close()
        .lpr("c"_str)
        .close()
        .ppr("A"_str)
        .close()
        .ppr("B"_str)
        .close()
        .done();
    auto loaded = taul::load(spec, lgr);
    ASSERT_TRUE(loaded);
    taul::grammar gram = std::move(*loaded);
    ASSERT_TRUE(gram.has_lpr("a"_str));
    ASSERT_TRUE(gram.has_lpr("b"_str));
    ASSERT_TRUE(gram.has_lpr("c"_str));
    ASSERT_TRUE(gram.has_ppr("A"_str));
    ASSERT_TRUE(gram.has_ppr("B"_str));

    taul::parse_tree pt =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("A"_str).value(), 0)
        .lexical(gram.lpr("a"_str).value(), 0, 1)
        .syntactic(gram.ppr("B"_str).value(), 1)
        .lexical(gram.lpr("a"_str).value(), 1, 1)
        .lexical(gram.lpr("b"_str).value(), 2, 1)
        .lexical(gram.lpr("c"_str).value(), 3, 1)
        .close()
        .lexical(gram.lpr("c"_str).value(), 4, 1)
        .lexical(taul::token::failure(5))
        .close()
        .abort();

    ASSERT_TRUE(pt.is_sealed());
    ASSERT_TRUE(pt.is_aborted());

    std::string expected_output = build_expected_output(gram);

    test_listener lstnr{};
    lstnr.playback(pt);

    TAUL_LOG(lgr, 
        "\nexpected_output:\n{}\n\nlstnr.output:\n{}",
        expected_output, lstnr.output);

    EXPECT_EQ(expected_output, lstnr.output);
}

