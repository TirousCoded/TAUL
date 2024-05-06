

#pragma once


#include "str.h"
#include "source_code.h"
#include "grammar.h"
#include "api_component.h"
#include "parse_tree.h"
#include "symbol_range.h"


namespace taul {


    // the listener is a component of the parser which, in a push-based
    // manner, receives information about the parse tree resolved during
    // the parsing process, in a depth-first manner, this information
    // coming in the form of a series of events

    // this is vary similar to the symbol observers of symbol streams


    class listener : public api_component {
    public:

        listener(std::shared_ptr<logger> lgr = nullptr);

        virtual ~listener() noexcept = default;


        // playback is used to play the listener event sequence that would
        // arise during the parsing of the specified parse tree

        // this is useful for re-evaluation of a parse tree after parsing

        void playback(const parse_tree& tree);


        // these events arise upon the vary start and end of listener
        // usage, during parsing or playback

        virtual void on_startup() = 0;
        virtual void on_shutdown() = 0;

        // these events arise upon the addition of a new lexical node,
        // during parsing or playback

        virtual void on_lexical(token tkn) = 0;

        // these events arise upon the opening/closing of a syntactic node,
        // during parsing or playback

        // length information is transferred implicitly via the child
        // node events which arise within the scope of the syntactic node

        virtual void on_syntactic(ppr_ref ppr, source_pos pos) = 0;

        virtual void on_close() = 0;

        // this event arises when a fatal syntax error aborts parsing,
        // during parsing or playback, occurring after all others

        virtual void on_abort() = 0;


        // TODO: for now I'm just having these not participate in playback,
        //       but in the future we may want to change that maybe

        // these events are used to report errors which arise during parsing

        // these arise whether or not they were able to be recovered from

        // unlike the above events, these do not participate in playback, as
        // they are not part of the parse tree

        virtual void on_terminal_error(token_range ids, token input) = 0;
        virtual void on_nonterminal_error(symbol_id id, token input) = 0;
    };
}

