

#pragma once


#include "symbol_stream.h"


namespace taul {


    // filters are streams w/ an upstream source who's outputs
    // are *filtered* by the filter stream


    template<typename Symbol>
    class symbol_filter;


    using glyph_filter = symbol_filter<glyph>;
    using token_filter = symbol_filter<token>;


    template<typename Symbol>
    class symbol_filter : public symbol_stream<Symbol> {
    public:

        inline symbol_filter(std::shared_ptr<logger> lgr = nullptr);

        virtual ~symbol_filter() noexcept = default;


        // bind_source specifies the stream upstream to the filter, if any

        // bind_source invalidates the current pipeline usage state

        // w/out a source the filter will end up-front

        virtual void bind_source(symbol_stream<Symbol>* source) = 0;

        // this version takes ownership of the upstream stream

        virtual void bind_source(std::shared_ptr<symbol_stream<Symbol>> source) = 0;
    };


    template<typename Symbol>
    inline symbol_filter<Symbol>::symbol_filter(std::shared_ptr<logger> lgr) 
        : symbol_stream(lgr) {}
}

