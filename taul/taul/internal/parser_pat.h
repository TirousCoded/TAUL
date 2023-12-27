

#pragma once


#include "../bias.h"
#include "../polarity.h"
#include "../grammar.h"
#include "../token.h"
#include "../node.h"
#include "../parser.h"


namespace taul {


    struct parser_rule;


    namespace internal {


        // for each parser match, its source string position and length
        // can be resolved using just the tokens it encompasses

        // to this end, the following maps token indices to source positions,
        // w/ it working by mapping every valid token index to the source
        // position of said token, and mapping the special case where the
        // token index equals the token sequence length to be the source
        // position at the vary end of the 

        // given a taul::token_seq tkns, and some input ind, the following
        // mapping is performed:
        //      1) if ind < tkns.size(), the source position is the source
        //         position of the token at ind
        //      2) if ind == tkns.size(), the source position is the source
        //         position at the end of the final token's section of
        //         source string, or 0 if there are no tokens

        // TODO: gonna need to update this when we add 'virtual tokens'

        source_pos map_tkn_to_src_pos(
            const taul::token_seq& tkns,
            std::size_t offset) noexcept;


        struct parser_match;
        class toplevel_parser_pat;

        // the below create_node, perform_eval, and finish_node, are to be
        // called in order, defining the behaviour of the whole process

        // these three define the behaviour of pat_parser_fn, and the behaviour
        // of name_parser_pat::eval, which needs to *nick* information about
        // the match created by perform_eval

        node create_node(
            node_ctx& ctx);

        parser_match perform_eval(
            taul::node& nd,
            toplevel_parser_pat* toplevel,
            node_ctx& ctx,
            const taul::token_seq& tkns,
            std::size_t offset,
            const std::shared_ptr<taul::logger>& lgr);

        void finish_node(
            taul::node& nd,
            toplevel_parser_pat* toplevel,
            const parser_match& match,
            const taul::token_seq& tkns);


        struct parser_match final {
            bool success = false;
            std::span<const token> tkns;
            std::size_t offset = 0;

            // in order for the system to not break when new nodes are added to the
            // end of the sequence, last_notpte below is NOT a past-the-end iterator

            node::iterator first;
            node::iterator last_notpte;


            inline node::iterator begin() const noexcept { return first; }
            inline node::iterator end() const noexcept { return std::next(last_notpte); }
        };

        inline parser_match make_parser_match(
            bool success,
            std::span<const token> tkns,
            std::size_t offset,
            node::iterator first,
            node::iterator last_notpte) noexcept {
            return parser_match{ success, tkns, offset, first, last_notpte };
        }


        // TODO: currently, we're gonna build our lexer/parser patterns as trees of
        //       the below, which is fine, but I feel we could revise the below 
        //       implementation to GREATLY improve memory locality, which I think
        //       could help given how much processing lexing/parsing involves

        std::optional<node> pat_parser_fn(
            node_ctx& ctx,
            const std::shared_ptr<parser_state>& state,
            const taul::token_seq& tkns,
            std::size_t offset,
            const std::shared_ptr<taul::logger>& lgr);

        // gonna make parser_pat extend parser_state, as it lets us use toplevel_parser_pat
        // as the 'const std::shared_ptr<parser_state>&' passed into pat_parser_fn

        class parser_pat : public parser_state {
        public:

            // these parser_pat objects form an expr tree for the ppr, w/ children thusly
            // encapsulating the children of any particular node in this tree

            // these children are the 'subexprs' of the expr tree node in question

            std::vector<std::shared_ptr<parser_pat>> children;


            parser_pat();

            virtual ~parser_pat() noexcept = default;


            // each ppr usage via pat_parser_fn creates a node 'nd' which will be the 
            // resultant of the total evaluation, if successful

            // this node is propagated through the parser_pat objects of the evaluation, w/
            // each one creating 0 or more child nodes for it, w/ the 'matches' created
            // by each evaluation encapsulating a range of these child nodes, w/ every of
            // such child nodes being associated w/ a range

            // these eval methods must ensure that nd.detach(_range/all) is used to 
            // appropriately cleanup the child nodes of unneeded matches

            // the toplevel_parser_pat object's returned match is to encompass *all* of
            // the child nodes of this node, and describe its final state more broadly

            virtual parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) = 0;
        };


        class toplevel_parser_pat final : public parser_pat {
        public:

            const std::size_t pprInd;

            // IMPORTANT: this MUST be non-owning or it'll form a strong reference cycle!

            const grammar_data* gramdat;


            toplevel_parser_pat(std::size_t pprInd);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class begin_parser_pat final : public parser_pat {
        public:

            begin_parser_pat();


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class end_parser_pat final : public parser_pat {
        public:

            end_parser_pat();


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class any_parser_pat final : public parser_pat {
        public:

            any_parser_pat();


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class string_parser_pat final : public parser_pat {
        public:

            std::string s;


            string_parser_pat(std::string s);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        // TODO: keep this in case we want to re-add it later

        /*class charset_parser_pat final : public parser_pat {
        public:

            std::string s;


            charset_parser_pat(std::string s);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };*/

        class token_parser_pat final : public parser_pat {
        public:

            token_parser_pat();


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class failure_parser_pat final : public parser_pat {
        public:

            failure_parser_pat();


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class name_parser_pat final : public parser_pat {
        public:

            bool lpr = true;
            std::size_t ruleIndOfRef = std::size_t(-1);


            name_parser_pat(bool lpr, std::size_t ruleIndOfRef);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class sequence_parser_pat final : public parser_pat {
        public:

            sequence_parser_pat();


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class set_parser_pat final : public parser_pat {
        public:

            bias b;


            set_parser_pat(bias b);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class modifier_parser_pat final : public parser_pat {
        public:

            std::uint16_t min, max;


            modifier_parser_pat(std::uint16_t min, std::uint16_t max);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class assertion_parser_pat final : public parser_pat {
        public:

            taul::polarity p;


            assertion_parser_pat(taul::polarity p);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class constraint_parser_pat final : public parser_pat {
        public:

            taul::polarity p;


            constraint_parser_pat(taul::polarity p);


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class localend_parser_pat final : public parser_pat {
        public:

            localend_parser_pat();


            parser_match eval(
                node_ctx& ctx,
                node& nd,
                const grammar_data& gramdat,
                const taul::token_seq& tkns,
                std::size_t offset,
                const std::size_t localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };
    }
}

