

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/str.h>


class StrTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }
};


TEST_F(StrTests, DefaultCtor) {
    taul::str aa{};

    EXPECT_TRUE(aa.empty());
    EXPECT_EQ(aa.data(), nullptr);

    EXPECT_EQ(aa.cbegin(), aa.cend());
    EXPECT_EQ(aa.begin(), aa.end());
    EXPECT_EQ(aa.crbegin(), aa.crend());
    EXPECT_EQ(aa.rbegin(), aa.rend());
}

TEST_F(StrTests, InitViaAllocAndCopyCharBuffer) {
    auto buff = "__abc__";

    taul::str aa(buff + 2, 3);
    taul::str bb(buff + 2, 0);

    EXPECT_EQ(aa.length(), 3);
    EXPECT_NE(aa.data(), nullptr);
    EXPECT_EQ(aa.at(0), 'a');
    EXPECT_EQ(aa.at(1), 'b');
    EXPECT_EQ(aa.at(2), 'c');

    // make sure it actually allocated new buffer

    EXPECT_NE(aa.data(), buff + 2);

    EXPECT_TRUE(bb.empty());
    EXPECT_EQ(bb.data(), nullptr);
}

TEST_F(StrTests, InitViaAllocAndFillWithCharValue) {
    taul::str aa(3, '\0');

    EXPECT_EQ(aa.length(), 3);
    EXPECT_NE(aa.data(), nullptr);
    EXPECT_EQ(aa.at(0), '\0');
    EXPECT_EQ(aa.at(1), '\0');
    EXPECT_EQ(aa.at(2), '\0');
}

TEST_F(StrTests, InitViaConvertFromStdString) {
    std::string old_aa = "";
    std::string old_bb = "abc";

    taul::str aa(old_aa);
    taul::str bb(old_bb);

    EXPECT_TRUE(aa.empty());
    EXPECT_EQ(aa.data(), nullptr);

    EXPECT_EQ(bb.length(), 3);
    EXPECT_NE(bb.data(), nullptr);
    EXPECT_EQ(bb.at(0), 'a');
    EXPECT_EQ(bb.at(1), 'b');
    EXPECT_EQ(bb.at(2), 'c');

    // make sure it actually allocated new buffer

    EXPECT_NE(bb.data(), old_bb.c_str());
}

TEST_F(StrTests, InitViaConvertFromStdStringView) {
    std::string old_aa = "";
    std::string old_bb = "abc";

    taul::str aa((std::string_view)old_aa);
    taul::str bb((std::string_view)old_bb);

    EXPECT_TRUE(aa.empty());
    EXPECT_EQ(aa.data(), nullptr);

    EXPECT_EQ(bb.length(), 3);
    EXPECT_NE(bb.data(), nullptr);
    EXPECT_EQ(bb.at(0), 'a');
    EXPECT_EQ(bb.at(1), 'b');
    EXPECT_EQ(bb.at(2), 'c');

    // make sure it actually allocated new buffer

    EXPECT_NE(bb.data(), old_bb.c_str());
}

TEST_F(StrTests, CopyCtor) {
    auto old_aa = taul::str{};
    auto old_bb = taul::str::lit("abc");

    taul::str aa(old_aa);
    taul::str bb(old_bb);

    EXPECT_TRUE(aa == taul::str{});
    EXPECT_TRUE(bb == taul::str::lit("abc"));

    EXPECT_EQ(aa.data(), old_aa.data());
    EXPECT_EQ(bb.data(), old_bb.data());
}

TEST_F(StrTests, MoveCtor) {
    auto old_aa = taul::str{};
    auto old_bb = taul::str::lit("abc");

    taul::str aa(std::move(old_aa));
    taul::str bb(std::move(old_bb));

    EXPECT_TRUE(aa == taul::str{});
    EXPECT_TRUE(bb == taul::str::lit("abc"));
}

TEST_F(StrTests, CopyAssign) {
    auto old_aa = taul::str{};
    auto old_bb = taul::str::lit("abc");

    taul::str aa{}, bb{};

    aa = old_aa;
    bb = old_bb;

    EXPECT_TRUE(aa == taul::str{});
    EXPECT_TRUE(bb == taul::str::lit("abc"));

    EXPECT_EQ(aa.data(), old_aa.data());
    EXPECT_EQ(bb.data(), old_bb.data());
}

TEST_F(StrTests, MoveAssign) {
    auto old_aa = taul::str{};
    auto old_bb = taul::str::lit("abc");

    taul::str aa{}, bb{};

    aa = std::move(old_aa);
    bb = std::move(old_bb);

    EXPECT_TRUE(aa == taul::str{});
    EXPECT_TRUE(bb == taul::str::lit("abc"));
}

