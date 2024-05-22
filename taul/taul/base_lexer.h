

#pragma once


#include "grammar.h"
#include "symbol_stream.h"


namespace taul {


    // IMPORTANT: taul::base_lexer is not intended to be implemented by the
    //            end-user of the TAUL library, instead existing to help
    //            decouple the API in case I ever want to impl some kind of
    //            alternative impl of taul::lexer
    //
    //            given the above, the end-user has not been provided w/ the
    //            code required to impl taul::base_lexer w/out tapping into
    //            the API backend


    // lexers are the second step in the parsing pipeline, providing
    // a stream of tokens to the parser, w/ the lexer receiving glyphs
    // from an upstream reader (or other glyph stream)

    // lexers establish a grammar association upon initialization


    // lexers are expected to have a few notable semantics:

    //      1) when a match fails and forms a failure token, the impl is
    //         expected to try and form the next token after that, and if
    //         it's another failure, to concatenate the two, repeating
    //         this process

    //      2) if a match succeeds, but is length 0, then the next token
    //         after that is to be a length 1 failure token, so as to avoid
    //         falling into an infinite loop due to lexer being deterministic
    //
    //         these failure tokens are subject to the concatentation of #1

    // TODO: should we have a semantic for if input is length 0 and no
    //       length 0 token was able to be matched, and no token is produced


    class base_lexer : public token_stream {
    public:

        const grammar gram;

        // cut_skip_tokens specifies if the lexer should forgo including
        // skip tokens in its stream output

        // cut skip tokens are still observed by observers

        bool cut_skip_tokens = true;


        inline base_lexer(grammar gram, std::shared_ptr<logger> lgr = nullptr) 
            : token_stream(lgr),
            gram(std::move(gram)) {}

        virtual ~base_lexer() noexcept = default;


        // bind_source specifies the stream upstream to the lexer, if any

        // bind_source invalidates the current pipeline usage state

        // w/out a source the lexer will end up-front

        virtual void bind_source(glyph_stream* source) = 0;

        // this version takes ownership of the upstream stream

        virtual void bind_source(std::shared_ptr<glyph_stream> source) = 0;
    };
}

