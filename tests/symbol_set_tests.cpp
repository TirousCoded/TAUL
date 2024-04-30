

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/symbol_set.h>


// these tests will test using taul::glyph_set

// copy/move ctor/assign will be presumed to work

// simple methods like size will be presumed to work

// methods simply wrapping others (like has_size
// wrapping size) will be presumed to work

// the ***_id versions of methods will be presumed
// to be testable indirectly by testing their non-_id
// versions which are presumed to simply wrap them

// the binary + and unary/binary - operator overloads
// will be presumed to just wrap simple usage of well
// tested methods, and so will be presumed to work

// iterator tests use std::advance, and does not
// directly use operator++() or operator++(int)


TEST(SymbolSetTests, DefaultCtor) {
    taul::glyph_set set{};

    EXPECT_TRUE(set.empty());
}

TEST(SymbolSetTests, SizeDoesNotCountEpsilon) {
    taul::glyph_set set{};

    set.add(U'a');

    ASSERT_FALSE(set.includes_epsilon());

    EXPECT_EQ(set.size(), 1);

    set.add_epsilon();

    ASSERT_TRUE(set.includes_epsilon());

    EXPECT_EQ(set.size(), 1);
}

TEST(SymbolSetTests, Iterators_EmptySet) {
    taul::glyph_set set{};

    ASSERT_TRUE(set.empty());

    EXPECT_EQ(set.begin(), set.end());
    EXPECT_EQ(set.cbegin(), set.cend());

    EXPECT_EQ(set.begin(), set.cbegin());
    EXPECT_EQ(set.end(), set.cend());
}

TEST(SymbolSetTests, Iterators_NonEmptySet) {
    auto lgr = taul::make_stderr_logger();

    taul::glyph_set set{};

    // add IDs out-of-order to ensure iteration is in order

    set.add(U'c');
    set.add(U'1');
    set.add(U'a');
    set.add(U'魂');
    set.add(U'b');
    set.add(U'3');
    set.add(taul::unicode_t(0x10ffff));
    set.add(U'2');

    ASSERT_EQ(set.size(), 8);

    EXPECT_NE(set.begin(), set.end());
    EXPECT_NE(set.cbegin(), set.cend());

    EXPECT_EQ(set.begin(), set.cbegin());
    EXPECT_EQ(set.end(), set.cend());

    for (std::size_t i = 0; i < set.size() + 1; i++) {
        TAUL_LOG(lgr, "i == {}", i);
        // ensure begin == cbegin
        EXPECT_EQ(std::next(set.begin(), i), std::next(set.cbegin(), i));
        // test that IDs are in expected order
        constexpr const char32_t order[8] = { U'1', U'2', U'3', U'a', U'b', U'c', U'魂', taul::unicode_t(0x10ffff) };
        //set.dump();
        if (i < 8) EXPECT_EQ(*std::next(set.begin(), i), taul::cp_id(order[i]));
        if (i < 8) EXPECT_EQ(*std::next(set.cbegin(), i), taul::cp_id(order[i]));
        for (std::size_t j = 0; j < set.size() + 1; j++) {
            TAUL_LOG(lgr, "j == {}", j);
            // compare different combos of iterators to IDs in set
            if (i == j) {
                EXPECT_EQ(std::next(set.begin(), i), std::next(set.begin(), j));
                if (i < set.size() && j < set.size()) {
                    EXPECT_EQ(*std::next(set.begin(), i), *std::next(set.begin(), j));
                }
            }
            else {
                EXPECT_NE(std::next(set.begin(), i), std::next(set.begin(), j));
                if (i < set.size() && j < set.size()) {
                    EXPECT_NE(*std::next(set.begin(), i), *std::next(set.begin(), j));
                }
            }
        }
    }
}

