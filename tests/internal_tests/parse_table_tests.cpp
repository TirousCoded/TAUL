

#include <gtest/gtest.h>

#include <taul/internal/parse_table.h>


namespace ns = taul::internal;


// these internal tests are not as comprehensive as frontend tests, as here 
// I'm just interested in testing the core LL(1) parse table generation

// these tests don't test the state of the first_sets_A, follow_sets_A, or
// prefix_sets_A fields of the parse table

// these tests are written such that they expect ID grouper behaviour to work,
// w/ it working being somewhat tested, but largely assumed, and w/ the tests
// being written such that the exact behaviour of the ID grouper is transparent


TEST(ParseTableTests, Glyph_EmptyNonTerminals) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0))
        .add_rule(taul::lpr_id(1))
        .add_rule(taul::lpr_id(2))
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    const auto a = table.lookup(taul::lpr_id(0), table.grouper(taul::end_cp_id));
    const auto b = table.lookup(taul::lpr_id(1), table.grouper(taul::end_cp_id));
    const auto c = table.lookup(taul::lpr_id(2), table.grouper(taul::end_cp_id));

    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_TRUE(c);

    if (a) EXPECT_EQ(a.value(), 0);
    if (b) EXPECT_EQ(b.value(), 1);
    if (c) EXPECT_EQ(c.value(), 2);
}

TEST(ParseTableTests, Glyph_NonEmptyNonTerminals) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0)) // starts w/ terminal
        .add_terminal(0, U'a', U'c')
        .add_nonterminal(0, taul::lpr_id(0))
        .add_terminal(0, U'A', U'C')
        .add_rule(taul::lpr_id(1)) // starts w/ (assertion-marked) terminal
        .add_terminal(1, U'd', U'f', true)
        .add_terminal(1, U'C', U'E')
        .add_nonterminal(1, taul::lpr_id(2))
        .add_rule(taul::lpr_id(2)) // starts w/ non-terminal
        .add_nonterminal(2, taul::lpr_id(0))
        .add_terminal(2, U'A', U'C')
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    EXPECT_FALSE(table.lookup(taul::lpr_id(0), table.grouper(taul::end_cp_id)));
    EXPECT_FALSE(table.lookup(taul::lpr_id(1), table.grouper(taul::end_cp_id)));
    EXPECT_FALSE(table.lookup(taul::lpr_id(2), table.grouper(taul::end_cp_id)));

    const auto a0 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'a')));
    const auto a1 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'b')));
    const auto a2 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'c')));
    
    const auto b0 = table.lookup(taul::lpr_id(1), table.grouper(taul::cp_id(U'd')));
    const auto b1 = table.lookup(taul::lpr_id(1), table.grouper(taul::cp_id(U'e')));
    const auto b2 = table.lookup(taul::lpr_id(1), table.grouper(taul::cp_id(U'f')));

    const auto c0 = table.lookup(taul::lpr_id(2), table.grouper(taul::cp_id(U'a')));
    const auto c1 = table.lookup(taul::lpr_id(2), table.grouper(taul::cp_id(U'b')));
    const auto c2 = table.lookup(taul::lpr_id(2), table.grouper(taul::cp_id(U'c')));

    EXPECT_TRUE(a0);
    EXPECT_TRUE(a1);
    EXPECT_TRUE(a2);
    
    EXPECT_TRUE(b0);
    EXPECT_TRUE(b1);
    EXPECT_TRUE(b2);
    
    EXPECT_TRUE(c0);
    EXPECT_TRUE(c1);
    EXPECT_TRUE(c2);

    if (a0) EXPECT_EQ(a0.value(), 0);
    if (a1) EXPECT_EQ(a1.value(), 0);
    if (a2) EXPECT_EQ(a2.value(), 0);

    if (b0) EXPECT_EQ(b0.value(), 1);
    if (b1) EXPECT_EQ(b1.value(), 1);
    if (b2) EXPECT_EQ(b2.value(), 1);

    if (c0) EXPECT_EQ(c0.value(), 2);
    if (c1) EXPECT_EQ(c1.value(), 2);
    if (c2) EXPECT_EQ(c2.value(), 2);
}

