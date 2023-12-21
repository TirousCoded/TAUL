

#include "traverser.h"


void taul::traverser::on_begin() {}

void taul::traverser::on_end() {}

void taul::traverse(
    traverser& t, 
    const node& nd) {
    t.on_begin();
    internal::do_traverse(t, nd);
    t.on_end();
}

void taul::internal::do_traverse(traverser& t, const node& nd) {
    bool skip_children = false;
    t.on_enter(nd, skip_children);
    if (!skip_children) {
        for (const auto& I : nd) {
            do_traverse(t, I);
        }
    }
    t.on_exit(nd);
}