TEST(SymbolSetTests, Ranges) {
    taul::glyph_set set{};

    EXPECT_TRUE(set.ranges().empty());

    set.add_range(U'a', U'f');
    set.add_range(U'D', U'B');
    set.add_range(U'3', U'7');
    set.add_range(U'k', U'i');

    if (set.ranges().size() == 4) {
        const auto range0 = taul::glyph_range{ taul::cp_id(U'3'), taul::cp_id(U'7') };
        const auto range1 = taul::glyph_range{ taul::cp_id(U'B'), taul::cp_id(U'D') };
        const auto range2 = taul::glyph_range{ taul::cp_id(U'a'), taul::cp_id(U'f') };
        const auto range3 = taul::glyph_range{ taul::cp_id(U'i'), taul::cp_id(U'k') };
        EXPECT_EQ(set.ranges()[0], range0);
        EXPECT_EQ(set.ranges()[1], range1);
        EXPECT_EQ(set.ranges()[2], range2);
        EXPECT_EQ(set.ranges()[3], range3);
    }
    else ADD_FAILURE();

    set.add_range(U'e', U'i');

    if (set.ranges().size() == 3) {
        const auto range0 = taul::glyph_range{ taul::cp_id(U'3'), taul::cp_id(U'7') };
        const auto range1 = taul::glyph_range{ taul::cp_id(U'B'), taul::cp_id(U'D') };
        const auto range2 = taul::glyph_range{ taul::cp_id(U'a'), taul::cp_id(U'k') };
        EXPECT_EQ(set.ranges()[0], range0);
        EXPECT_EQ(set.ranges()[1], range1);
        EXPECT_EQ(set.ranges()[2], range2);
    }
    else ADD_FAILURE();

    set.remove_range(U'b', U'j');

    if (set.ranges().size() == 4) {
        const auto range0 = taul::glyph_range{ taul::cp_id(U'3'), taul::cp_id(U'7') };
        const auto range1 = taul::glyph_range{ taul::cp_id(U'B'), taul::cp_id(U'D') };
        const auto range2 = taul::glyph_range{ taul::cp_id(U'a'), taul::cp_id(U'a') };
        const auto range3 = taul::glyph_range{ taul::cp_id(U'k'), taul::cp_id(U'k') };
        EXPECT_EQ(set.ranges()[0], range0);
        EXPECT_EQ(set.ranges()[1], range1);
        EXPECT_EQ(set.ranges()[2], range2);
        EXPECT_EQ(set.ranges()[3], range3);
    }
    else ADD_FAILURE();
}

