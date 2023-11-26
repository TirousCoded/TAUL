

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
        const auto& lpr = toplevel->gramdat->_lprs[toplevel->lprInd];
        const auto matched = toplevel->eval(*(toplevel->gramdat), txt, offset, source_pos(-1), lgr);
        result = 
            matched.success 
            ? taul::token(lpr, matched.txt, matched.offset) 
            : taul::token::failure("", matched.offset);
    }
    else TAUL_DEADEND;
    return result;
}

taul::internal::lexer_pat::lexer_pat()
    : lexer_state() {}

taul::internal::toplevel_lexer_pat::toplevel_lexer_pat(std::size_t lprInd)
    : lexer_pat(),
    lprInd(lprInd),
    gramdat(nullptr) {}

taul::internal::match taul::internal::toplevel_lexer_pat::eval(const grammar_data& gramdat,  std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    std::uint32_t len = 0;
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if (matched.success) {
            len += (std::uint32_t)matched.txt.length();
        }
        else {
            success = false;
            break;
        }
    }
    return
        success
        ? make_match(true, txt.substr(offset, len), offset)
        : make_match(false, "", offset);
}

taul::internal::begin_lexer_pat::begin_lexer_pat()
    : lexer_pat() {}

taul::internal::match taul::internal::begin_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    return make_match(offset == 0, "", offset);
}

taul::internal::end_lexer_pat::end_lexer_pat()
    : lexer_pat() {}

taul::internal::match taul::internal::end_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    return make_match(offset == txt.size(), "", offset);
}

taul::internal::any_lexer_pat::any_lexer_pat()
    : lexer_pat() {}

taul::internal::match taul::internal::any_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, 1);
    return
        !_substr.empty()
        ? make_match(true, _substr, offset)
        : make_match(false, "", offset);
}

taul::internal::string_lexer_pat::string_lexer_pat(std::string s)
    : lexer_pat(), 
    s(std::move(s)) {}

taul::internal::match taul::internal::string_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, s.size());
    return
        _substr == std::string_view(s)
        ? make_match(true, _substr, offset)
        : make_match(false, "", offset);
}

taul::internal::charset_lexer_pat::charset_lexer_pat(std::string s)
    : lexer_pat(),
    s(std::move(s)) {}

taul::internal::match taul::internal::charset_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
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
        ? make_match(true, _substr, offset)
        : make_match(false, "", offset);
}

taul::internal::sequence_lexer_pat::sequence_lexer_pat()
    : lexer_pat() {}

taul::internal::match taul::internal::sequence_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    std::uint32_t len = 0;
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if (matched.success) {
            len += (std::uint32_t)matched.txt.length();
        }
        else {
            success = false;
            break;
        }
    }
    return
        success
        ? make_match(true, txt.substr(offset, len), offset)
        : make_match(false, "", offset);
}

taul::internal::set_lexer_pat::set_lexer_pat(bias b)
    : lexer_pat(),
    b(b) {}

taul::internal::match taul::internal::set_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    match selection = make_match(false, "", offset);
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(gramdat, txt, offset, localend_offset, lgr);
        // skip if fails
        if (!matched.success) {
            continue;
        }
        // select first successful match no matter what
        bool select = !selection.success;
        // otherwise, select if new match is preferrable
        if (!select) {
            switch (b) {
            case bias::first_longest:   select = matched.txt.length() > selection.txt.length();     break;
            case bias::first_shortest:  select = matched.txt.length() < selection.txt.length();     break;
            case bias::last_longest:    select = matched.txt.length() >= selection.txt.length();    break;
            case bias::last_shortest:   select = matched.txt.length() <= selection.txt.length();    break;
            case bias::first:           select = false;                                             break;
            case bias::last:            select = true;                                              break;
            default:                    TAUL_DEADEND;                                               break;
            }
        }
        // if above two make select == true, select tkn
        if (select) {
            selection = matched;
        }
        // if our bias is 'first', then we may exit early
        if (select && b == bias::first) {
            break;
        }
    }
    return selection;
}

taul::internal::modifier_lexer_pat::modifier_lexer_pat(std::uint16_t min, std::uint16_t max) 
    : lexer_pat(), 
    min(min), 
    max(max) {}

taul::internal::match taul::internal::modifier_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 1);
    const auto has_max = max > 0;
    std::uint32_t len = 0;
    std::size_t n = 0;
    bool success = false;
    while (true) {
        const auto matched = children.back()->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if (matched.success) {
            len += (std::uint32_t)matched.txt.length();
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
        ? make_match(true, txt.substr(offset, len), offset)
        : make_match(false, "", offset);
}

taul::internal::assertion_lexer_pat::assertion_lexer_pat(taul::polarity p)
    : lexer_pat(),
    p(p) {}

taul::internal::match taul::internal::assertion_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 1);
    const auto matched = children.back()->eval(gramdat, txt, offset, localend_offset, lgr);
    const bool success =
        p == polarity::positive 
        ? matched.success
        : !matched.success;
    return make_match(success, "", offset);
}

taul::internal::constraint_lexer_pat::constraint_lexer_pat(taul::polarity p) 
    : lexer_pat(), 
    p(p) {}

taul::internal::match taul::internal::constraint_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 2);
    bool success{};
    const auto constrained = children[0]->eval(gramdat, txt, offset, localend_offset, lgr);
    if (constrained.success) {
        // putting this in an if-statement so we can skip evaluating it if constrained fails
        const auto constraining = children[1]->eval(gramdat, txt, offset, offset + source_pos(constrained.txt.length()), lgr);
        success =
            p == polarity::positive
            ? constraining.success
            : !constraining.success;
    }
    else {
        success = false;
    }
    return
        success
        ? constrained
        : make_match(false, "", offset);
}

taul::internal::localend_lexer_pat::localend_lexer_pat() 
    : lexer_pat() {}

taul::internal::match taul::internal::localend_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    return make_match(offset == localend_offset, "", offset);
}

taul::internal::name_ofLPR_forLPR_lexer_pat::name_ofLPR_forLPR_lexer_pat(std::size_t lprIndOfRef)
    : lexer_pat(), 
    lprIndOfRef(lprIndOfRef) {}

taul::internal::match taul::internal::name_ofLPR_forLPR_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(lprIndOfRef < gramdat._lprs.size());
    const auto tkn = gramdat._lprs[lprIndOfRef].lexer()(txt, offset, lgr);
    return
        (bool)tkn
        ? make_match(true, tkn.str(), offset)
        : make_match(false, "", offset);
}
