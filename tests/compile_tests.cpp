

#include <gtest/gtest.h>

#include <taul/all.h>


class CompileTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;

    taul::node_ctx ctx;
    taul::spec_error_counter ec;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }
};


TEST_F(CompileTests, FailWithSourceCodeNotFoundSpecError_DueToSrcSharedPtrBeingNullptr) {

    std::shared_ptr<taul::source_code> src = nullptr;

    auto result = taul::compile(ctx, src, ec, lgr);


    EXPECT_FALSE(result);

    EXPECT_EQ(ec.count(taul::spec_error::source_code_not_found), 1);
}

TEST_F(CompileTests, FailWithSourceCodeNotFoundSpecError_DueToSrcFileNotFound) {

    std::filesystem::path src_path = std::filesystem::current_path() / "does-not-exist.taul";

    ASSERT_FALSE(std::filesystem::exists(src_path)) << "src_path file incorrectly exists!";

    auto result = taul::compile(ctx, src_path, ec, lgr);


    EXPECT_FALSE(result);

    EXPECT_EQ(ec.count(taul::spec_error::source_code_not_found), 1);
}