TEST(SymbolSetTests, Includes) {
    taul::glyph_set set{};

    set.add(U'a');
    set.add(U'c');
    set.add(U'e');

    ASSERT_EQ(set.size(), 3);

    EXPECT_TRUE(set.includes(U'a'));
    EXPECT_TRUE(set.includes(U'c'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_FALSE(set.includes(U'f'));
}

TEST(SymbolSetTests, IncludesRange) {
    taul::glyph_set set{};

    set.add(U'a');
    set.add(U'b');
    set.add(U'c');
    //set.add(U'd');
    set.add(U'e');
    set.add(U'f');
    set.add(U'g');

    ASSERT_EQ(set.size(), 6);

    EXPECT_TRUE(set.includes_range(U'a', U'a'));
    EXPECT_TRUE(set.includes_range(U'a', U'c'));
    EXPECT_TRUE(set.includes_range(U'c', U'a')); // will swap for us

    EXPECT_TRUE(set.includes_range(U'e', U'e'));
    EXPECT_TRUE(set.includes_range(U'e', U'g'));
    EXPECT_TRUE(set.includes_range(U'g', U'e')); // will swap for us

    EXPECT_FALSE(set.includes_range(U'c', U'e'));
    EXPECT_FALSE(set.includes_range(U'e', U'c')); // will swap for us

    EXPECT_FALSE(set.includes_range(U'\0', U'b'));
    EXPECT_FALSE(set.includes_range(U'f', taul::unicode_t(0x10ffff)));
}

TEST(SymbolSetTests, IncludesEpsilon) {
    taul::glyph_set set{};

    EXPECT_FALSE(set.includes_epsilon());

    set.add_epsilon();

    EXPECT_TRUE(set.includes_epsilon());
}

TEST(SymbolSetTests, IncludesSet) {
    taul::glyph_set set0{}, set1{}, set2{};

    set0.add(U'a');
    set0.add(U'b');
    set0.add(U'c');
    set0.add(U'd');

    set1.add(U'c');
    set1.add(U'd');
    set1.add(U'e');

    set2.add(U'b');
    set2.add(U'c');

    EXPECT_FALSE(set0.includes_set(set1));
    EXPECT_TRUE(set0.includes_set(set2));
}

TEST(SymbolSetTests, IncludesSet_NicheCase1) {
    taul::glyph_set set0{}, set1{};

    set0.add(U'a');
    set0.add(U'b');
    set0.add(U'c');
    //set0.add(U'd');
    set0.add(U'e');
    set0.add(U'f');
    set0.add(U'g');

    set1.add(U'b');
    set1.add(U'c');
    set1.add(U'd');
    set1.add(U'e');
    set1.add(U'f');

    EXPECT_FALSE(set0.includes_set(set1));
}

TEST(SymbolSetTests, IncludesSet_NicheCase2) {
    taul::glyph_set set0{}, set1{};

    set0.add(U'a');
    set0.add(U'b');
    set0.add(U'c');
    //set0.add(U'd');
    set0.add(U'e');
    set0.add(U'f');
    set0.add(U'g');

    set1.add(U'b');
    set1.add(U'c');
    //set1.add(U'd');
    set1.add(U'e');
    set1.add(U'f');

    EXPECT_TRUE(set0.includes_set(set1));
}

TEST(SymbolSetTests, IncludesSet_NicheCase3) {
    taul::glyph_set set0{}, set1{}, set2{};

    set0.add_epsilon();
    //set1.add_epsilon();
    set2.add_epsilon();

    EXPECT_TRUE(set0.includes_set(set1));
    EXPECT_TRUE(set0.includes_set(set2));
}

TEST(SymbolSetTests, IncludesSet_NicheCase4) {
    taul::glyph_set set0{}, set1{}, set2{};

    //set0.add_epsilon();
    //set1.add_epsilon();
    set2.add_epsilon();

    EXPECT_TRUE(set0.includes_set(set1));
    EXPECT_FALSE(set0.includes_set(set2));
}

TEST(SymbolSetTests, Add) {
    taul::glyph_set set{};

    ASSERT_TRUE(set.empty());
    ASSERT_FALSE(set.includes(U'a'));
    ASSERT_FALSE(set.includes(U'b'));
    ASSERT_FALSE(set.includes(U'c'));
    ASSERT_FALSE(set.includes(U'd'));
    ASSERT_FALSE(set.includes(U'e'));
    ASSERT_FALSE(set.includes(U'f'));

    set.add(U'a');
    set.add(U'b');
    set.add(U'c');

    EXPECT_EQ(set.size(), 3);
    EXPECT_TRUE(set.includes(U'a'));
    EXPECT_TRUE(set.includes(U'b'));
    EXPECT_TRUE(set.includes(U'c'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_FALSE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'f'));

    set.add(U'c');
    set.add(U'd');
    set.add(U'e');

    EXPECT_EQ(set.size(), 5);
    EXPECT_TRUE(set.includes(U'a'));
    EXPECT_TRUE(set.includes(U'b'));
    EXPECT_TRUE(set.includes(U'c'));
    EXPECT_TRUE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'f'));
}

TEST(SymbolSetTests, AddRange) {
    taul::glyph_set set0{}, set1{}, set2{};

    set0.add(U'b');
    set0.add(U'c');
    set0.add(U'd');

    ASSERT_EQ(set0.size(), 3);
    ASSERT_FALSE(set0.includes(U'a'));
    ASSERT_TRUE(set0.includes(U'b'));
    ASSERT_TRUE(set0.includes(U'c'));
    ASSERT_TRUE(set0.includes(U'd'));
    ASSERT_FALSE(set0.includes(U'e'));
    ASSERT_FALSE(set0.includes(U'f'));

    ASSERT_TRUE(set1.empty());

    ASSERT_TRUE(set2.empty());

    set1.add_range(U'b', U'd');
    set2.add_range(U'd', U'b'); // will swap for us

    EXPECT_EQ(set0, set1);
    EXPECT_EQ(set0, set2);
    EXPECT_EQ(set1, set2);
}

TEST(SymbolSetTests, AddEpsilon) {
    taul::glyph_set set{};

    ASSERT_FALSE(set.includes_epsilon());

    set.add_epsilon();

    EXPECT_TRUE(set.includes_epsilon());
}

