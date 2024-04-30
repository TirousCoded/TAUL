

#include "string_reader.h"


using namespace taul::string_literals;


taul::string_reader::string_reader(str input)
    : reader(), 
    _input(std::move(input)),
    _pos(0), 
    _observer(nullptr) {}

taul::string_reader::string_reader(const source_code& input)
    : string_reader(input.str()) {}

void taul::string_reader::bind_observer(glyph_observer* observer) {
    _observer = observer;
    _observer_ownership.reset();
}

void taul::string_reader::bind_observer(std::shared_ptr<glyph_observer> observer) {
    _observer = observer.get();
    _observer_ownership = observer;
}

taul::glyph taul::string_reader::peek() {
    return _peek();
}

taul::glyph taul::string_reader::next() {
    glyph result = _peek();
    if (!_done()) _pos++;
    if (_observer) _observer->observe(result);
    return result;
}

bool taul::string_reader::done() {
    return _done();
}

void taul::string_reader::reset() {
    _reset();
}

void taul::string_reader::change_input(str new_input) {
    _input = std::move(new_input);
    _reset();
}

void taul::string_reader::change_input(const source_code& new_input) {
    change_input(new_input.str());
}

taul::glyph taul::string_reader::_peek() {
    if (_done()) return glyph::end(_pos);
    return glyph::normal(unicode_t(_input[_pos]), _pos);
}

bool taul::string_reader::_done() const {
    return _pos == _input.length();
}

void taul::string_reader::_reset() {
    _pos = 0;
}

