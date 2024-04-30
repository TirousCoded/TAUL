

#include <gtest/gtest.h>

#include <taul/internal/id_grouper.h>


TEST(IDGrouperTests, Glyph_Init) {
    taul::internal::id_grouper<taul::glyph> grouper{};
    using traits_t = taul::symbol_traits<taul::glyph>;
    
    for (std::uint32_t i = 0; i < 0x110000; i += 0x10000) {
        EXPECT_EQ(std::uint32_t(grouper.get_group_id(taul::cp_id(i))), 0);
    }
    EXPECT_EQ(std::uint32_t(grouper.get_group_id(taul::cp_id(0x10ffff))), 0);

    const auto range0 = taul::glyph_range{ TAUL_FIRST_ID(cp), TAUL_LAST_ID(cp) };
    EXPECT_EQ(grouper.get_symbol_range(0), range0);
}

TEST(IDGrouperTests, Glyph_NonInit) {
    taul::internal::id_grouper<taul::glyph> grouper{};
    using traits_t = taul::symbol_traits<taul::glyph>;
    
    grouper.add_use_case(taul::cp_id(U'*'), taul::cp_id(U'*'));
    grouper.add_use_case(taul::cp_id(U'3'), taul::cp_id(U'8'));
    grouper.add_use_case(taul::cp_id(U'?'), taul::cp_id(U'?'));
    grouper.add_use_case(taul::cp_id(U'B'), taul::cp_id(U'D'));
    grouper.add_use_case(taul::cp_id(U'a'), taul::cp_id(U'f'));
    grouper.add_use_case(taul::cp_id(U'C'), taul::cp_id(U'b'));
    grouper.add_use_case(taul::cp_id(U'魂'), taul::cp_id(U'魂'));

    EXPECT_EQ(grouper.get_group_id(traits_t::first_id), 0);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'*') - 1), 0);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'*')), 1);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'*') + 1), 2);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'3') - 1), 2);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'3')), 3);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'8')), 3);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'8') + 1), 4);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'?') - 1), 4);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'?')), 5);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'?') + 1), 6);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'B') - 1), 6);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'B')), 7);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'C')), 8);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'D')), 8);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'D') + 1), 9);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'a') - 1), 9);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'a')), 10);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'b')), 10);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'b') + 1), 11);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'f')), 11);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'f') + 1), 12);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'魂') - 1), 12);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'魂')), 13);
    EXPECT_EQ(grouper.get_group_id(taul::cp_id(U'魂') + 1), 14);
    EXPECT_EQ(grouper.get_group_id(traits_t::last_id), 14);

    const auto range0 = taul::glyph_range{ traits_t::first_id, taul::cp_id(U'*') - 1 };
    const auto range1 = taul::glyph_range{ taul::cp_id(U'*'), taul::cp_id(U'*') };
    const auto range2 = taul::glyph_range{ taul::cp_id(U'*') + 1, taul::cp_id(U'3') - 1 };
    const auto range3 = taul::glyph_range{ taul::cp_id(U'3'), taul::cp_id(U'8') };
    const auto range4 = taul::glyph_range{ taul::cp_id(U'8') + 1, taul::cp_id(U'?') - 1 };
    const auto range5 = taul::glyph_range{ taul::cp_id(U'?'), taul::cp_id(U'?') };
    const auto range6 = taul::glyph_range{ taul::cp_id(U'?') + 1, taul::cp_id(U'B') - 1 };
    const auto range7 = taul::glyph_range{ taul::cp_id(U'B'), taul::cp_id(U'B') };
    const auto range8 = taul::glyph_range{ taul::cp_id(U'C'), taul::cp_id(U'D') };
    const auto range9 = taul::glyph_range{ taul::cp_id(U'D') + 1, taul::cp_id(U'a') - 1 };
    const auto range10 = taul::glyph_range{ taul::cp_id(U'a'), taul::cp_id(U'b') };
    const auto range11 = taul::glyph_range{ taul::cp_id(U'b') + 1, taul::cp_id(U'f') };
    const auto range12 = taul::glyph_range{ taul::cp_id(U'f') + 1, taul::cp_id(U'魂') - 1 };
    const auto range13 = taul::glyph_range{ taul::cp_id(U'魂'), taul::cp_id(U'魂') };
    const auto range14 = taul::glyph_range{ taul::cp_id(U'魂') + 1, traits_t::last_id };
    EXPECT_EQ(grouper.get_symbol_range(0), range0);
    EXPECT_EQ(grouper.get_symbol_range(1), range1);
    EXPECT_EQ(grouper.get_symbol_range(2), range2);
    EXPECT_EQ(grouper.get_symbol_range(3), range3);
    EXPECT_EQ(grouper.get_symbol_range(4), range4);
    EXPECT_EQ(grouper.get_symbol_range(5), range5);
    EXPECT_EQ(grouper.get_symbol_range(6), range6);
    EXPECT_EQ(grouper.get_symbol_range(7), range7);
    EXPECT_EQ(grouper.get_symbol_range(8), range8);
    EXPECT_EQ(grouper.get_symbol_range(9), range9);
    EXPECT_EQ(grouper.get_symbol_range(10), range10);
    EXPECT_EQ(grouper.get_symbol_range(11), range11);
    EXPECT_EQ(grouper.get_symbol_range(12), range12);
    EXPECT_EQ(grouper.get_symbol_range(13), range13);
    EXPECT_EQ(grouper.get_symbol_range(14), range14);
}

// see id_grouper.h for why this is commented out

// also, I COULD write some tests to replace below... but I'm lazy, so I haven't, lol,
// as our id_grouper<token> code basically copies id_grouper<glyph> almost exactly

//TEST(IDGrouperTests, Token_Init) {
//    taul::internal::id_grouper<taul::token> grouper{};
//    using traits_t = taul::symbol_traits<taul::token>;
//
//    for (std::uint32_t i = 0; i < 65'000; i += 1'000) {
//        EXPECT_EQ(std::uint32_t(grouper.get_group_id(taul::lpr_id(i))), i);
//    }
//    EXPECT_EQ(std::uint32_t(grouper.get_group_id(TAUL_LAST_ID(lpr))), std::uint32_t(TAUL_TOTAL_IDS(lpr) - 1));
//
//    for (std::uint32_t i = 0; i < 65'000; i += 1'000) {
//        const auto range0 = taul::token_range{ taul::lpr_id(i), taul::lpr_id(i) };
//        EXPECT_EQ(grouper.get_symbol_range(i), range0);
//    }
//    const auto range0 = taul::token_range{ taul::lpr_id(taul::internal::group_id(TAUL_TOTAL_IDS(lpr) - 1)), taul::lpr_id(taul::internal::group_id(TAUL_TOTAL_IDS(lpr) - 1)) };
//    EXPECT_EQ(grouper.get_symbol_range(taul::internal::group_id(TAUL_TOTAL_IDS(lpr) - 1)), range0);
//}

