

#include <gtest/gtest.h>

#include <taul/all.h>


// some quick-n'-dirty internal tests for taul::internal::block_alloc

#include <taul/internal/block_alloc.h>


TEST(BlockAllocTests, Tests) {

    const auto lgr = taul::make_stderr_logger();

    taul::internal::block_alloc<32, 10> ba(lgr);

    std::vector<taul::internal::block<32>*> _blocks;


    EXPECT_EQ(ba.pages(), 0);
    EXPECT_EQ(ba.blocks_recycled(), 0);
    EXPECT_EQ(ba.blocks_outstanding(), 0);


    for (std::size_t i = 0; i < 5; i++) {
        _blocks.push_back(ba.alloc());
        EXPECT_NE(_blocks.back(), nullptr) << "_blocks.back()==" << _blocks.back();
    }


    EXPECT_EQ(ba.pages(), 1);
    EXPECT_EQ(ba.blocks_recycled(), 0);
    EXPECT_EQ(ba.blocks_outstanding(), 5);


    for (std::size_t i = 0; i < 3; i++) {
        ba.free(_blocks.back());
        _blocks.pop_back();
    }


    EXPECT_EQ(ba.pages(), 1);
    EXPECT_EQ(ba.blocks_recycled(), 3);
    EXPECT_EQ(ba.blocks_outstanding(), 2);


    for (std::size_t i = 0; i < 8; i++) {
        _blocks.push_back(ba.alloc());
        EXPECT_NE(_blocks.back(), nullptr) << "_blocks.back()==" << _blocks.back();
    }


    EXPECT_EQ(ba.pages(), 1);
    EXPECT_EQ(ba.blocks_recycled(), 0);
    EXPECT_EQ(ba.blocks_outstanding(), 10);


    for (std::size_t i = 0; i < 1; i++) {
        _blocks.push_back(ba.alloc());
        EXPECT_NE(_blocks.back(), nullptr) << "_blocks.back()==" << _blocks.back();
    }


    EXPECT_EQ(ba.pages(), 2);
    EXPECT_EQ(ba.blocks_recycled(), 0);
    EXPECT_EQ(ba.blocks_outstanding(), 11);


    for (std::size_t i = 0; i < 9; i++) {
        _blocks.push_back(ba.alloc());
        EXPECT_NE(_blocks.back(), nullptr) << "_blocks.back()==" << _blocks.back();
    }


    EXPECT_EQ(ba.pages(), 2);
    EXPECT_EQ(ba.blocks_recycled(), 0);
    EXPECT_EQ(ba.blocks_outstanding(), 20);


    for (std::size_t i = 0; i < 13; i++) {
        ba.free(_blocks.back());
        _blocks.pop_back();
    }


    EXPECT_EQ(ba.pages(), 2);
    EXPECT_EQ(ba.blocks_recycled(), 13);
    EXPECT_EQ(ba.blocks_outstanding(), 7);


    for (std::size_t i = 0; i < 11; i++) {
        _blocks.push_back(ba.alloc());
        EXPECT_NE(_blocks.back(), nullptr) << "_blocks.back()==" << _blocks.back();
    }


    EXPECT_EQ(ba.pages(), 2);
    EXPECT_EQ(ba.blocks_recycled(), 2);
    EXPECT_EQ(ba.blocks_outstanding(), 18);


    for (std::size_t i = 0; i < 18; i++) {
        ba.free(_blocks.back());
        _blocks.pop_back();
    }


    EXPECT_EQ(ba.pages(), 2);
    EXPECT_EQ(ba.blocks_recycled(), 20);
    EXPECT_EQ(ba.blocks_outstanding(), 0);
}

