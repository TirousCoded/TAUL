

#include "regular_error_handler.h"

#include "base_parser.h"


taul::regular_error_handler::regular_error_handler(std::shared_ptr<logger> lgr) 
    : error_handler(lgr) {}

void taul::regular_error_handler::startup(base_parser* client) {
    TAUL_ASSERT(client);
    _client = client;
}

void taul::regular_error_handler::shutdown() {
    // do nothing
}

void taul::regular_error_handler::terminal_error(token_range ids, token input) {
    TAUL_DEREF_SAFE(_client) {
        while (true) {
            if (_client->eh_done()) return;
            if (_client->eh_check()) return;
            _client->eh_next();
        }
    }
}

void taul::regular_error_handler::nonterminal_error(symbol_id id, token input) {
    TAUL_DEREF_SAFE(_client) {
        while (true) {
            if (_client->eh_done()) return;
            if (_client->eh_check()) return;
            _client->eh_next();
        }
    }
}