TEST_F(StrTests, MoveAssignOntoSelf) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("abc");

    aa = std::move(aa);
    bb = std::move(bb);

    EXPECT_TRUE(aa == taul::str{});
    EXPECT_TRUE(bb == taul::str::lit("abc"));
}

TEST_F(StrTests, InitViaStringLiteral) {
    auto aa = taul::str::lit("abc");
    auto bb = taul::str::lit("");

    EXPECT_EQ(aa.length(), 3);
    EXPECT_NE(aa.data(), nullptr);
    EXPECT_EQ(aa.at(0), 'a');
    EXPECT_EQ(aa.at(1), 'b');
    EXPECT_EQ(aa.at(2), 'c');

    EXPECT_EQ(bb, taul::str{});
}

TEST_F(StrTests, SizeAndLengthMethods) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("abc");

    EXPECT_EQ(aa.size(), 0);
    EXPECT_EQ(aa.length(), 0);

    EXPECT_FALSE(aa.has_size());
    EXPECT_FALSE(aa.has_length());
    EXPECT_TRUE(aa.empty());

    EXPECT_EQ(bb.size(), 3);
    EXPECT_EQ(bb.length(), 3);

    EXPECT_TRUE(bb.has_size());
    EXPECT_TRUE(bb.has_length());
    EXPECT_FALSE(bb.empty());
}

TEST_F(StrTests, Data) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("abc");
    auto cc = taul::str("abc", 3);

    EXPECT_EQ(aa.data(), nullptr);

    ASSERT_NE(bb.data(), nullptr);
    EXPECT_EQ(std::string_view(bb.data(), 3), "abc");

    ASSERT_NE(cc.data(), nullptr);
    EXPECT_EQ(std::string_view(cc.data(), 3), "abc");
}

TEST_F(StrTests, At) {
    auto aa = taul::str::lit("abc");

    EXPECT_EQ(aa.at(0), 'a');
    EXPECT_EQ(aa.at(1), 'b');
    EXPECT_EQ(aa.at(2), 'c');
}

TEST_F(StrTests, At_ThrowsIfOutOfBounds) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("abc");

    EXPECT_THROW(aa.at(0), std::out_of_range);
    EXPECT_THROW(bb.at(3), std::out_of_range);
}

TEST_F(StrTests, AtUnchecked) {
    auto s = taul::str::lit("abc");

    EXPECT_EQ(s.at_unchecked(0), 'a');
    EXPECT_EQ(s.at_unchecked(1), 'b');
    EXPECT_EQ(s.at_unchecked(2), 'c');

    EXPECT_EQ(s[0], 'a');
    EXPECT_EQ(s[1], 'b');
    EXPECT_EQ(s[2], 'c');
}

TEST_F(StrTests, Back) {
    auto aa = taul::str::lit("abc");

    EXPECT_EQ(aa.back(), 'c');
}

TEST_F(StrTests, Back_ThrowsIfEmpty) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("");

    EXPECT_THROW(aa.back(), std::out_of_range);
    EXPECT_THROW(bb.back(), std::out_of_range);
}

TEST_F(StrTests, Front) {
    auto aa = taul::str::lit("abc");

    EXPECT_EQ(aa.front(), 'a');
}

TEST_F(StrTests, Front_ThrowsIfEmpty) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("");

    EXPECT_THROW(aa.front(), std::out_of_range);
    EXPECT_THROW(bb.front(), std::out_of_range);
}

TEST_F(StrTests, SubStr) {
    auto aa = taul::str::lit("abcdef");
    auto bb = taul::str("abcdef", 6);

    EXPECT_EQ(aa.substr(2, 3), taul::str::lit("cde"));
    EXPECT_EQ(aa.substr(2), taul::str::lit("cdef"));
    EXPECT_EQ(aa.substr(6), taul::str{});

    EXPECT_EQ(bb.substr(2, 3), taul::str::lit("cde"));
    EXPECT_EQ(bb.substr(2), taul::str::lit("cdef"));
    EXPECT_EQ(bb.substr(6), taul::str{});
}

TEST_F(StrTests, Iterators) {
    auto aa = taul::str::lit("abc");
    auto bb = taul::str("abc", 3);
    taul::str::iterator it{};
    {
        it = aa.begin();
        ASSERT_NE(it, aa.end());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_NE(it, aa.end());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, aa.end());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_EQ(it, aa.end());
    }
    {
        it = bb.begin();
        ASSERT_NE(it, bb.end());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_NE(it, bb.end());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, bb.end());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_EQ(it, bb.end());
    }
}