TEST(ParseTableTests, Glyph_Alternation) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0)) // starts w/ terminal
        .add_terminal(0, U'a', U'c')
        .add_rule(taul::lpr_id(0)) // starts w/ (assertion-marked) terminal
        .add_terminal(1, U'd', U'f', true)
        .add_rule(taul::lpr_id(0)) // starts w/ non-terminal
        .add_nonterminal(2, taul::lpr_id(1))
        .add_rule(taul::lpr_id(1)) // helper
        .add_terminal(3, U'A', U'C')
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    EXPECT_FALSE(table.lookup(taul::lpr_id(0), table.grouper(taul::end_cp_id)));
    EXPECT_FALSE(table.lookup(taul::lpr_id(1), table.grouper(taul::end_cp_id)));

    const auto a0 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'a')));
    const auto a1 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'b')));
    const auto a2 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'c')));
    
    const auto a3 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'd')));
    const auto a4 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'e')));
    const auto a5 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'f')));

    const auto a6 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'A')));
    const auto a7 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'B')));
    const auto a8 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'C')));
    
    const auto b0 = table.lookup(taul::lpr_id(1), table.grouper(taul::cp_id(U'A')));
    const auto b1 = table.lookup(taul::lpr_id(1), table.grouper(taul::cp_id(U'B')));
    const auto b2 = table.lookup(taul::lpr_id(1), table.grouper(taul::cp_id(U'C')));

    EXPECT_TRUE(a0);
    EXPECT_TRUE(a1);
    EXPECT_TRUE(a2);
    
    EXPECT_TRUE(a3);
    EXPECT_TRUE(a4);
    EXPECT_TRUE(a5);
    
    EXPECT_TRUE(a6);
    EXPECT_TRUE(a7);
    EXPECT_TRUE(a8);

    EXPECT_TRUE(b0);
    EXPECT_TRUE(b1);
    EXPECT_TRUE(b2);

    if (a0) EXPECT_EQ(a0.value(), 0);
    if (a1) EXPECT_EQ(a1.value(), 0);
    if (a2) EXPECT_EQ(a2.value(), 0);
    
    if (a3) EXPECT_EQ(a3.value(), 1);
    if (a4) EXPECT_EQ(a4.value(), 1);
    if (a5) EXPECT_EQ(a5.value(), 1);
    
    if (a6) EXPECT_EQ(a6.value(), 2);
    if (a7) EXPECT_EQ(a7.value(), 2);
    if (a8) EXPECT_EQ(a8.value(), 2);

    if (b0) EXPECT_EQ(b0.value(), 3);
    if (b1) EXPECT_EQ(b1.value(), 3);
    if (b2) EXPECT_EQ(b2.value(), 3);
}

TEST(ParseTableTests, Glyph_WithEmptyAlternative) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0))
        .add_terminal(0, U'a', U'c')
        .add_rule(taul::lpr_id(0))
        .add_terminal(1, U'd', U'f', true)
        .add_rule(taul::lpr_id(0)) // the empty alternative
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    const auto a0 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'a')));
    const auto a1 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'b')));
    const auto a2 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'c')));

    const auto a3 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'd')));
    const auto a4 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'e')));
    const auto a5 = table.lookup(taul::lpr_id(0), table.grouper(taul::cp_id(U'f')));

    const auto a6 = table.lookup(taul::lpr_id(0), table.grouper(taul::end_cp_id));

    EXPECT_TRUE(a0);
    EXPECT_TRUE(a1);
    EXPECT_TRUE(a2);

    EXPECT_TRUE(a3);
    EXPECT_TRUE(a4);
    EXPECT_TRUE(a5);

    EXPECT_TRUE(a6);

    if (a0) EXPECT_EQ(a0.value(), 0);
    if (a1) EXPECT_EQ(a1.value(), 0);
    if (a2) EXPECT_EQ(a2.value(), 0);

    if (a3) EXPECT_EQ(a3.value(), 1);
    if (a4) EXPECT_EQ(a4.value(), 1);
    if (a5) EXPECT_EQ(a5.value(), 1);

    if (a6) EXPECT_EQ(a6.value(), 2);
}

