

#pragma once


#include "api_component.h"


namespace taul {


    // observers are special sink-like support components used to observe
    // the glyphs/tokens output by a stream

    // obververs are not themselves streams, w/ the reason being that 
    // streams are *pull-based*, while observers are *push-based*, being
    // acted upon by the stream they observe, not the other way around


    template<typename Symbol>
    class symbol_observer;


    using glyph_observer = symbol_observer<glyph>;
    using token_observer = symbol_observer<token>;


    template<typename Symbol>
    class symbol_observer : public api_component {
    public:

        inline symbol_observer(std::shared_ptr<logger> lgr = nullptr);

        virtual ~symbol_observer() noexcept = default;


        // observe specifies the next symbol observed

        virtual void observe(Symbol x) = 0;
    };


    template<typename Symbol>
    inline symbol_observer<Symbol>::symbol_observer(std::shared_ptr<logger> lgr) 
        : api_component(lgr) {}
}

