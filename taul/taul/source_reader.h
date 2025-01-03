

#pragma once


#include "str.h"
#include "source_code.h"
#include "reader.h"


namespace taul {


    class source_reader final : public reader {
    public:

        source_reader(str input = str(), encoding in_e = utf8);
        source_reader(const source_code& input);

        virtual ~source_reader() noexcept = default;


        virtual void bind_observer(glyph_observer* observer) override final;
        virtual void bind_observer(std::shared_ptr<glyph_observer> observer) override final;
        virtual glyph peek() override final;
        virtual glyph next() override final;
        virtual bool done() override final;
        void reset() override final;


        // change_input changes the string reader's input string

        // change_input invalidates the current pipeline usage state

        void change_input(str new_input, encoding in_e = utf8);
        void change_input(const source_code& new_input);


    private:

        str _input;
        encoding _in_e;
        decoder<char> _decoder;

        glyph_observer* _observer;
        std::shared_ptr<glyph_observer> _observer_ownership;

        
        // these help avoid virtual call indirection

        glyph _peek();
        bool _done() const;
        void _reset();
    };
}