TEST(ParseTableTests, Glyph_Ambiguity) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0))
        .add_terminal(0, U'a', U'd')
        .add_rule(taul::lpr_id(0))
        .add_terminal(1, U'b', U'f', true)
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_FALSE(details.collisions.empty()); // not gonna bother asserting *what* collision details should be
}

TEST(ParseTableTests, Glyph_Ambiguity_NoAmbiguityIfNonTerminalIDsDiffer_A) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0))
        .add_terminal(0, U'a', U'd')
        .add_rule(taul::lpr_id(1))
        .add_terminal(1, U'b', U'f', true)
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);

    EXPECT_TRUE(details.collisions.empty()); // no ambiguity should arise

    // don't much care to test *what* is built, as other tests handle that
}

TEST(ParseTableTests, Glyph_Ambiguity_NoAmbiguityIfNonTerminalIDsDiffer_B) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0)) // empty
        .add_rule(taul::lpr_id(1)) // also empty
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);

    EXPECT_TRUE(details.collisions.empty()); // no ambiguity should arise

    // don't much care to test *what* is built, as other tests handle that
}

TEST(ParseTableTests, Glyph_LeftRecursion) {
    ns::parse_table_build_details<taul::glyph> details{};
    const ns::parse_table<taul::glyph> table =
        ns::parse_table<taul::glyph>()
        .add_rule(taul::lpr_id(0))
        .add_nonterminal(0, taul::lpr_id(1))
        .add_terminal(0, U'd', U'f', true)
        .add_rule(taul::lpr_id(0))
        .add_terminal(1, U'a', U'c', true)
        .add_rule(taul::lpr_id(1))
        .add_nonterminal(2, taul::lpr_id(0))
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_FALSE(details.collisions.empty()); // not gonna bother asserting *what* collision details should be
}

TEST(ParseTableTests, Token_EmptyNonTerminals) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0))
        .add_rule(taul::ppr_id(1))
        .add_rule(taul::ppr_id(2))
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    const auto a = table.lookup(taul::ppr_id(0), table.grouper(taul::end_lpr_id));
    const auto b = table.lookup(taul::ppr_id(1), table.grouper(taul::end_lpr_id));
    const auto c = table.lookup(taul::ppr_id(2), table.grouper(taul::end_lpr_id));

    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_TRUE(c);

    if (a) EXPECT_EQ(a.value(), 0);
    if (b) EXPECT_EQ(b.value(), 1);
    if (c) EXPECT_EQ(c.value(), 2);
}

