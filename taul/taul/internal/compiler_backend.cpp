

#include "compiler_backend.h"


using namespace taul::string_literals;


void taul::internal::compiler_backend::push_expr() {
    expr_stk.push_back(expr{});
}

void taul::internal::compiler_backend::pop_expr() {
    auto result = _build_expr_spec();
    expr_stk.pop_back();
    _merge_down(result);
}

void taul::internal::compiler_backend::push_suffix(suffix_type type, source_pos pos) {
    TAUL_ASSERT(in_expr());
    suffix_stk.push_back(suffix{ type, pos });
    expr_stk.back().suffixes++;
}

void taul::internal::compiler_backend::pop_suffix() {
    TAUL_ASSERT(in_expr());
    TAUL_ASSERT(expr_stk.back().suffixes >= 1);
    suffix_stk.pop_back();
    expr_stk.back().suffixes--;
}

void taul::internal::compiler_backend::push_nonterminal(ppr_ref x, source_pos pos) {
    nonterminal_stk.push_back(nonterminal{ x, pos });
}

void taul::internal::compiler_backend::pop_nonterminal() {
    TAUL_ASSERT(in_nonterminal());
    nonterminal_stk.pop_back();
}

taul::internal::compiler_backend::compiler_backend(const source_code& src, spec_error_counter& ec, grammar gram, std::shared_ptr<logger> lgr, bool dbgsyms)
    : listener(lgr),
    src(&src),
    ec(&ec),
    gram(gram),
    dbgsyms(dbgsyms) {}

void taul::internal::compiler_backend::on_startup() {
    // do nothing
}

void taul::internal::compiler_backend::on_shutdown() {
    _build_result();
}

void taul::internal::compiler_backend::on_lexical(token tkn) {
    _handle_lexical(tkn);
}

void taul::internal::compiler_backend::on_syntactic(ppr_ref ppr, source_pos pos) {
    push_nonterminal(ppr, pos);
    _handle_syntactic_begin();
}

void taul::internal::compiler_backend::on_close() {
    _handle_syntactic_end();
    pop_nonterminal();
}

void taul::internal::compiler_backend::on_abort() {
    // do nothing
}

void taul::internal::compiler_backend::on_terminal_error(token_range ids, token input) {
    TAUL_DEREF_SAFE(src) {
        str input_name{};
        if (input.is_normal()) input_name = input.lpr.value().name();
        else if (input.is_failure()) input_name = "** lexical error **"_str;
        else if (input.is_end()) input_name = "** end-of-input **"_str;
        else TAUL_DEADEND;

        raise(
            input.pos,
            spec_error::syntax_error,
            "unexpected {}!",
            input_name);
    }
}

void taul::internal::compiler_backend::on_nonterminal_error(symbol_id id, token input) {
    TAUL_DEREF_SAFE(src) {
        TAUL_ASSERT(is_ppr_id(id));

        str nonterminal_name = gram.ppr_at(size_t(id) - size_t(TAUL_FIRST_ID(ppr))).name();

        str input_name{};
        if (input.is_normal()) input_name = input.lpr.value().name();
        else if (input.is_failure()) input_name = "** lexical error **"_str;
        else if (input.is_end()) input_name = "** end-of-input **"_str;
        else TAUL_DEADEND;

        raise(
            input.pos,
            spec_error::syntax_error,
            "expected {}, but found {}!",
            nonterminal_name,
            input_name);
    }
}

taul::spec taul::internal::compiler_backend::_build_expr_spec() {
    spec_writer sw{};
    // write prefixes for suffixes (in reverse order of push)
    while (expr_stk.back().suffixes >= 1) {
        auto suffix_pos = suffix_stk.back().pos; // copy pos of suffix
        switch (suffix_stk.back().type) {
        case suffix_type::optional:
        {
            if (dbgsyms) sw.pos(suffix_pos);
            sw.optional();
        }
        break;
        case suffix_type::kleene_star:
        {
            if (dbgsyms) sw.pos(suffix_pos);
            sw.kleene_star();
        }
        break;
        case suffix_type::kleene_plus:
        {
            if (dbgsyms) sw.pos(suffix_pos);
            sw.kleene_plus();
        }
        break;
        default: TAUL_DEADEND; break;
        }
        pop_suffix();
        // write pos for closes too for errors which arise at close
        if (dbgsyms) expr_stk.back().sw_main.pos(suffix_pos);
        // also gotta remember to write close to sw_main for each suffix
        expr_stk.back().sw_main.close();
    }
    // write main body of expr
    sw.write_spec(expr_stk.back().sw_main);
    return sw.done();
}

