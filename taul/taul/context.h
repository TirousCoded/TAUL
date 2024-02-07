

#pragma once


#include <string_view>
#include <optional>

#include "logger.h"

#include "grammar.h"

#include "token.h"
#include "token_seq.h"
#include "node.h"


namespace taul {


    class no_grammar_bound_error final : public std::logic_error {
    public:

        inline explicit no_grammar_bound_error(const std::string& msg) : logic_error(msg) {}
        inline explicit no_grammar_bound_error(const char* msg) : logic_error(msg) {}
    };


    class context final {
    public:

        std::shared_ptr<logger> lgr = nullptr;


        context();

        context(
            std::shared_ptr<logger> lgr);

        // this overload binds a grammar up-front to the context

        context(
            const grammar& gram,
            std::shared_ptr<logger> lgr = nullptr);

        context(const context&) = delete;
        context(context&& x) noexcept;

        ~context() noexcept = default;

        context& operator=(const context&) = delete;
        context& operator=(context&& rhs) noexcept;


        // TODO: these are temporary, to be replaced later w/ taul::parse_tree

        node create(
            const lexer_rule& lpr,
            std::string_view str,
            source_pos pos = 0);

        node create(
            token tkn);

        node create(
            const parser_rule& ppr,
            std::string_view str,
            source_pos pos = 0);


        // binds gram to the context, replacing any previous binding, if any

        void bind(
            const grammar& gram);


        // match resolves a single token from an input

        // start_rule specifies the LPR to use as the start rule, w/ the overload
        // w/out start_rule using the grammar as a whole

        // input and offset specify the input and the offset into it where lexical
        // analysis is to begin, w/ offset == input.length() being allowed

        // throw taul::no_grammar_bound_error if no grammar is bound

        // throw taul::lpr_not_found_error if start_rule specifies no valid LPR

        // behaviour is undefined if offset > input.length()

        token match(
            std::string_view start_rule,
            std::string_view input,
            source_pos offset = 0);

        token match(
            std::string_view input,
            source_pos offset = 0);


        // TODO: be sure to incorporate the below into our *official* documentation later

        // TODO: in our *official* documentation, be sure to include details about how tokenize
        //       methods have special behaviour regarding cutting 'skip tokens' for grammar-wide lexers

        // if a token during tokenization produced has length 0, then to avoid an infinite loop
        // (since the lexer will otherwise deterministically resolve the same token over-and-over)
        // the function will insert a failure token matching the next character into the output

        // if during the above the length 0 token is located at the end of the text, no failure token
        // will be inserted

        // if a match during tokenization matches a failure token, of any length, then a unit length 
        // failure token at the position of the failed match will be output, unless the failure 
        // occurs at the vary end of text, in which case no such insertion will occur

        // if multiple failure tokens created during tokenization are contiguous, they will be
        // concatenated into a single failure token, inheriting the position value of the first (w/ 
        // all such tokens being expected to exist contiguosly regarding their positions)
        

        // tokenize resolves a sequence of tokens from an input

        // start_rule specifies the LPR to use as the start rule, w/ the overload
        // w/out start_rule using the grammar as a whole

        // input and offset specify the input and the offset into it where lexical
        // analysis is to begin, w/ offset == input.length() being allowed

        // for the overload using the grammar as a whole, cut_skip_tokens specifies
        // if 'skip' tokens should be cut, w/ this being enabled by default

        // throw taul::no_grammar_bound_error if no grammar is bound

        // throw taul::lpr_not_found_error if start_rule specifies no valid LPR

        // behaviour is undefined if offset > input.length()

        token_seq tokenize(
            std::string_view start_rule,
            std::string_view input,
            source_pos offset = 0);
        
        token_seq tokenize(
            std::string_view input,
            source_pos offset = 0,
            bool cut_skip_tokens = true);


        // parse parses a parse tree from an input

        // start_rule specifies the LPR to use as the start rule, w/ the overload
        // w/out start_rule using the grammar as a whole

        // input and offset specify the input and the offset into it where
        // parsing is to begin, w/ offset == input.length() being allowed

        // throw taul::no_grammar_bound_error if no grammar is bound

        // throw taul::ppr_not_found_error if start_rule specifies no valid PPR

        // behaviour is undefined if offset > input.length()

        std::optional<node> parse(
            std::string_view start_rule,
            const taul::token_seq& input,
            std::size_t offset = 0);
        
        std::optional<node> parse(
            std::string_view start_rule,
            std::string_view input,
            source_pos offset = 0);


    private:

        std::unique_ptr<node_ctx> nodectx;
        std::optional<grammar> gram;


        void _check_grammar_bound() const;
        void _check_has_lpr_start_rule(std::string_view start_rule) const;
        void _check_has_ppr_start_rule(std::string_view start_rule) const;
    };
}