TEST(ParseTableTests, Token_NonEmptyNonTerminals) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0)) // starts w/ terminal
        .add_terminal(0, 0, 2)
        .add_nonterminal(0, taul::ppr_id(0))
        .add_terminal(0, 10, 12)
        .add_rule(taul::ppr_id(1)) // starts w/ (assertion-marked) terminal
        .add_terminal(1, 3, 5, true)
        .add_terminal(1, 12, 14)
        .add_nonterminal(1, taul::ppr_id(2))
        .add_rule(taul::ppr_id(2)) // starts w/ non-terminal
        .add_nonterminal(2, taul::ppr_id(0))
        .add_terminal(2, 10, 12)
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    EXPECT_FALSE(table.lookup(taul::ppr_id(0), table.grouper(taul::end_lpr_id)));
    EXPECT_FALSE(table.lookup(taul::ppr_id(1), table.grouper(taul::end_lpr_id)));
    EXPECT_FALSE(table.lookup(taul::ppr_id(2), table.grouper(taul::end_lpr_id)));

    const auto a0 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(0)));
    const auto a1 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(1)));
    const auto a2 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(2)));

    const auto b0 = table.lookup(taul::ppr_id(1), table.grouper(taul::lpr_id(3)));
    const auto b1 = table.lookup(taul::ppr_id(1), table.grouper(taul::lpr_id(4)));
    const auto b2 = table.lookup(taul::ppr_id(1), table.grouper(taul::lpr_id(5)));

    const auto c0 = table.lookup(taul::ppr_id(2), table.grouper(taul::lpr_id(0)));
    const auto c1 = table.lookup(taul::ppr_id(2), table.grouper(taul::lpr_id(1)));
    const auto c2 = table.lookup(taul::ppr_id(2), table.grouper(taul::lpr_id(2)));

    EXPECT_TRUE(a0);
    EXPECT_TRUE(a1);
    EXPECT_TRUE(a2);

    EXPECT_TRUE(b0);
    EXPECT_TRUE(b1);
    EXPECT_TRUE(b2);

    EXPECT_TRUE(c0);
    EXPECT_TRUE(c1);
    EXPECT_TRUE(c2);

    if (a0) EXPECT_EQ(a0.value(), 0);
    if (a1) EXPECT_EQ(a1.value(), 0);
    if (a2) EXPECT_EQ(a2.value(), 0);

    if (b0) EXPECT_EQ(b0.value(), 1);
    if (b1) EXPECT_EQ(b1.value(), 1);
    if (b2) EXPECT_EQ(b2.value(), 1);

    if (c0) EXPECT_EQ(c0.value(), 2);
    if (c1) EXPECT_EQ(c1.value(), 2);
    if (c2) EXPECT_EQ(c2.value(), 2);
}

TEST(ParseTableTests, Token_Alternation) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0)) // starts w/ terminal
        .add_terminal(0, 0, 2)
        .add_rule(taul::ppr_id(0)) // starts w/ (assertion-marked) terminal
        .add_terminal(1, 3, 5, true)
        .add_rule(taul::ppr_id(0)) // starts w/ non-terminal
        .add_nonterminal(2, taul::ppr_id(1))
        .add_rule(taul::ppr_id(1)) // helper
        .add_terminal(3, 10, 12)
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    EXPECT_FALSE(table.lookup(taul::ppr_id(0), table.grouper(taul::end_lpr_id)));
    EXPECT_FALSE(table.lookup(taul::ppr_id(1), table.grouper(taul::end_lpr_id)));

    const auto a0 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(0)));
    const auto a1 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(1)));
    const auto a2 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(2)));

    const auto a3 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(3)));
    const auto a4 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(4)));
    const auto a5 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(5)));

    const auto a6 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(10)));
    const auto a7 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(11)));
    const auto a8 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(12)));

    const auto b0 = table.lookup(taul::ppr_id(1), table.grouper(taul::lpr_id(10)));
    const auto b1 = table.lookup(taul::ppr_id(1), table.grouper(taul::lpr_id(11)));
    const auto b2 = table.lookup(taul::ppr_id(1), table.grouper(taul::lpr_id(12)));

    EXPECT_TRUE(a0);
    EXPECT_TRUE(a1);
    EXPECT_TRUE(a2);

    EXPECT_TRUE(a3);
    EXPECT_TRUE(a4);
    EXPECT_TRUE(a5);

    EXPECT_TRUE(a6);
    EXPECT_TRUE(a7);
    EXPECT_TRUE(a8);

    EXPECT_TRUE(b0);
    EXPECT_TRUE(b1);
    EXPECT_TRUE(b2);

    if (a0) EXPECT_EQ(a0.value(), 0);
    if (a1) EXPECT_EQ(a1.value(), 0);
    if (a2) EXPECT_EQ(a2.value(), 0);

    if (a3) EXPECT_EQ(a3.value(), 1);
    if (a4) EXPECT_EQ(a4.value(), 1);
    if (a5) EXPECT_EQ(a5.value(), 1);

    if (a6) EXPECT_EQ(a6.value(), 2);
    if (a7) EXPECT_EQ(a7.value(), 2);
    if (a8) EXPECT_EQ(a8.value(), 2);

    if (b0) EXPECT_EQ(b0.value(), 3);
    if (b1) EXPECT_EQ(b1.value(), 3);
    if (b2) EXPECT_EQ(b2.value(), 3);
}