void taul::internal::compiler_backend::_merge_down(const spec& x) {
    if (in_expr()) expr_stk.back().sw_main.write_spec(x);
    else if (in_lpr()) sw_lpr_defs.write_spec(x);
    else if (in_ppr()) sw_ppr_defs.write_spec(x);
    else TAUL_DEADEND;
}

void taul::internal::compiler_backend::_build_result() {
    if (cancelled) return;
    spec_writer sw{};
    sw.write_spec(sw_lpr_decls);
    sw.write_spec(sw_ppr_decls);
    sw.write_spec(sw_lpr_defs);
    sw.write_spec(sw_ppr_defs);
    result = std::make_optional(sw.done());
}

void taul::internal::compiler_backend::_handle_lexical(token tkn) {
    TAUL_DEREF_SAFE(src && ec) {
        if (!in_nonterminal()) return; // just in case
        auto nonterminal = nonterminal_stk.back().ppr.name();
        if (tkn.is_normal()) {
            auto name = tkn.lpr.value().name();
            auto text = tkn.str(*src);
            if (name == "IDENTIFIER"_str) {
                if (nonterminal == "Rule_Name"_str) {
                    rule_name = text;
                }
                else if (nonterminal == "Name"_str) {
                    TAUL_ASSERT(in_expr());
                    if (dbgsyms) expr_stk.back().sw_main.pos(tkn.pos);
                    expr_stk.back().sw_main.name(text);
                }
            }
            else if (name == "STRING"_str) {
                if (nonterminal == "String"_str) {
                    if (text.back() != '\'') {
                        raise(
                            tkn.pos,
                            spec_error::illegal_string_literal,
                            "malformed string literal!");
                    }
                    else {
                        TAUL_ASSERT(in_expr());
                        if (dbgsyms) expr_stk.back().sw_main.pos(tkn.pos);
                        expr_stk.back().sw_main.string(text.substr(1, text.length() - 2));
                    }
                }
            }
            else if (name == "CHARSET"_str) {
                if (nonterminal == "Charset"_str) {
                    if (text.back() != ']') {
                        raise(
                            tkn.pos,
                            spec_error::illegal_charset_literal,
                            "malformed charset literal!");
                    }
                    else {
                        TAUL_ASSERT(in_expr());
                        if (dbgsyms) expr_stk.back().sw_main.pos(tkn.pos);
                        expr_stk.back().sw_main.charset(text.substr(1, text.length() - 2));
                    }
                }
            }
        }
    }
}

