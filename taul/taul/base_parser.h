

#pragma once


#include "grammar.h"
#include "pipeline_component.h"
#include "symbol_stream.h"
#include "listener.h"
#include "error_handler.h"


namespace taul {


    // IMPORTANT: taul::base_parser is not intended to be implemented by the
    //            end-user of the TAUL library, instead existing to help
    //            decouple the API in case I ever want to impl some kind of
    //            alternative impl of taul::parser
    //
    //            given the above, the end-user has not been provided w/ the
    //            code required to impl taul::base_parser w/out tapping into
    //            the API backend


    // parsers are the third and final step in the parsing pipeline, 
    // providing final parse tree generation, and/or just outputting to
    // a listener, w/ the parser receiving tokens from an upstream lexer 
    // (or other token stream)

    // parsers establish a grammar association upon initialization


    class base_parser : public pipeline_component {
    public:

        const grammar gram;


        inline base_parser(grammar gram, std::shared_ptr<logger> lgr = nullptr)
            : pipeline_component(lgr),
            gram(std::move(gram)) {}

        virtual ~base_parser() noexcept = default;


        // bind_source specifies the stream upstream to the parser, if any

        // bind_source invalidates the current pipeline usage state

        // w/out a source the parser will have no input to parse

        // behaviour is undefined if the lexer upstream to the parser has
        // a different grammar association

        // behaviour is undefined if this is called from an error handler
        // during one of its events

        virtual void bind_source(token_stream* source) = 0;

        // this version takes ownership of the upstream stream

        virtual void bind_source(std::shared_ptr<token_stream> source) = 0;


        // bind_listener specifies the listener of the parser, if any

        // behaviour is undefined if this is called from an error handler
        // during one of its events

        virtual void bind_listener(listener* listener) = 0;

        // this version takes ownership of the listener

        virtual void bind_listener(std::shared_ptr<listener> listener) = 0;


        // bind_error_handler specifies the error handler of the parser, if any

        // behaviour is undefined if this is called from an error handler
        // during one of its events

        virtual void bind_error_handler(error_handler* error_handler) = 0;

        // this version takes ownership of the error handler

        virtual void bind_error_handler(std::shared_ptr<error_handler> error_handler) = 0;


        // parse and parse_notree invoke the parser w/ start_rule

        // parse_notree forgos the allocation of a parse tree

        // behaviour is undefined if the start_rule passed to the parser has
        // a different grammar association

        // behaviour is undefined if the grammar has no PPR under name

        // behaviour is undefined if this is called from an error handler
        // during one of its events

        // TODO: the '(const str& name)' overloads haven't really been unit tested

        virtual parse_tree parse(ppr_ref start_rule) = 0;
        virtual parse_tree parse(const str& name) = 0;

        virtual void parse_notree(ppr_ref start_rule) = 0;
        virtual void parse_notree(const str& name) = 0;


        // TODO: the below methods haven't really been unit tested

        // these are used by error handler impls to query/manipulate the pipeline

        // behaviour is undefined if these are used outside the context
        // of parser error recovery

        virtual token eh_peek() = 0; // forwards 'peek' call to lexer
        virtual token eh_next() = 0; // forwards 'next' call to lexer
        virtual bool eh_done() = 0; // forwards 'done' call to lexer

        virtual bool eh_check() = 0; // checks if parser error has been recovered from
    };
}

