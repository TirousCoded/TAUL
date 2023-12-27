

#include "traverser.h"


void taul::traverser::traverse(const node& nd) {
    on_begin();
    _traverse(nd);
    on_end();
}

void taul::traverser::on_begin() {}

void taul::traverser::on_end() {}

void taul::traverser::_traverse(const node& nd) {
    bool skip_children = false;
    on_enter(nd, skip_children);
    if (!skip_children) {
        for (const auto& I : nd) {
            _traverse(I);
        }
    }
    on_exit(nd);
}
