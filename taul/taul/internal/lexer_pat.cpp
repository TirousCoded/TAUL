

#include "lexer_pat.h"

#include "../asserts.h"


taul::token taul::internal::pat_lexer_fn(
    const std::shared_ptr<lexer_state>& state,
    std::string_view txt,
    source_pos offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(state);
    const auto toplevel = static_cast<toplevel_lexer_pat*>(state.get());
    TAUL_ASSERT(toplevel);
    TAUL_ASSERT(toplevel && toplevel->gramdat);
    taul::token result{};
    if (toplevel && toplevel->gramdat) {
        result = toplevel->eval(*(toplevel->gramdat), txt, offset, source_pos(-1), lgr);
    }
    else TAUL_DEADEND;
    return result;
}

taul::internal::lexer_pat::lexer_pat(std::size_t lprInd)
    : lexer_state(),
    lprInd(lprInd) {}

taul::internal::toplevel_lexer_pat::toplevel_lexer_pat(std::size_t lprInd)
    : lexer_pat(lprInd), 
    gramdat(nullptr) {}

taul::token taul::internal::toplevel_lexer_pat::eval(const grammar_data& gramdat,  std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    std::uint32_t len = 0;
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto tkn = I->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if ((bool)tkn) {
            len += (std::uint32_t)tkn.str().length();
        }
        else {
            success = false;
            break;
        }
    }
    return
        success
        ? taul::token(gramdat._lprs[lprInd], txt.substr(offset, len), offset)
        : taul::token::failure("", offset);
}

taul::internal::begin_lexer_pat::begin_lexer_pat(std::size_t lprInd)
    : lexer_pat(lprInd) {}

taul::token taul::internal::begin_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.empty());
    return
        offset == 0
        ? taul::token(gramdat._lprs[lprInd], "", offset)
        : taul::token::failure("", offset);
}

taul::internal::end_lexer_pat::end_lexer_pat(std::size_t lprInd)
    : lexer_pat(lprInd) {}

taul::token taul::internal::end_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.empty());
    return
        offset == txt.size()
        ? taul::token(gramdat._lprs[lprInd], "", offset)
        : taul::token::failure("", offset);
}

taul::internal::any_lexer_pat::any_lexer_pat(std::size_t lprInd)
    : lexer_pat(lprInd) {}

taul::token taul::internal::any_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, 1);
    return
        !_substr.empty()
        ? taul::token(gramdat._lprs[lprInd], _substr, offset)
        : taul::token::failure("", offset);
}

taul::internal::string_lexer_pat::string_lexer_pat(std::size_t lprInd, std::string s)
    : lexer_pat(lprInd), 
    s(std::move(s)) {}

taul::token taul::internal::string_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, s.size());
    return
        _substr == std::string_view(s)
        ? taul::token(gramdat._lprs[lprInd], _substr, offset)
        : taul::token::failure("", offset);
}

taul::internal::charset_lexer_pat::charset_lexer_pat(std::size_t lprInd, std::string s)
    : lexer_pat(lprInd),
    s(std::move(s)) {}

taul::token taul::internal::charset_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, 1);
    bool success = false;
    if (!_substr.empty()) {
        for (const auto& I : s) {
            if (_substr[0] == I) {
                success = true;
                break;
            }
        }
    }
    return
        success
        ? taul::token(gramdat._lprs[lprInd], _substr, offset)
        : taul::token::failure("", offset);
}

taul::internal::sequence_lexer_pat::sequence_lexer_pat(std::size_t lprInd)
    : lexer_pat(lprInd) {}

taul::token taul::internal::sequence_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    std::uint32_t len = 0;
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto tkn = I->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if ((bool)tkn) {
            len += (std::uint32_t)tkn.str().length();
        }
        else {
            success = false;
            break;
        }
    }
    return
        success
        ? taul::token(gramdat._lprs[lprInd], txt.substr(offset, len), offset)
        : taul::token::failure("", offset);
}

taul::internal::set_lexer_pat::set_lexer_pat(std::size_t lprInd, bias b)
    : lexer_pat(lprInd),
    b(b) {}

taul::token taul::internal::set_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    token selection = token::failure("", offset);
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto tkn = I->eval(gramdat, txt, offset, localend_offset, lgr);
        // skip if fails
        if (!(bool)tkn) {
            continue;
        }
        // select first successful match no matter what
        bool select = !(bool)selection;
        // otherwise, select if new match is preferrable
        if (!select) {
            switch (b) {
            case bias::first_longest:   select = tkn.str().length() > selection.str().length();     break;
            case bias::first_shortest:  select = tkn.str().length() < selection.str().length();     break;
            case bias::last_longest:    select = tkn.str().length() >= selection.str().length();    break;
            case bias::last_shortest:   select = tkn.str().length() <= selection.str().length();    break;
            case bias::first:           select = false;                                             break;
            case bias::last:            select = true;                                              break;
            default:                    TAUL_DEADEND;                                               break;
            }
        }
        // if above two make select == true, select tkn
        if (select) {
            selection = tkn;
        }
        // if our bias is 'first', then we may exit early
        if (select && b == bias::first) {
            break;
        }
    }
    return selection;
}

taul::internal::modifier_lexer_pat::modifier_lexer_pat(std::size_t lprInd, std::uint16_t min, std::uint16_t max) 
    : lexer_pat(lprInd), 
    min(min), 
    max(max) {}

taul::token taul::internal::modifier_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.size() == 1);
    const auto has_max = max > 0;
    std::uint32_t len = 0;
    std::size_t n = 0;
    bool success = false;
    while (true) {
        const auto tkn = children.back()->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if ((bool)tkn) {
            len += (std::uint32_t)tkn.str().length();
            n++;
            if (has_max && n == max) {
                break;
            }
        }
        else break;
    }
    if (n >= min) {
        success = true;
    }
    return
        success
        ? taul::token(gramdat._lprs[lprInd], txt.substr(offset, len), offset)
        : taul::token::failure("", offset);
}

taul::internal::assertion_lexer_pat::assertion_lexer_pat(std::size_t lprInd, taul::polarity p)
    : lexer_pat(lprInd),
    p(p) {}

taul::token taul::internal::assertion_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.size() == 1);
    const auto tkn = children.back()->eval(gramdat, txt, offset, localend_offset, lgr);
    const bool success =
        p == polarity::positive 
        ? (bool)tkn
        : !(bool)tkn;
    return
        success
        ? taul::token(gramdat._lprs[lprInd], "", offset)
        : taul::token::failure("", offset);
}

taul::internal::constraint_lexer_pat::constraint_lexer_pat(std::size_t lprInd, taul::polarity p) 
    : lexer_pat(lprInd), 
    p(p) {}

taul::token taul::internal::constraint_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    TAUL_ASSERT(children.size() == 2);
    bool success{};
    const auto constrained = children[0]->eval(gramdat, txt, offset, localend_offset, lgr);
    if ((bool)constrained) {
        // putting this in an if-statement so we can skip evaluating it if constrained fails
        const auto constraining = children[1]->eval(gramdat, txt, offset, offset + source_pos(constrained.str().length()), lgr);
        success =
            p == polarity::positive
            ? (bool)constraining
            : !(bool)constraining;
    }
    else {
        success = false;
    }
    return
        success
        ? constrained
        : taul::token::failure("", offset);
}

taul::internal::localend_lexer_pat::localend_lexer_pat(std::size_t lprInd) 
    : lexer_pat(lprInd) {}

taul::token taul::internal::localend_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    return
        offset == localend_offset
        ? taul::token(gramdat._lprs[lprInd], "", offset)
        : taul::token::failure("", offset);
}
