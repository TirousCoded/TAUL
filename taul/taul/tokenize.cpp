

#include "tokenize.h"

#include "asserts.h"
#include "grammar.h"


void taul::tokenize_into(std::vector<token>& target, const lexer& f, std::string_view txt, const std::shared_ptr<logger>& lgr) {
    tokenize_into(target, f, txt, 0, lgr);
}

void taul::tokenize_into(std::vector<token>& target, const lexer& f, std::string_view txt, source_pos offset, const std::shared_ptr<logger>& lgr) {
    TAUL_ASSERT(offset <= txt.length());
    TAUL_LOG(lgr, "tokenizing \"{}\" (offset=={})...", (std::string)txt, offset);
    const bool cut_skip_tokens = internal::get_lexer_f(f) == internal::grammar_wide_lexer_function_cut_skip_tokens;
    bool backIsFail = false;
    do {
        const auto tkn = f(txt, offset, lgr);
        TAUL_LOG(lgr, "tokenized {}", tkn);
        if ((bool)tkn) {
            // special check made to account for cutting skip tokens for grammar-wide lexers
            if (!cut_skip_tokens || tkn.lpr().qualifer != qualifier::skip) {
                target.push_back(tkn);
            }
            else {
                TAUL_LOG(lgr, "token discarded; skip token cut");
            }
            if (tkn.str().length() > 0) {
                offset += (source_pos)tkn.str().length();
                backIsFail = false;
            }
            else if (offset < txt.length()) {
                target.push_back(token::failure(txt.substr(offset, 1), offset));
                offset++;
                backIsFail = true;
            }
        }
        else if (offset < txt.length()) {
            if (backIsFail) {
                TAUL_ASSERT(!target.empty());
                const auto& back_tkn = target.back();
                const auto new_tkn = token::failure(txt.substr(back_tkn.pos(), back_tkn.str().length() + 1), back_tkn.pos());
                target.pop_back();
                target.push_back(new_tkn);
                TAUL_LOG(lgr, "token discarded; extending existing failure token");
            }
            else {
                target.push_back(token::failure(txt.substr(offset, 1), offset));
                TAUL_LOG(lgr, "token discarded; outputting unit length failure token");
            }
            offset++;
            backIsFail = true;
        }
    } while (offset < txt.length());
    const auto tkn = f(txt, offset, lgr);
    TAUL_LOG(lgr, "tokenized {}", tkn);
    if ((bool)tkn) {
        // special check made to account for cutting skip tokens for grammar-wide lexers
        if (!cut_skip_tokens || tkn.lpr().qualifer != qualifier::skip) {
            target.push_back(tkn);
        }
        else {
            TAUL_LOG(lgr, "token discarded; skip token cut");
        }
    }
    else {
        TAUL_LOG(lgr, "token discarded; no final token output");
    }
    TAUL_LOG(lgr, "tokenization complete!");
}

std::vector<taul::token> taul::tokenize(const lexer& f, std::string_view txt, const std::shared_ptr<logger>& lgr) {
    return tokenize(f, txt, 0, lgr);
}

std::vector<taul::token> taul::tokenize(const lexer& f, std::string_view txt, source_pos offset, const std::shared_ptr<logger>& lgr) {
    TAUL_ASSERT(offset <= txt.length());
    std::vector<token> result{};
    tokenize_into(result, f, txt, offset, lgr);
    return result;
}