TEST_F(StrTests, Iterators_Const) {
    auto aa = taul::str::lit("abc");
    auto bb = taul::str("abc", 3);
    taul::str::const_iterator it{};
    {
        it = aa.cbegin();
        ASSERT_NE(it, aa.cend());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_NE(it, aa.cend());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, aa.cend());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_EQ(it, aa.cend());
    }
    {
        it = bb.cbegin();
        ASSERT_NE(it, bb.cend());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_NE(it, bb.cend());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, bb.cend());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_EQ(it, bb.cend());
    }
}

TEST_F(StrTests, Iterators_Reverse) {
    auto aa = taul::str::lit("abc");
    auto bb = taul::str("abc", 3);
    taul::str::reverse_iterator it{};
    {
        it = aa.rbegin();
        ASSERT_NE(it, aa.rend());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_NE(it, aa.rend());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, aa.rend());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_EQ(it, aa.rend());
    }
    {
        it = bb.rbegin();
        ASSERT_NE(it, bb.rend());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_NE(it, bb.rend());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, bb.rend());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_EQ(it, bb.rend());
    }
}

TEST_F(StrTests, Iterators_ConstReverse) {
    auto aa = taul::str::lit("abc");
    auto bb = taul::str("abc", 3);
    taul::str::const_reverse_iterator it{};
    {
        it = aa.crbegin();
        ASSERT_NE(it, aa.crend());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_NE(it, aa.crend());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, aa.crend());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_EQ(it, aa.crend());
    }
    {
        it = bb.crbegin();
        ASSERT_NE(it, bb.crend());
        EXPECT_EQ(*it, 'c');
        std::advance(it, 1);
        ASSERT_NE(it, bb.crend());
        EXPECT_EQ(*it, 'b');
        std::advance(it, 1);
        ASSERT_NE(it, bb.crend());
        EXPECT_EQ(*it, 'a');
        std::advance(it, 1);
        ASSERT_EQ(it, bb.crend());
    }
}

TEST_F(StrTests, ExplicitConversionIntoStdString) {
    auto old_aa = taul::str{};
    auto old_bb = taul::str::lit("abc");
    auto old_cc = taul::str("abc", 3);

    std::string aa{}, bb{}, cc{};

    aa = (std::string)old_aa;
    bb = (std::string)old_bb;
    cc = (std::string)old_cc;

    EXPECT_EQ(aa, "");
    EXPECT_EQ(bb, "abc");
    EXPECT_EQ(cc, "abc");
}

TEST_F(StrTests, ImplicitConversionIntoStdStringView) {
    auto old_aa = taul::str{};
    auto old_bb = taul::str::lit("abc");
    auto old_cc = taul::str("abc", 3);

    std::string_view aa{}, bb{}, cc{};

    aa = old_aa;
    bb = old_bb;
    cc = old_cc;

    EXPECT_EQ(aa.data(), old_aa.data());
    EXPECT_EQ(bb.data(), old_bb.data());
    EXPECT_EQ(cc.data(), old_cc.data());

    EXPECT_EQ(aa.length(), old_aa.length());
    EXPECT_EQ(bb.length(), old_bb.length());
    EXPECT_EQ(cc.length(), old_cc.length());
}

