

#pragma once


#include <taul/symbol_stream.h>


class test_token_stream_for_reset final : public taul::token_stream {
public:

    bool flag = false;


    test_token_stream_for_reset() = default;


    inline void bind_observer(taul::token_observer*) override final {}
    inline void bind_observer(std::shared_ptr<taul::token_observer>) override final {}
    inline taul::token peek() override final { return taul::token::end(); }
    inline taul::token next() override final { return taul::token::end(); }
    inline bool done() override final { return true; }
    inline void reset() override final { flag = true; }
};

