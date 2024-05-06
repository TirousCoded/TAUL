

#include "listener.h"


taul::listener::listener(std::shared_ptr<logger> lgr) 
    : api_component(lgr) {}

void taul::listener::playback(const parse_tree& x) {
    on_startup();
    // each node knows how nested it is, called its 'level'
    // when the current node's level is below or equal to level of top
    // nodestk node, then we know it's time to pop said top nodestk node
    std::vector<std::size_t> nodestk{};
    for (const auto& I : x) {
        // use 'level' info to tell when it's time to close
        while (!nodestk.empty()) {
            const auto& nd = x.at(nodestk.back());
            // if I.level() <= nd.level(), then we want to close, as returning
            // to the level of nd.level() means we've exited nd
            if (I.level() > nd.level()) break;
            on_close();
            nodestk.pop_back();
        }
        // after closing any in need of closing, now add lexical, or open syntactic
        if (I.is_lexical()) on_lexical(I.tkn().value());
        if (I.is_syntactic()) {
            on_syntactic(I.ppr().value(), I.pos());
            nodestk.push_back(I.index());
        }
    }
    // close any still outstanding
    while (!nodestk.empty()) {
        const auto& nd = x.at(nodestk.back());
        on_close();
        nodestk.pop_back();
    }
    if (x.is_aborted()) on_abort();
    on_shutdown();
}

