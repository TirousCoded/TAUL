

#include "node_assembler.h"


taul::node_assembler::node_assembler() {}

taul::node_assembler::node_assembler(const std::shared_ptr<logger>&) 
    : node_assembler() {}

taul::node_assembler& taul::node_assembler::begin(node&& x) {
    TAUL_ASSERT(_scope_stack.empty());
    _push(std::forward<node&&>(x));
    return *this;
}

taul::node taul::node_assembler::end() {
    while (_scope_stack.size() > 1) {
        exit();
    }
    return _pop();
}

taul::node_assembler& taul::node_assembler::attach(node&& x) {
    _attach(std::forward<node&&>(x));
    return *this;
}

taul::node_assembler& taul::node_assembler::enter(node&& x) {
    _push(std::forward<node&&>(x));
    return *this;
}

taul::node_assembler& taul::node_assembler::exit() {
    TAUL_ASSERT(_scope_stack.size() >= 2);
    auto nd = _pop();
    _attach(std::move(nd));
    return *this;
}

void taul::node_assembler::_push(node&& x) {
    _scope_stack.push_back(std::forward<node&&>(x));
}

taul::node taul::node_assembler::_pop() {
    TAUL_ASSERT(_scope_stack.size() >= 1);
    auto result = std::move(_scope_stack.back());
    _scope_stack.pop_back();
    return result;
}

void taul::node_assembler::_attach(node&& x) {
    TAUL_ASSERT(_scope_stack.size() >= 1);
    _scope_stack.back().attach(std::forward<node&&>(x));
}
