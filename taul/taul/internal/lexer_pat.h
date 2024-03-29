

#pragma once


#include "../grammar.h"
#include "../token.h"

#include "migrated/lexer.h"


namespace taul {


    struct lexer_rule;


    namespace internal {


        struct lexer_match final {
            bool success;
            std::string_view txt;
            source_pos offset;
        };

        inline lexer_match make_lexer_match(
            bool success,
            std::string_view txt,
            source_pos offset) noexcept {
            return lexer_match{ success, txt, offset };
        }


        // TODO: currently, we're gonna build our lexer/parser patterns as trees of
        //       the below, which is fine, but I feel we could revise the below 
        //       implementation to GREATLY improve memory locality, which I think
        //       could help given how much processing lexing/parsing involves

        taul::token pat_lexer_fn(
            const std::shared_ptr<lexer_state>& state,
            std::string_view txt,
            source_pos offset,
            const std::shared_ptr<taul::logger>& lgr);

        // gonna make lexer_pat extend lexer_state, as it lets us use toplevel_lexer_pat
        // as the 'const std::shared_ptr<lexer_state>&' passed into pat_lexer_fn

        class lexer_pat : public lexer_state {
        public:

            // these lexer_pat objects form an expr tree for the lpr, w/ children thusly
            // encapsulating the children of any particular node in this tree

            // these children are the 'subexprs' of the expr tree node in question

            std::vector<std::shared_ptr<lexer_pat>> children;


            lexer_pat();

            virtual ~lexer_pat() noexcept = default;


            virtual lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) = 0;
        };


        class toplevel_lexer_pat final : public lexer_pat {
        public:

            const std::size_t lprInd;

            // IMPORTANT: this MUST be non-owning or it'll form a strong reference cycle!

            const grammar_data* gramdat;


            toplevel_lexer_pat(std::size_t lprInd);


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class begin_lexer_pat final : public lexer_pat {
        public:

            begin_lexer_pat();


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class end_lexer_pat final : public lexer_pat {
        public:

            end_lexer_pat();


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class any_lexer_pat final : public lexer_pat {
        public:

            any_lexer_pat();


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class string_lexer_pat final : public lexer_pat {
        public:

            std::string s;


            string_lexer_pat(std::string s);


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class charset_lexer_pat final : public lexer_pat {
        public:

            std::string s;


            charset_lexer_pat(std::string s);


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class range_lexer_pat final : public lexer_pat {
        public:

            char a;
            char b;


            range_lexer_pat(char a, char b);


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;


        private:

            bool _in_range(char x) const noexcept;
        };
        
        class name_lexer_pat final : public lexer_pat {
        public:

            std::size_t lprIndOfRef = std::size_t(-1);


            name_lexer_pat(std::size_t lprIndOfRef);


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class sequence_lexer_pat final : public lexer_pat {
        public:

            sequence_lexer_pat();


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class set_lexer_pat final : public lexer_pat {
        public:

            set_lexer_pat();


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class not_lexer_pat final : public lexer_pat {
        public:

            not_lexer_pat();


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class modifier_lexer_pat final : public lexer_pat {
        public:

            std::uint16_t min, max;


            modifier_lexer_pat(std::uint16_t min, std::uint16_t max);


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };

        class assertion_lexer_pat final : public lexer_pat {
        public:

            bool positive;


            assertion_lexer_pat(bool positive);


            lexer_match eval(
                const grammar_data& gramdat,
                std::string_view txt,
                source_pos offset,
                const source_pos localend_offset,
                const std::shared_ptr<taul::logger>& lgr) override final;
        };
    }
}

