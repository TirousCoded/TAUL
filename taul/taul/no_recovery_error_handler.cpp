

#include "no_recovery_error_handler.h"


taul::no_recovery_error_handler::no_recovery_error_handler(std::shared_ptr<logger> lgr) 
    : error_handler(lgr) {}

void taul::no_recovery_error_handler::startup(base_parser*) {
    // do nothing
}

void taul::no_recovery_error_handler::shutdown() {
    // do nothing
}

void taul::no_recovery_error_handler::terminal_error(token_range, token) {
    // do nothing
}

void taul::no_recovery_error_handler::nonterminal_error(symbol_id, token) {
    // do nothing
}

