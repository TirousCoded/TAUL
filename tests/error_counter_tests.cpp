

#include <gtest/gtest.h>

#include <taul/all.h>


enum class test_error : std::uint8_t {
    a,
    b,
    c,
    num,
};

using test_error_counter = taul::error_counter<test_error>;


TEST(error_counter_tests, basicUsage) {

    test_error_counter ec{};

    
    EXPECT_EQ(ec.count(test_error::a), 0);
    EXPECT_EQ(ec.count(test_error::b), 0);
    EXPECT_EQ(ec.count(test_error::c), 0);
    EXPECT_EQ(ec.total(), 0);


    ec.raise(test_error::a);
    ec.raise(test_error::b);
    ec.raise(test_error::b);
    ec.raise(test_error::c);
    ec.raise(test_error::c);
    ec.raise(test_error::c);


    EXPECT_EQ(ec.count(test_error::a), 1);
    EXPECT_EQ(ec.count(test_error::b), 2);
    EXPECT_EQ(ec.count(test_error::c), 3);
    EXPECT_EQ(ec.total(), 6);
}

TEST(error_counter_tests, moveCtor) {

    test_error_counter old_ec{};

    old_ec.raise(test_error::a);
    old_ec.raise(test_error::b);
    old_ec.raise(test_error::b);
    old_ec.raise(test_error::c);
    old_ec.raise(test_error::c);
    old_ec.raise(test_error::c);


    test_error_counter ec(std::move(old_ec));


    EXPECT_EQ(ec.count(test_error::a), 1);
    EXPECT_EQ(ec.count(test_error::b), 2);
    EXPECT_EQ(ec.count(test_error::c), 3);
    EXPECT_EQ(ec.total(), 6);
}

TEST(error_counter_tests, moveAssign) {

    test_error_counter old_ec{};

    old_ec.raise(test_error::a);
    old_ec.raise(test_error::b);
    old_ec.raise(test_error::b);
    old_ec.raise(test_error::c);
    old_ec.raise(test_error::c);
    old_ec.raise(test_error::c);


    test_error_counter ec{};

    ec = std::move(old_ec);


    EXPECT_EQ(ec.count(test_error::a), 1);
    EXPECT_EQ(ec.count(test_error::b), 2);
    EXPECT_EQ(ec.count(test_error::c), 3);
    EXPECT_EQ(ec.total(), 6);
}

