

#include "tokenize.h"

#include "../../asserts.h"
#include "../../grammar.h"


void taul::internal::tokenize_into(token_seq& target, const lexer& f, std::string_view txt, const std::shared_ptr<logger>& lgr) {
    tokenize_into(target, f, txt, 0, lgr);
}

void taul::internal::tokenize_into(token_seq& target, const lexer& f, std::string_view txt, source_pos offset, const std::shared_ptr<logger>& lgr) {
    TAUL_ASSERT(offset <= txt.length());
    TAUL_LOG(lgr, "tokenizing \"{}\" (offset=={})...", (std::string)txt, offset);
    target.reset(txt);
    target.skip(offset);
    const bool cut_skip_tokens = get_lexer_f(f) == grammar_wide_lexer_function_cut_skip_tokens;
    bool backIsFail = false;
    do {
        const auto tkn = f(target.str(), target.read_pos(), lgr);
        TAUL_LOG(lgr, "tokenized {}", tkn);
        if ((bool)tkn) {
            // special check made to account for cutting skip tokens for grammar-wide lexing
            if (!cut_skip_tokens || tkn.lpr().qualifer != qualifier::skip) {
                target.push(tkn.lpr(), (std::uint32_t)tkn.str().length());
            }
            else {
                target.skip((std::uint32_t)tkn.str().length());
                TAUL_LOG(lgr, "token discarded; skip token cut");
            }
            if (tkn.str().length() > 0) {
                backIsFail = false;
            }
            else if (!target.at_end()) {
                target.push_failure(1);
                backIsFail = true;
            }
        }
        else if (!target.at_end()) {
            if (backIsFail) {
                TAUL_ASSERT(!target.empty());
                const token back_tkn = target.back();
                target.pop();
                target.push_failure((std::uint32_t)back_tkn.str().length() + 1);
                TAUL_LOG(lgr, "token discarded; extending existing failure token");
            }
            else {
                target.push_failure(1);
                TAUL_LOG(lgr, "token discarded; outputting unit length failure token");
            }
            backIsFail = true;
        }
    } while (!target.at_end());
    const auto tkn = f(target.str(), target.read_pos(), lgr);
    TAUL_LOG(lgr, "tokenized {}", tkn);
    if ((bool)tkn) {
        // special check made to account for cutting skip tokens for grammar-wide lexing
        if (!cut_skip_tokens || tkn.lpr().qualifer != qualifier::skip) {
            target.push(tkn.lpr(), (std::uint32_t)tkn.str().length());
        }
        else {
            target.skip((std::uint32_t)tkn.str().length());
            TAUL_LOG(lgr, "token discarded; skip token cut");
        }
    }
    else {
        TAUL_LOG(lgr, "token discarded; no final token output");
    }
    TAUL_LOG(lgr, "tokenization complete!");
}

taul::token_seq taul::internal::tokenize(const lexer& f, std::string_view txt, const std::shared_ptr<logger>& lgr) {
    return tokenize(f, txt, 0, lgr);
}

taul::token_seq taul::internal::tokenize(const lexer& f, std::string_view txt, source_pos offset, const std::shared_ptr<logger>& lgr) {
    TAUL_ASSERT(offset <= txt.length());
    token_seq result(txt); // <- dummy
    tokenize_into(result, f, txt, offset, lgr);
    return result;
}