TEST_F(StrTests, Equality) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("abc");
    auto cc = taul::str("abc", 3);
    auto dd = taul::str::lit("def");
    auto ee = taul::str("def", 3);
    auto ff = taul::str("\0\0", 2);
    auto gg = taul::str("\0\0", 2);

    EXPECT_TRUE(taul::str::equal(aa, aa));
    EXPECT_TRUE(!taul::str::equal(aa, bb));
    EXPECT_TRUE(!taul::str::equal(aa, cc));
    EXPECT_TRUE(!taul::str::equal(aa, dd));
    EXPECT_TRUE(!taul::str::equal(aa, ee));
    EXPECT_TRUE(!taul::str::equal(aa, ff));
    EXPECT_TRUE(!taul::str::equal(aa, gg));

    EXPECT_TRUE(!taul::str::equal(bb, aa));
    EXPECT_TRUE(taul::str::equal(bb, bb));
    EXPECT_TRUE(taul::str::equal(bb, cc));
    EXPECT_TRUE(!taul::str::equal(bb, dd));
    EXPECT_TRUE(!taul::str::equal(bb, ee));
    EXPECT_TRUE(!taul::str::equal(bb, ff));
    EXPECT_TRUE(!taul::str::equal(bb, gg));

    EXPECT_TRUE(!taul::str::equal(cc, aa));
    EXPECT_TRUE(taul::str::equal(cc, bb));
    EXPECT_TRUE(taul::str::equal(cc, cc));
    EXPECT_TRUE(!taul::str::equal(cc, dd));
    EXPECT_TRUE(!taul::str::equal(cc, ee));
    EXPECT_TRUE(!taul::str::equal(cc, ff));
    EXPECT_TRUE(!taul::str::equal(cc, gg));

    EXPECT_TRUE(!taul::str::equal(dd, aa));
    EXPECT_TRUE(!taul::str::equal(dd, bb));
    EXPECT_TRUE(!taul::str::equal(dd, cc));
    EXPECT_TRUE(taul::str::equal(dd, dd));
    EXPECT_TRUE(taul::str::equal(dd, ee));
    EXPECT_TRUE(!taul::str::equal(dd, ff));
    EXPECT_TRUE(!taul::str::equal(dd, gg));

    EXPECT_TRUE(!taul::str::equal(ee, aa));
    EXPECT_TRUE(!taul::str::equal(ee, bb));
    EXPECT_TRUE(!taul::str::equal(ee, cc));
    EXPECT_TRUE(taul::str::equal(ee, dd));
    EXPECT_TRUE(taul::str::equal(ee, ee));
    EXPECT_TRUE(!taul::str::equal(ee, ff));
    EXPECT_TRUE(!taul::str::equal(ee, gg));

    EXPECT_TRUE(!taul::str::equal(ff, aa));
    EXPECT_TRUE(!taul::str::equal(ff, bb));
    EXPECT_TRUE(!taul::str::equal(ff, cc));
    EXPECT_TRUE(!taul::str::equal(ff, dd));
    EXPECT_TRUE(!taul::str::equal(ff, ee));
    EXPECT_TRUE(taul::str::equal(ff, ff));
    EXPECT_TRUE(taul::str::equal(ff, gg));

    EXPECT_TRUE(!taul::str::equal(gg, aa));
    EXPECT_TRUE(!taul::str::equal(gg, bb));
    EXPECT_TRUE(!taul::str::equal(gg, cc));
    EXPECT_TRUE(!taul::str::equal(gg, dd));
    EXPECT_TRUE(!taul::str::equal(gg, ee));
    EXPECT_TRUE(taul::str::equal(gg, ff));
    EXPECT_TRUE(taul::str::equal(gg, gg));

    EXPECT_TRUE((aa == aa));
    EXPECT_TRUE(!(aa == bb));
    EXPECT_TRUE(!(aa == cc));
    EXPECT_TRUE(!(aa == dd));
    EXPECT_TRUE(!(aa == ee));
    EXPECT_TRUE(!(aa == ff));
    EXPECT_TRUE(!(aa == gg));

    EXPECT_TRUE(!(bb == aa));
    EXPECT_TRUE((bb == bb));
    EXPECT_TRUE((bb == cc));
    EXPECT_TRUE(!(bb == dd));
    EXPECT_TRUE(!(bb == ee));
    EXPECT_TRUE(!(bb == ff));
    EXPECT_TRUE(!(bb == gg));

    EXPECT_TRUE(!(cc == aa));
    EXPECT_TRUE((cc == bb));
    EXPECT_TRUE((cc == cc));
    EXPECT_TRUE(!(cc == dd));
    EXPECT_TRUE(!(cc == ee));
    EXPECT_TRUE(!(cc == ff));
    EXPECT_TRUE(!(cc == gg));

    EXPECT_TRUE(!(dd == aa));
    EXPECT_TRUE(!(dd == bb));
    EXPECT_TRUE(!(dd == cc));
    EXPECT_TRUE((dd == dd));
    EXPECT_TRUE((dd == ee));
    EXPECT_TRUE(!(dd == ff));
    EXPECT_TRUE(!(dd == gg));

    EXPECT_TRUE(!(ee == aa));
    EXPECT_TRUE(!(ee == bb));
    EXPECT_TRUE(!(ee == cc));
    EXPECT_TRUE((ee == dd));
    EXPECT_TRUE((ee == ee));
    EXPECT_TRUE(!(ee == ff));
    EXPECT_TRUE(!(ee == gg));

    EXPECT_TRUE(!(ff == aa));
    EXPECT_TRUE(!(ff == bb));
    EXPECT_TRUE(!(ff == cc));
    EXPECT_TRUE(!(ff == dd));
    EXPECT_TRUE(!(ff == ee));
    EXPECT_TRUE((ff == ff));
    EXPECT_TRUE((ff == gg));

    EXPECT_TRUE(!(gg == aa));
    EXPECT_TRUE(!(gg == bb));
    EXPECT_TRUE(!(gg == cc));
    EXPECT_TRUE(!(gg == dd));
    EXPECT_TRUE(!(gg == ee));
    EXPECT_TRUE((gg == ff));
    EXPECT_TRUE((gg == gg));

    EXPECT_TRUE(!(aa != aa));
    EXPECT_TRUE((aa != bb));
    EXPECT_TRUE((aa != cc));
    EXPECT_TRUE((aa != dd));
    EXPECT_TRUE((aa != ee));
    EXPECT_TRUE((aa != ff));
    EXPECT_TRUE((aa != gg));

    EXPECT_TRUE((bb != aa));
    EXPECT_TRUE(!(bb != bb));
    EXPECT_TRUE(!(bb != cc));
    EXPECT_TRUE((bb != dd));
    EXPECT_TRUE((bb != ee));
    EXPECT_TRUE((bb != ff));
    EXPECT_TRUE((bb != gg));

    EXPECT_TRUE((cc != aa));
    EXPECT_TRUE(!(cc != bb));
    EXPECT_TRUE(!(cc != cc));
    EXPECT_TRUE((cc != dd));
    EXPECT_TRUE((cc != ee));
    EXPECT_TRUE((cc != ff));
    EXPECT_TRUE((cc != gg));

    EXPECT_TRUE((dd != aa));
    EXPECT_TRUE((dd != bb));
    EXPECT_TRUE((dd != cc));
    EXPECT_TRUE(!(dd != dd));
    EXPECT_TRUE(!(dd != ee));
    EXPECT_TRUE((dd != ff));
    EXPECT_TRUE((dd != gg));

    EXPECT_TRUE((ee != aa));
    EXPECT_TRUE((ee != bb));
    EXPECT_TRUE((ee != cc));
    EXPECT_TRUE(!(ee != dd));
    EXPECT_TRUE(!(ee != ee));
    EXPECT_TRUE((ee != ff));
    EXPECT_TRUE((ee != gg));

    EXPECT_TRUE((ff != aa));
    EXPECT_TRUE((ff != bb));
    EXPECT_TRUE((ff != cc));
    EXPECT_TRUE((ff != dd));
    EXPECT_TRUE((ff != ee));
    EXPECT_TRUE(!(ff != ff));
    EXPECT_TRUE(!(ff != gg));

    EXPECT_TRUE((gg != aa));
    EXPECT_TRUE((gg != bb));
    EXPECT_TRUE((gg != cc));
    EXPECT_TRUE((gg != dd));
    EXPECT_TRUE((gg != ee));
    EXPECT_TRUE(!(gg != ff));
    EXPECT_TRUE(!(gg != gg));
}

