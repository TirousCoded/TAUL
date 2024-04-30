

#include <gtest/gtest.h>

#include <taul/logger.h>


class test_logger final : public taul::logger {
public:

    std::string last_msg;
    std::size_t logs = 0;


    test_logger() = default;


protected:

    inline void on_log(const std::string& msg) override final {
        last_msg = msg;
        logs++;
    }
};


TEST(logger_tests, basicUsage) {

    const auto lgr = std::make_shared<test_logger>();


    EXPECT_EQ(lgr->logs, 0);

    lgr->log("test 1");

    EXPECT_EQ(lgr->last_msg, "test 1");
    EXPECT_EQ(lgr->logs, 1);

    taul::log(lgr, "test {}", 2);

    ASSERT_EQ(lgr->last_msg, "test 2");
    EXPECT_EQ(lgr->logs, 2);

    TAUL_LOG(lgr, "test {}", 3);

    ASSERT_EQ(lgr->last_msg, "test 3");
    EXPECT_EQ(lgr->logs, 3);

    TAUL_LOG_IF(false, lgr, "test {}", 4); // <- don't log!

    ASSERT_EQ(lgr->last_msg, "test 3");
    EXPECT_EQ(lgr->logs, 3);

    TAUL_LOG_IF(true, lgr, "test {}", 5);

    ASSERT_EQ(lgr->last_msg, "test 5");
    EXPECT_EQ(lgr->logs, 4);


    // these should do nothing, w/out issues

    taul::log(nullptr, "do nothing {}", 1);
    TAUL_LOG(nullptr, "do nothing {}", 2);
    TAUL_LOG_IF(false, nullptr, "do nothing {}", 3);
    TAUL_LOG_IF(true, nullptr, "do nothing {}", 4);
}


TEST(logger_tests, stdout_logger) {

    const auto lgr = taul::make_stdout_logger();

    ASSERT_TRUE((bool)lgr); // <- impl. should always succeed


    // can't really tell if this *work*, but it shouldn't *crash*

    TAUL_LOG(lgr, "test usage {}, {}, {}", "abc", 123, "hi stdout!");
}


TEST(logger_tests, stderr_logger) {

    const auto lgr = taul::make_stderr_logger();

    ASSERT_TRUE((bool)lgr); // <- impl. should always succeed


    // can't really tell if this *work*, but it shouldn't *crash*

    TAUL_LOG(lgr, "test usage {}, {}, {}", "abc", 123, "hi stderr!");
}

