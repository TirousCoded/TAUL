

#pragma once


#include "symbol_stream.h"


namespace taul {


    // readers are the vary first step in the parsing pipeline, providing
    // a stream of glyphs to the lexer

    // readers do not define a grammar association


    class reader : public glyph_stream {
    public:

        reader(std::shared_ptr<logger> lgr = nullptr);

        virtual ~reader() noexcept = default;
    };
}