TEST_F(StrTests, EqualityWithStringView) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("abc");
    auto cc = taul::str("abc", 3);
    auto dd = taul::str::lit("def");
    auto ee = taul::str("def", 3);
    auto ff = taul::str("\0\0", 2);
    auto gg = taul::str("\0\0", 2);

    EXPECT_TRUE(taul::str::equal(aa, std::string_view("")));
    EXPECT_TRUE(!taul::str::equal(bb, std::string_view("")));
    EXPECT_TRUE(!taul::str::equal(cc, std::string_view("")));
    EXPECT_TRUE(!taul::str::equal(dd, std::string_view("")));
    EXPECT_TRUE(!taul::str::equal(ee, std::string_view("")));
    EXPECT_TRUE(!taul::str::equal(ff, std::string_view("")));
    EXPECT_TRUE(!taul::str::equal(gg, std::string_view("")));

    EXPECT_TRUE(!taul::str::equal(aa, std::string_view("abc")));
    EXPECT_TRUE(taul::str::equal(bb, std::string_view("abc")));
    EXPECT_TRUE(taul::str::equal(cc, std::string_view("abc")));
    EXPECT_TRUE(!taul::str::equal(dd, std::string_view("abc")));
    EXPECT_TRUE(!taul::str::equal(ee, std::string_view("abc")));
    EXPECT_TRUE(!taul::str::equal(ff, std::string_view("abc")));
    EXPECT_TRUE(!taul::str::equal(gg, std::string_view("abc")));

    EXPECT_TRUE(!taul::str::equal(aa, std::string_view("def")));
    EXPECT_TRUE(!taul::str::equal(bb, std::string_view("def")));
    EXPECT_TRUE(!taul::str::equal(cc, std::string_view("def")));
    EXPECT_TRUE(taul::str::equal(dd, std::string_view("def")));
    EXPECT_TRUE(taul::str::equal(ee, std::string_view("def")));
    EXPECT_TRUE(!taul::str::equal(ff, std::string_view("def")));
    EXPECT_TRUE(!taul::str::equal(gg, std::string_view("def")));

    EXPECT_TRUE(!taul::str::equal(aa, std::string_view("\0\0", 2)));
    EXPECT_TRUE(!taul::str::equal(bb, std::string_view("\0\0", 2)));
    EXPECT_TRUE(!taul::str::equal(cc, std::string_view("\0\0", 2)));
    EXPECT_TRUE(!taul::str::equal(dd, std::string_view("\0\0", 2)));
    EXPECT_TRUE(!taul::str::equal(ee, std::string_view("\0\0", 2)));
    EXPECT_TRUE(taul::str::equal(ff, std::string_view("\0\0", 2)));
    EXPECT_TRUE(taul::str::equal(gg, std::string_view("\0\0", 2)));
    
    EXPECT_TRUE((aa == std::string_view("")));
    EXPECT_TRUE(!(bb == std::string_view("")));
    EXPECT_TRUE(!(cc == std::string_view("")));
    EXPECT_TRUE(!(dd == std::string_view("")));
    EXPECT_TRUE(!(ee == std::string_view("")));
    EXPECT_TRUE(!(ff == std::string_view("")));
    EXPECT_TRUE(!(gg == std::string_view("")));

    EXPECT_TRUE(!(aa == std::string_view("abc")));
    EXPECT_TRUE((bb == std::string_view("abc")));
    EXPECT_TRUE((cc == std::string_view("abc")));
    EXPECT_TRUE(!(dd == std::string_view("abc")));
    EXPECT_TRUE(!(ee == std::string_view("abc")));
    EXPECT_TRUE(!(ff == std::string_view("abc")));
    EXPECT_TRUE(!(gg == std::string_view("abc")));

    EXPECT_TRUE(!(aa == std::string_view("def")));
    EXPECT_TRUE(!(bb == std::string_view("def")));
    EXPECT_TRUE(!(cc == std::string_view("def")));
    EXPECT_TRUE((dd == std::string_view("def")));
    EXPECT_TRUE((ee == std::string_view("def")));
    EXPECT_TRUE(!(ff == std::string_view("def")));
    EXPECT_TRUE(!(gg == std::string_view("def")));

    EXPECT_TRUE(!(aa == std::string_view("\0\0", 2)));
    EXPECT_TRUE(!(bb == std::string_view("\0\0", 2)));
    EXPECT_TRUE(!(cc == std::string_view("\0\0", 2)));
    EXPECT_TRUE(!(dd == std::string_view("\0\0", 2)));
    EXPECT_TRUE(!(ee == std::string_view("\0\0", 2)));
    EXPECT_TRUE((ff == std::string_view("\0\0", 2)));
    EXPECT_TRUE((gg == std::string_view("\0\0", 2)));

    EXPECT_TRUE(!(aa != std::string_view("")));
    EXPECT_TRUE((bb != std::string_view("")));
    EXPECT_TRUE((cc != std::string_view("")));
    EXPECT_TRUE((dd != std::string_view("")));
    EXPECT_TRUE((ee != std::string_view("")));
    EXPECT_TRUE((ff != std::string_view("")));
    EXPECT_TRUE((gg != std::string_view("")));

    EXPECT_TRUE((aa != std::string_view("abc")));
    EXPECT_TRUE(!(bb != std::string_view("abc")));
    EXPECT_TRUE(!(cc != std::string_view("abc")));
    EXPECT_TRUE((dd != std::string_view("abc")));
    EXPECT_TRUE((ee != std::string_view("abc")));
    EXPECT_TRUE((ff != std::string_view("abc")));
    EXPECT_TRUE((gg != std::string_view("abc")));

    EXPECT_TRUE((aa != std::string_view("def")));
    EXPECT_TRUE((bb != std::string_view("def")));
    EXPECT_TRUE((cc != std::string_view("def")));
    EXPECT_TRUE(!(dd != std::string_view("def")));
    EXPECT_TRUE(!(ee != std::string_view("def")));
    EXPECT_TRUE((ff != std::string_view("def")));
    EXPECT_TRUE((gg != std::string_view("def")));

    EXPECT_TRUE((aa != std::string_view("\0\0", 2)));
    EXPECT_TRUE((bb != std::string_view("\0\0", 2)));
    EXPECT_TRUE((cc != std::string_view("\0\0", 2)));
    EXPECT_TRUE((dd != std::string_view("\0\0", 2)));
    EXPECT_TRUE((ee != std::string_view("\0\0", 2)));
    EXPECT_TRUE(!(ff != std::string_view("\0\0", 2)));
    EXPECT_TRUE(!(gg != std::string_view("\0\0", 2)));
}