TEST(SymbolSetTests, AddSet) {
    taul::glyph_set set0{}, set1{}, set2{};

    set0.add(U'b');
    set0.add(U'c');
    set0.add(U'd');

    set1.add(U'a');
    set1.add(U'b');
    set1.add(U'c');

    set2.add(U'd');
    set2.add(U'e');
    set2.add_epsilon();

    ASSERT_EQ(set0.size(), 3);
    ASSERT_FALSE(set0.includes(U'a'));
    ASSERT_TRUE(set0.includes(U'b'));
    ASSERT_TRUE(set0.includes(U'c'));
    ASSERT_TRUE(set0.includes(U'd'));
    ASSERT_FALSE(set0.includes(U'e'));
    ASSERT_FALSE(set0.includes(U'f'));
    ASSERT_FALSE(set0.includes_epsilon());
    
    ASSERT_EQ(set1.size(), 3);
    ASSERT_TRUE(set1.includes(U'a'));
    ASSERT_TRUE(set1.includes(U'b'));
    ASSERT_TRUE(set1.includes(U'c'));
    ASSERT_FALSE(set1.includes(U'd'));
    ASSERT_FALSE(set1.includes(U'e'));
    ASSERT_FALSE(set1.includes(U'f'));
    ASSERT_FALSE(set1.includes_epsilon());
    
    ASSERT_EQ(set2.size(), 2);
    ASSERT_FALSE(set2.includes(U'a'));
    ASSERT_FALSE(set2.includes(U'b'));
    ASSERT_FALSE(set2.includes(U'c'));
    ASSERT_TRUE(set2.includes(U'd'));
    ASSERT_TRUE(set2.includes(U'e'));
    ASSERT_FALSE(set2.includes(U'f'));
    ASSERT_TRUE(set2.includes_epsilon());

    set0.add_set(set1);

    EXPECT_EQ(set0.size(), 4);
    EXPECT_TRUE(set0.includes(U'a'));
    EXPECT_TRUE(set0.includes(U'b'));
    EXPECT_TRUE(set0.includes(U'c'));
    EXPECT_TRUE(set0.includes(U'd'));
    EXPECT_FALSE(set0.includes(U'e'));
    EXPECT_FALSE(set0.includes(U'f'));
    EXPECT_FALSE(set0.includes_epsilon());

    set0.add_set(set2);

    EXPECT_EQ(set0.size(), 5);
    EXPECT_TRUE(set0.includes(U'a'));
    EXPECT_TRUE(set0.includes(U'b'));
    EXPECT_TRUE(set0.includes(U'c'));
    EXPECT_TRUE(set0.includes(U'd'));
    EXPECT_TRUE(set0.includes(U'e'));
    EXPECT_FALSE(set0.includes(U'f'));
    EXPECT_TRUE(set0.includes_epsilon());
}

TEST(SymbolSetTests, AddSet_NicheCase1) {
    taul::glyph_set set0{}, set1{}, set2{};

    set0.add(U'a');
    set0.add(U'd');

    set1.add(U'c');
    set1.add(U'd');
    set1.add(U'e');
    set1.add(U'f');

    ASSERT_EQ(set0.size(), 2);
    ASSERT_TRUE(set0.includes(U'a'));
    ASSERT_FALSE(set0.includes(U'b'));
    ASSERT_FALSE(set0.includes(U'c'));
    ASSERT_TRUE(set0.includes(U'd'));
    ASSERT_FALSE(set0.includes(U'e'));
    ASSERT_FALSE(set0.includes(U'f'));
    ASSERT_FALSE(set0.includes_epsilon());
    
    ASSERT_EQ(set1.size(), 4);
    ASSERT_FALSE(set1.includes(U'a'));
    ASSERT_FALSE(set1.includes(U'b'));
    ASSERT_TRUE(set1.includes(U'c'));
    ASSERT_TRUE(set1.includes(U'd'));
    ASSERT_TRUE(set1.includes(U'e'));
    ASSERT_TRUE(set1.includes(U'f'));
    ASSERT_FALSE(set1.includes_epsilon());

    set0.add_set(set1);

    EXPECT_EQ(set0.size(), 5);
    EXPECT_TRUE(set0.includes(U'a'));
    EXPECT_FALSE(set0.includes(U'b'));
    EXPECT_TRUE(set0.includes(U'c'));
    EXPECT_TRUE(set0.includes(U'd'));
    EXPECT_TRUE(set0.includes(U'e'));
    EXPECT_TRUE(set0.includes(U'f'));
    EXPECT_FALSE(set0.includes_epsilon());
}