void taul::internal::compiler_backend::_handle_syntactic_begin() {
    TAUL_ASSERT(in_nonterminal());
    TAUL_DEREF_SAFE(src && ec) {
        auto name = nonterminal_stk.back().ppr.name();
        if (name == "LexerSection"_str) {
            current_section = section::lexer;
        }
        else if (name == "ParserSection"_str) {
            current_section = section::parser;
        }
        else if (name == "Rule"_str) {
            current_qualifier = qualifier::none;
            qualifier_count = 0;
        }
        else if (name == "Rule_Alts"_str) {
            if (in_lpr()) {
                if (dbgsyms) {
                    sw_lpr_decls.pos(nonterminal_stk.back().pos);
                    sw_lpr_defs.pos(nonterminal_stk.back().pos);
                }
                sw_lpr_decls.lpr_decl(rule_name);
                sw_lpr_defs.lpr(rule_name, current_qualifier);
            }
            if (in_ppr()) {
                if (dbgsyms) {
                    sw_ppr_decls.pos(nonterminal_stk.back().pos);
                    sw_ppr_defs.pos(nonterminal_stk.back().pos);
                }
                sw_ppr_decls.ppr_decl(rule_name);
                sw_ppr_defs.ppr(rule_name, current_qualifier);
            }
        }
        else if (name == "Qualifier"_str) {
            qualifier_count++;
        }
        else if (name == "Qualifier_Skip"_str) {
            current_qualifier = qualifier::skip;
        }
        else if (name == "Qualifier_Support"_str) {
            current_qualifier = qualifier::support;
        }
        else if (name == "Expr"_str) {
            push_expr();
        }
        else if (name == "Expr_NoSuffix"_str) {
            push_expr();
        }
        else if (name == "Sequence"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.sequence();
        }
        else if (name == "LookAhead"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.lookahead();
        }
        else if (name == "LookAheadNot"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.lookahead_not();
        }
        else if (name == "Not"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.not0();
        }
        else if (name == "Optional_Suffix"_str) {
            push_suffix(suffix_type::optional, nonterminal_stk.back().pos);
        }
        else if (name == "KleeneStar_Suffix"_str) {
            push_suffix(suffix_type::kleene_star, nonterminal_stk.back().pos);
        }
        else if (name == "KleenePlus_Suffix"_str) {
            push_suffix(suffix_type::kleene_plus, nonterminal_stk.back().pos);
        }
        else if (name == "Alt_Divider"_str) {
            // the way we have it setup we gotta account for 
            // toplevel Alts not being in an Expr
            if (in_expr()) {
                if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
                expr_stk.back().sw_main.alternative();
            }
            else {
                if (in_lpr()) {
                    if (dbgsyms) sw_lpr_defs.pos(nonterminal_stk.back().pos);
                    sw_lpr_defs.alternative();
                }
                if (in_ppr()) {
                    if (dbgsyms) sw_ppr_defs.pos(nonterminal_stk.back().pos);
                    sw_ppr_defs.alternative();
                }
            }
        }
    }
}

void taul::internal::compiler_backend::_handle_syntactic_end() {
    TAUL_ASSERT(in_nonterminal());
    TAUL_DEREF_SAFE(src && ec) {
        auto name = nonterminal_stk.back().ppr.name();
        if (name == "Rule_Qualifiers"_str) {
            if (qualifier_count >= 2) {
                TAUL_DEREF_SAFE(src) {
                    raise(
                        nonterminal_stk.back().pos,
                        spec_error::illegal_multiple_qualifiers, 
                        "illegal multiple qualifiers on rule!");
                }
            }
        }
        else if (name == "Rule_Alts"_str) {
            if (in_lpr()) {
                // write pos for closes too for errors which arise at close
                if (dbgsyms) sw_lpr_defs.pos(nonterminal_stk.back().pos);
                sw_lpr_defs.close();
            }
            if (in_ppr()) {
                // write pos for closes too for errors which arise at close
                if (dbgsyms) sw_ppr_defs.pos(nonterminal_stk.back().pos);
                sw_ppr_defs.close();
            }
        }
        else if (name == "Expr"_str) {
            pop_expr();
        }
        else if (name == "Expr_NoSuffix"_str) {
            pop_expr();
        }
        else if (name == "End"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.end();
        }
        else if (name == "Any"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.any();
        }
        else if (name == "Token"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.token();
        }
        else if (name == "Failure"_str) {
            TAUL_ASSERT(in_expr());
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.failure();
        }
        else if (name == "Sequence"_str) {
            TAUL_ASSERT(in_expr());
            // write pos for closes too for errors which arise at close
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.close();
        }
        else if (name == "LookAhead"_str) {
            TAUL_ASSERT(in_expr());
            // write pos for closes too for errors which arise at close
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.close();
        }
        else if (name == "LookAheadNot"_str) {
            TAUL_ASSERT(in_expr());
            // write pos for closes too for errors which arise at close
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.close();
        }
        else if (name == "Not"_str) {
            TAUL_ASSERT(in_expr());
            // write pos for closes too for errors which arise at close
            if (dbgsyms) expr_stk.back().sw_main.pos(nonterminal_stk.back().pos);
            expr_stk.back().sw_main.close();
        }
    }
}