TEST_F(StrTests, EqualityWithCString) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("abc");
    auto cc = taul::str("abc", 3);
    auto dd = taul::str::lit("def");
    auto ee = taul::str("def", 3);

    EXPECT_TRUE(taul::str::equal(aa, ""));
    EXPECT_TRUE(!taul::str::equal(bb, ""));
    EXPECT_TRUE(!taul::str::equal(cc, ""));
    EXPECT_TRUE(!taul::str::equal(dd, ""));
    EXPECT_TRUE(!taul::str::equal(ee, ""));

    EXPECT_TRUE(!taul::str::equal(aa, "abc"));
    EXPECT_TRUE(taul::str::equal(bb, "abc"));
    EXPECT_TRUE(taul::str::equal(cc, "abc"));
    EXPECT_TRUE(!taul::str::equal(dd, "abc"));
    EXPECT_TRUE(!taul::str::equal(ee, "abc"));

    EXPECT_TRUE(!taul::str::equal(aa, "def"));
    EXPECT_TRUE(!taul::str::equal(bb, "def"));
    EXPECT_TRUE(!taul::str::equal(cc, "def"));
    EXPECT_TRUE(taul::str::equal(dd, "def"));
    EXPECT_TRUE(taul::str::equal(ee, "def"));

    EXPECT_TRUE((aa == ""));
    EXPECT_TRUE(!(bb == ""));
    EXPECT_TRUE(!(cc == ""));
    EXPECT_TRUE(!(dd == ""));
    EXPECT_TRUE(!(ee == ""));

    EXPECT_TRUE(!(aa == "abc"));
    EXPECT_TRUE((bb == "abc"));
    EXPECT_TRUE((cc == "abc"));
    EXPECT_TRUE(!(dd == "abc"));
    EXPECT_TRUE(!(ee == "abc"));

    EXPECT_TRUE(!(aa == "def"));
    EXPECT_TRUE(!(bb == "def"));
    EXPECT_TRUE(!(cc == "def"));
    EXPECT_TRUE((dd == "def"));
    EXPECT_TRUE((ee == "def"));

    EXPECT_TRUE(!(aa != ""));
    EXPECT_TRUE((bb != ""));
    EXPECT_TRUE((cc != ""));
    EXPECT_TRUE((dd != ""));
    EXPECT_TRUE((ee != ""));

    EXPECT_TRUE((aa != "abc"));
    EXPECT_TRUE(!(bb != "abc"));
    EXPECT_TRUE(!(cc != "abc"));
    EXPECT_TRUE((dd != "abc"));
    EXPECT_TRUE((ee != "abc"));

    EXPECT_TRUE((aa != "def"));
    EXPECT_TRUE((bb != "def"));
    EXPECT_TRUE((cc != "def"));
    EXPECT_TRUE(!(dd != "def"));
    EXPECT_TRUE(!(ee != "def"));
}