TEST(SymbolSetTests, Remove) {
    taul::glyph_set set{};

    set.add(U'a');
    set.add(U'b');
    set.add(U'c');
    set.add(U'd');
    set.add(U'e');
    set.add(U'f');

    ASSERT_EQ(set.size(), 6);
    ASSERT_TRUE(set.includes(U'a'));
    ASSERT_TRUE(set.includes(U'b'));
    ASSERT_TRUE(set.includes(U'c'));
    ASSERT_TRUE(set.includes(U'd'));
    ASSERT_TRUE(set.includes(U'e'));
    ASSERT_TRUE(set.includes(U'f'));

    set.remove(U'c');

    EXPECT_EQ(set.size(), 5);
    EXPECT_TRUE(set.includes(U'a'));
    EXPECT_TRUE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_TRUE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_TRUE(set.includes(U'f'));

    set.remove(U'c'); // c no longer in set

    EXPECT_EQ(set.size(), 5);
    EXPECT_TRUE(set.includes(U'a'));
    EXPECT_TRUE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_TRUE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_TRUE(set.includes(U'f'));

    set.remove(U'a');

    EXPECT_EQ(set.size(), 4);
    EXPECT_FALSE(set.includes(U'a'));
    EXPECT_TRUE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_TRUE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_TRUE(set.includes(U'f'));

    set.remove(U'f');

    EXPECT_EQ(set.size(), 3);
    EXPECT_FALSE(set.includes(U'a'));
    EXPECT_TRUE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_TRUE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'f'));

    set.remove(U'b');

    EXPECT_EQ(set.size(), 2);
    EXPECT_FALSE(set.includes(U'a'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_TRUE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'f'));

    set.remove(U'd');

    EXPECT_EQ(set.size(), 1);
    EXPECT_FALSE(set.includes(U'a'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'f'));

    set.remove(U'e');

    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.includes(U'a'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_FALSE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'f'));
}

