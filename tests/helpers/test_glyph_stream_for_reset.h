

#pragma once


#include <taul/symbol_stream.h>


class test_glyph_stream_for_reset final : public taul::glyph_stream {
public:

    bool flag = false;


    test_glyph_stream_for_reset() = default;


    inline void bind_observer(taul::glyph_observer*) override final {}
    inline void bind_observer(std::shared_ptr<taul::glyph_observer>) override final {}
    inline taul::glyph peek() override final { return taul::glyph::end(); }
    inline taul::glyph next() override final { return taul::glyph::end(); }
    inline bool done() override final { return true; }
    inline void reset() override final { flag = true; }
};