TEST_F(StrTests, Concat) {
    auto s0 = taul::str{};
    auto s1 = taul::str::lit("abc");
    auto s2 = taul::str("d\0f", 3);

    auto aa0 = taul::str::concat(s0, s0);
    auto bb0 = taul::str::concat(s0, s1);
    auto cc0 = taul::str::concat(s0, s2);

    auto aa1 = taul::str::concat(s1, s0);
    auto bb1 = taul::str::concat(s1, s1);
    auto cc1 = taul::str::concat(s1, s2);

    auto aa2 = taul::str::concat(s2, s0);
    auto bb2 = taul::str::concat(s2, s1);
    auto cc2 = taul::str::concat(s2, s2);

    EXPECT_EQ(aa0, taul::str::lit(""));
    EXPECT_EQ(bb0, taul::str::lit("abc"));
    EXPECT_EQ(cc0, taul::str("d\0f", 3));

    EXPECT_EQ(aa1, taul::str::lit("abc"));
    EXPECT_EQ(bb1, taul::str::lit("abcabc"));
    EXPECT_EQ(cc1, taul::str("abcd\0f", 6));

    EXPECT_EQ(aa2, taul::str("d\0f", 3));
    EXPECT_EQ(bb2, taul::str("d\0fabc", 6));
    EXPECT_EQ(cc2, taul::str("d\0fd\0f", 6));
}

