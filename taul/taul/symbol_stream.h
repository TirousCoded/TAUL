

#pragma once


#include "pipeline_component.h"
#include "symbols.h"
#include "symbol_observer.h"


namespace taul {


    template<typename Symbol>
    class symbol_stream;


    using glyph_stream = symbol_stream<glyph>;
    using token_stream = symbol_stream<token>;
    

    template<typename Symbol>
    class symbol_stream : public pipeline_component {
    public:

        inline symbol_stream(std::shared_ptr<logger> lgr = nullptr);

        virtual ~symbol_stream() noexcept = default;


        // bind_observer binds an observer to the stream, if any

        // the observer will be forwarded the result of 'next' calls

        virtual void bind_observer(symbol_observer<Symbol>* observer) = 0;

        // this version takes ownership of the observer

        virtual void bind_observer(std::shared_ptr<symbol_observer<Symbol>> observer) = 0;

        // peek returns what the next symbol in the stream is,
        // w/out advancing it
        
        virtual Symbol peek() = 0;

        // next returns what the next symbol in the stream is,
        // advancing it, if it hasn't ended

        virtual Symbol next() = 0;

        // done returns if the stream has ended

        // querying this may require performing processing on
        // stream state to discern end-of-input symbols

        virtual bool done() = 0;
    };


    template<typename Symbol>
    inline symbol_stream<Symbol>::symbol_stream(std::shared_ptr<logger> lgr) 
        : pipeline_component(lgr) {}
}

