

#include <gtest/gtest.h>

#include <taul/hashing.h>
#include <taul/str.h>


using namespace taul::string_literals;


// TODO: I don't consider the below to be comprehensive enough w/ regards
//       to unit testing hashing in TAUL

TEST(HashingTests, Hash) {
    auto a = 31.143;
    auto b = "abc"_str;
    auto c = 10;
    auto d = U'$';

    EXPECT_EQ(taul::hash(a), std::hash<decltype(a)>{}(a));
    EXPECT_EQ(taul::hash(b), std::hash<decltype(b)>{}(b));
    EXPECT_EQ(taul::hash(c), std::hash<decltype(c)>{}(c));
    EXPECT_EQ(taul::hash(d), std::hash<decltype(d)>{}(d));

    auto expected1 = taul::hash_combine(taul::hash(a), taul::hash(b));
    auto expected2 = taul::hash_combine(taul::hash(a), taul::hash_combine(taul::hash(b), taul::hash(c)));
    auto expected3 = taul::hash_combine(taul::hash(a), taul::hash_combine(taul::hash(b), taul::hash_combine(taul::hash(c), taul::hash(d))));

    auto actual1 = taul::hash(a, b);
    auto actual2 = taul::hash(a, b, c);
    auto actual3 = taul::hash(a, b, c, d);

    EXPECT_EQ(expected1, actual1);
    EXPECT_EQ(expected2, actual2);
    EXPECT_EQ(expected3, actual3);
}