TEST(ParseTableTests, Token_WithEmptyAlternative) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0))
        .add_terminal(0, 0, 2)
        .add_rule(taul::ppr_id(0))
        .add_terminal(1, 3, 5, true)
        .add_rule(taul::ppr_id(0)) // the empty alternative
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_TRUE(details.collisions.empty());

    const auto a0 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(0)));
    const auto a1 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(1)));
    const auto a2 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(2)));

    const auto a3 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(3)));
    const auto a4 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(4)));
    const auto a5 = table.lookup(taul::ppr_id(0), table.grouper(taul::lpr_id(5)));

    const auto a6 = table.lookup(taul::ppr_id(0), table.grouper(taul::end_lpr_id));

    EXPECT_TRUE(a0);
    EXPECT_TRUE(a1);
    EXPECT_TRUE(a2);

    EXPECT_TRUE(a3);
    EXPECT_TRUE(a4);
    EXPECT_TRUE(a5);

    EXPECT_TRUE(a6);

    if (a0) EXPECT_EQ(a0.value(), 0);
    if (a1) EXPECT_EQ(a1.value(), 0);
    if (a2) EXPECT_EQ(a2.value(), 0);

    if (a3) EXPECT_EQ(a3.value(), 1);
    if (a4) EXPECT_EQ(a4.value(), 1);
    if (a5) EXPECT_EQ(a5.value(), 1);

    if (a6) EXPECT_EQ(a6.value(), 2);
}

TEST(ParseTableTests, Token_Ambiguity) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0))
        .add_terminal(0, 0, 4)
        .add_rule(taul::ppr_id(0))
        .add_terminal(1, 1, 5, true)
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_FALSE(details.collisions.empty()); // not gonna bother asserting *what* collision details should be
}

TEST(ParseTableTests, Token_Ambiguity_NoAmbiguityIfNonTerminalIDsDiffer_A) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0))
        .add_terminal(0, 0, 4)
        .add_rule(taul::ppr_id(1))
        .add_terminal(1, 1, 5, true)
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);

    EXPECT_TRUE(details.collisions.empty()); // no ambiguity should arise

    // don't much care to test *what* is built, as other tests handle that
}

TEST(ParseTableTests, Token_Ambiguity_NoAmbiguityIfNonTerminalIDsDiffer_B) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0)) // empty
        .add_rule(taul::ppr_id(1)) // also empty
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);

    EXPECT_TRUE(details.collisions.empty()); // no ambiguity should arise

    // don't much care to test *what* is built, as other tests handle that
}

TEST(ParseTableTests, Token_LeftRecursion) {
    ns::parse_table_build_details<taul::token> details{};
    const ns::parse_table<taul::token> table =
        ns::parse_table<taul::token>()
        .add_rule(taul::ppr_id(0))
        .add_nonterminal(0, taul::ppr_id(1))
        .add_terminal(0, 0, 2, true)
        .add_rule(taul::ppr_id(0))
        .add_terminal(1, 3, 5, true)
        .add_rule(taul::ppr_id(1))
        .add_nonterminal(2, taul::ppr_id(0))
        .build_mappings(details);

    TAUL_LOG(taul::make_stderr_logger(), "{}\n{}", table.fmt(), details.fmt(table.grouper));

    EXPECT_FALSE(details.nonterminal_id_is_terminal_id);
    EXPECT_FALSE(details.terminal_ids_not_in_legal_range);
    EXPECT_FALSE(details.nonterminal_not_in_rules_vector);
    EXPECT_FALSE(details.collisions.empty()); // not gonna bother asserting *what* collision details should be
}