TEST_F(StrTests, ConcatWithStringView) {
    auto s0 = taul::str{};
    auto s1 = taul::str::lit("abc");
    auto s2 = taul::str("d\0f", 3);

    std::string_view sv0("");
    std::string_view sv1("abc");
    std::string_view sv2("d\0f", 3);

    auto aa0 = taul::str::concat(s0, sv0);
    auto bb0 = taul::str::concat(s0, sv1);
    auto cc0 = taul::str::concat(s0, sv2);

    auto aa1 = taul::str::concat(s1, sv0);
    auto bb1 = taul::str::concat(s1, sv1);
    auto cc1 = taul::str::concat(s1, sv2);

    auto aa2 = taul::str::concat(s2, sv0);
    auto bb2 = taul::str::concat(s2, sv1);
    auto cc2 = taul::str::concat(s2, sv2);

    EXPECT_EQ(aa0, taul::str::lit(""));
    EXPECT_EQ(bb0, taul::str::lit("abc"));
    EXPECT_EQ(cc0, taul::str("d\0f", 3));

    EXPECT_EQ(aa1, taul::str::lit("abc"));
    EXPECT_EQ(bb1, taul::str::lit("abcabc"));
    EXPECT_EQ(cc1, taul::str("abcd\0f", 6));

    EXPECT_EQ(aa2, taul::str("d\0f", 3));
    EXPECT_EQ(bb2, taul::str("d\0fabc", 6));
    EXPECT_EQ(cc2, taul::str("d\0fd\0f", 6));
}

TEST_F(StrTests, ConcatWithCString) {
    auto s0 = taul::str{};
    auto s1 = taul::str::lit("abc");
    auto s2 = taul::str("def", 3);

    std::string_view sv0("");
    std::string_view sv1("abc");
    std::string_view sv2("d\0f", 3);

    auto aa0 = taul::str::concat(s0, sv0);
    auto bb0 = taul::str::concat(s0, sv1);
    auto cc0 = taul::str::concat(s0, sv2);

    auto aa1 = taul::str::concat(s1, sv0);
    auto bb1 = taul::str::concat(s1, sv1);
    auto cc1 = taul::str::concat(s1, sv2);

    auto aa2 = taul::str::concat(s2, sv0);
    auto bb2 = taul::str::concat(s2, sv1);
    auto cc2 = taul::str::concat(s2, sv2);

    EXPECT_EQ(aa0, taul::str::lit(""));
    EXPECT_EQ(bb0, taul::str::lit("abc"));
    EXPECT_EQ(cc0, taul::str("d\0f", 3));

    EXPECT_EQ(aa1, taul::str::lit("abc"));
    EXPECT_EQ(bb1, taul::str::lit("abcabc"));
    EXPECT_EQ(cc1, taul::str("abcd\0f", 6));

    EXPECT_EQ(aa2, taul::str::lit("def"));
    EXPECT_EQ(bb2, taul::str::lit("defabc"));
    EXPECT_EQ(cc2, taul::str("defd\0f", 6));
}

TEST_F(StrTests, Fmt) {
    using namespace taul::string_literals;
    // we're just testing some basic usage, w/ us presuming that if these
    // basics work that the rest of encoding stuff can be presumed also
    
    EXPECT_EQ("abc123"_str.fmt(), "abc123");
    EXPECT_EQ(u8"abc123Δ魂💩"_str.fmt(), taul::convert_encoding<char>(taul::utf32, taul::utf8, U"abc123Δ魂💩"));
    EXPECT_EQ(u"abc123Δ魂💩"_str.fmt(), taul::convert_encoding<char>(taul::utf32, taul::utf8, U"abc123Δ魂💩"));
    EXPECT_EQ(U"abc123Δ魂💩"_str.fmt(), taul::convert_encoding<char>(taul::utf32, taul::utf8, U"abc123Δ魂💩"));
    EXPECT_EQ(L"abc123Δ魂💩"_str.fmt(), taul::convert_encoding<char>(taul::utf32, taul::utf8, U"abc123Δ魂💩"));
}

TEST_F(StrTests, Hash) {
    auto aa = taul::str{};
    auto bb = taul::str::lit("");
    auto cc = taul::str::lit("abc");
    auto dd = taul::str::lit("ab");

    auto aa_hash = std::hash<taul::str>{}(aa);
    auto bb_hash = std::hash<taul::str>{}(bb);
    auto cc_hash = std::hash<taul::str>{}(cc);
    auto dd_hash = std::hash<taul::str>{}(dd);

    EXPECT_EQ(aa_hash, aa_hash);
    EXPECT_EQ(aa_hash, bb_hash);
    EXPECT_NE(aa_hash, cc_hash);
    EXPECT_NE(aa_hash, dd_hash);

    EXPECT_EQ(bb_hash, bb_hash);
    EXPECT_NE(bb_hash, cc_hash);
    EXPECT_NE(bb_hash, dd_hash);

    EXPECT_EQ(cc_hash, cc_hash);
    EXPECT_NE(cc_hash, dd_hash);

    EXPECT_EQ(dd_hash, dd_hash);
}