TEST(SymbolSetTests, RemoveRange) {
    taul::glyph_set set{};

    set.add(U'a');
    set.add(U'b');
    set.add(U'c');
    set.add(U'd');
    set.add(U'e');
    set.add(U'f');

    ASSERT_EQ(set.size(), 6);
    ASSERT_TRUE(set.includes(U'a'));
    ASSERT_TRUE(set.includes(U'b'));
    ASSERT_TRUE(set.includes(U'c'));
    ASSERT_TRUE(set.includes(U'd'));
    ASSERT_TRUE(set.includes(U'e'));
    ASSERT_TRUE(set.includes(U'f'));

    set.remove_range(U'b', U'd');

    EXPECT_EQ(set.size(), 3);
    EXPECT_TRUE(set.includes(U'a'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_TRUE(set.includes(U'e'));
    EXPECT_TRUE(set.includes(U'f'));

    set.remove_range(U'e', U'c'); // will swap for us

    EXPECT_EQ(set.size(), 2);
    EXPECT_TRUE(set.includes(U'a'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_FALSE(set.includes(U'e'));
    EXPECT_TRUE(set.includes(U'f'));

    set.remove_range(U'\0', U'c');

    EXPECT_EQ(set.size(), 1);
    EXPECT_FALSE(set.includes(U'a'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_FALSE(set.includes(U'e'));
    EXPECT_TRUE(set.includes(U'f'));

    set.remove_range(U'c', taul::unicode_t(0x10ffff));

    EXPECT_TRUE(set.empty());
}

TEST(SymbolSetTests, RemoveEpsilon) {
    taul::glyph_set set{};

    set.add_epsilon();

    ASSERT_TRUE(set.includes_epsilon());

    set.remove_epsilon();

    EXPECT_FALSE(set.includes_epsilon());
}

TEST(SymbolSetTests, RemoveSet) {
    taul::glyph_set set0{}, set1{}, set2{};

    set0.add(U'a');
    set0.add(U'b');
    set0.add(U'c');
    set0.add(U'd');
    set0.add(U'e');
    set0.add(U'f');
    set0.add_epsilon();

    set1.add(U'b');
    set1.add(U'c');
    set1.add(U'd');

    set2.add(U'd');
    set2.add(U'e');
    set2.add_epsilon();

    ASSERT_EQ(set0.size(), 6);
    ASSERT_TRUE(set0.includes(U'a'));
    ASSERT_TRUE(set0.includes(U'b'));
    ASSERT_TRUE(set0.includes(U'c'));
    ASSERT_TRUE(set0.includes(U'd'));
    ASSERT_TRUE(set0.includes(U'e'));
    ASSERT_TRUE(set0.includes(U'f'));
    ASSERT_TRUE(set0.includes_epsilon());

    ASSERT_EQ(set1.size(), 3);
    ASSERT_FALSE(set1.includes(U'a'));
    ASSERT_TRUE(set1.includes(U'b'));
    ASSERT_TRUE(set1.includes(U'c'));
    ASSERT_TRUE(set1.includes(U'd'));
    ASSERT_FALSE(set1.includes(U'e'));
    ASSERT_FALSE(set1.includes(U'f'));
    ASSERT_FALSE(set1.includes_epsilon());

    ASSERT_EQ(set2.size(), 2);
    ASSERT_FALSE(set2.includes(U'a'));
    ASSERT_FALSE(set2.includes(U'b'));
    ASSERT_FALSE(set2.includes(U'c'));
    ASSERT_TRUE(set2.includes(U'd'));
    ASSERT_TRUE(set2.includes(U'e'));
    ASSERT_FALSE(set2.includes(U'f'));
    ASSERT_TRUE(set2.includes_epsilon());

    set0.remove_set(set1);

    EXPECT_EQ(set0.size(), 3);
    EXPECT_TRUE(set0.includes(U'a'));
    EXPECT_FALSE(set0.includes(U'b'));
    EXPECT_FALSE(set0.includes(U'c'));
    EXPECT_FALSE(set0.includes(U'd'));
    EXPECT_TRUE(set0.includes(U'e'));
    EXPECT_TRUE(set0.includes(U'f'));
    EXPECT_TRUE(set0.includes_epsilon());

    set0.remove_set(set2);

    EXPECT_EQ(set0.size(), 2);
    EXPECT_TRUE(set0.includes(U'a'));
    EXPECT_FALSE(set0.includes(U'b'));
    EXPECT_FALSE(set0.includes(U'c'));
    EXPECT_FALSE(set0.includes(U'd'));
    EXPECT_FALSE(set0.includes(U'e'));
    EXPECT_TRUE(set0.includes(U'f'));
    EXPECT_FALSE(set0.includes_epsilon());
}

TEST(SymbolSetTests, RemoveAll) {
    taul::glyph_set set{};

    set.add(U'a');
    set.add(U'b');
    set.add(U'c');
    set.add(U'd');
    set.add(U'e');
    set.add(U'f');
    set.add_epsilon();

    ASSERT_EQ(set.size(), 6);
    ASSERT_TRUE(set.includes(U'a'));
    ASSERT_TRUE(set.includes(U'b'));
    ASSERT_TRUE(set.includes(U'c'));
    ASSERT_TRUE(set.includes(U'd'));
    ASSERT_TRUE(set.includes(U'e'));
    ASSERT_TRUE(set.includes(U'f'));
    ASSERT_TRUE(set.includes_epsilon());

    set.remove_all();

    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.includes(U'a'));
    EXPECT_FALSE(set.includes(U'b'));
    EXPECT_FALSE(set.includes(U'c'));
    EXPECT_FALSE(set.includes(U'd'));
    EXPECT_FALSE(set.includes(U'e'));
    EXPECT_FALSE(set.includes(U'f'));
    EXPECT_FALSE(set.includes_epsilon());
}

TEST(SymbolSetTests, Equality) {
    taul::glyph_set set0{}, set1{}, set2{}, set3{}, set4{};

    set1.add(U'a');
    set1.add(U'b');
    set1.add(U'c');
    
    set2.add(U'a');
    set2.add(U'b');
    set2.add(U'c');

    set3.add(U'a');
    set3.add(U'b');

    set4.add(U'a');
    set4.add(U'd');
    set4.add(U'c');

    EXPECT_EQ(set0, set0);
    EXPECT_NE(set0, set1);
    EXPECT_NE(set0, set2);
    EXPECT_NE(set0, set3);
    EXPECT_NE(set0, set4);

    EXPECT_EQ(set1, set1);
    EXPECT_EQ(set1, set2);
    EXPECT_NE(set1, set3);
    EXPECT_NE(set1, set4);

    EXPECT_EQ(set2, set2);
    EXPECT_NE(set2, set3);
    EXPECT_NE(set2, set4);

    EXPECT_EQ(set3, set3);
    EXPECT_NE(set3, set4);

    EXPECT_EQ(set4, set4);
}

TEST(SymbolSetTests, Equality_Epsilon) {
    taul::glyph_set set0{}, set1{}, set2{};

    set0.add(U'a');
    set0.add_epsilon();
    
    set1.add(U'a');
    set1.add_epsilon();

    set2.add(U'a');

    EXPECT_EQ(set0, set0);
    EXPECT_EQ(set0, set1);
    EXPECT_NE(set0, set2);

    EXPECT_EQ(set1, set1);
    EXPECT_NE(set1, set2);

    EXPECT_EQ(set2, set2);
}

TEST(SymbolSetTests, Inverse_A) {
    taul::glyph_set set{};

    set.add(U'a');
    //set.add(U'b');
    //set.add(U'c');
    set.add(U'd');
    set.add(U'e');
    set.add(U'f');
    set.add_id(taul::end_cp_id);
    set.add_epsilon();

    ASSERT_EQ(set.size(), 5);
    ASSERT_TRUE(set.includes(U'a'));
    ASSERT_FALSE(set.includes(U'b'));
    ASSERT_FALSE(set.includes(U'c'));
    ASSERT_TRUE(set.includes(U'd'));
    ASSERT_TRUE(set.includes(U'e'));
    ASSERT_TRUE(set.includes(U'f'));
    ASSERT_TRUE(set.includes_id(taul::end_cp_id));
    ASSERT_TRUE(set.includes_epsilon());

    const auto a = set.inverse();

    EXPECT_EQ(a.size(), 0x110000 - 4);
    EXPECT_TRUE(a.includes_range(taul::unicode_t(0), U'a' - 1));
    EXPECT_TRUE(a.includes_range(U'b', U'c'));
    EXPECT_TRUE(a.includes_range(U'f' + 1, taul::unicode_t(0x10ffff)));
    EXPECT_FALSE(a.includes_id(taul::end_cp_id));
    EXPECT_FALSE(a.includes_epsilon());

    EXPECT_EQ(a.inverse(), set);

    TAUL_LOG(taul::make_stderr_logger(), "before: {}\nafter: {}", set, a);
}

TEST(SymbolSetTests, Inverse_B) {
    taul::glyph_set set{};

    set.add(U'a');

    ASSERT_EQ(set.size(), 1);
    ASSERT_TRUE(set.includes(U'a'));
    ASSERT_FALSE(set.includes_epsilon());

    const auto a = set.inverse();

    EXPECT_EQ(a.size(), TAUL_TOTAL_IDS(cp) - 1);
    EXPECT_TRUE(a.includes_id_range(taul::symbol_traits<taul::glyph>::first_id, taul::cp_id(U'a') - 1));
    EXPECT_TRUE(a.includes_id_range(taul::cp_id(U'a') + 1, taul::symbol_traits<taul::glyph>::last_id));
    EXPECT_TRUE(a.includes_epsilon());

    EXPECT_EQ(a.inverse(), set);

    TAUL_LOG(taul::make_stderr_logger(), "before: {}\nafter: {}", set, a);
}

TEST(SymbolSetTests, Inverse_C) {
    taul::glyph_set set{};

    //set.add_epsilon();

    ASSERT_EQ(set.size(), 0);
    ASSERT_FALSE(set.includes_epsilon());

    const auto a = set.inverse();

    EXPECT_EQ(a.size(), TAUL_TOTAL_IDS(cp));
    EXPECT_TRUE(a.includes_id_range(taul::symbol_traits<taul::glyph>::first_id, taul::symbol_traits<taul::glyph>::last_id));
    EXPECT_TRUE(a.includes_epsilon());

    EXPECT_EQ(a.inverse(), set);

    TAUL_LOG(taul::make_stderr_logger(), "before: {}\nafter: {}", set, a);
}

TEST(SymbolSetTests, Inverse_D) {
    taul::glyph_set set{};

    set.add_epsilon();

    ASSERT_EQ(set.size(), 0);
    ASSERT_TRUE(set.includes_epsilon());

    const auto a = set.inverse();

    EXPECT_EQ(a.size(), TAUL_TOTAL_IDS(cp));
    EXPECT_TRUE(a.includes_id_range(taul::symbol_traits<taul::glyph>::first_id, taul::symbol_traits<taul::glyph>::last_id));
    EXPECT_FALSE(a.includes_epsilon());

    EXPECT_EQ(a.inverse(), set);

    TAUL_LOG(taul::make_stderr_logger(), "before: {}\nafter: {}", set, a);
}

