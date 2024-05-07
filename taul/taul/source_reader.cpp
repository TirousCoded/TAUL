

#include "source_reader.h"


using namespace taul::string_literals;


taul::source_reader::source_reader(str input, encoding in_e)
    : reader(), 
    _input(input),
    _in_e(in_e),
    _decoder(in_e, input),
    _observer(nullptr) {}

taul::source_reader::source_reader(const source_code& input)
    : source_reader(input.str()) {}

void taul::source_reader::bind_observer(glyph_observer* observer) {
    _observer = observer;
    _observer_ownership.reset();
}

void taul::source_reader::bind_observer(std::shared_ptr<glyph_observer> observer) {
    _observer = observer.get();
    _observer_ownership = observer;
}

taul::glyph taul::source_reader::peek() {
    return _peek();
}

taul::glyph taul::source_reader::next() {
    glyph result = _peek();
    if (_observer) _observer->observe(result);
    _decoder.next(); // advance input state
    return result;
}

bool taul::source_reader::done() {
    return _done();
}

void taul::source_reader::reset() {
    _reset();
}

void taul::source_reader::change_input(str new_input, encoding in_e) {
    _input = new_input;
    _in_e = in_e;
    _decoder = decoder<char>(in_e, new_input);
    _reset();
}

void taul::source_reader::change_input(const source_code& new_input) {
    change_input(new_input.str());
}

taul::glyph taul::source_reader::_peek() {
    auto data = _decoder.peek();
    if (_done()) return glyph::end(source_pos(_decoder.pos()));
    if (!data) return glyph::end(source_pos(_decoder.pos())); // fail safe
    return glyph::normal(data.value().cp, source_pos(_decoder.pos()), source_len(data.value().chars<char>()));
}

bool taul::source_reader::_done() const {
    return _decoder.done();
}

void taul::source_reader::_reset() {
    _decoder = decoder<char>(_in_e, _input);
}

