

#include "compile.h"

#include "grammar.h"
#include "lexer.h"
#include "tokenize.h"
#include "load_taul_grammar.h"

#include "internal/util.h"


std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::shared_ptr<source_code>& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    if (!src) {
        internal::raise_spec_error(
            &ec, 
            spec_error::source_code_not_found,
            lgr,
            "passed (std::shared_ptr<source_code>) src argument was nullptr!");
        return std::nullopt;
    }
    // get our TAUL language grammar
    grammar taul_gram = load_taul_grammar();
    TAUL_ASSERT(taul_gram.contains_ppr("Spec"));
    // tokenize input
    auto tkns = tokenize(taul_gram, src->str());
    // parse AST
    auto ast = taul_gram.parser("Spec")(ctx, tkns);
    TAUL_ASSERT((bool)ast);
    // traverse AST
    internal::compile_traverser ct{};
    ct.src = src;
    ct.ec = &ec;
    ct.lgr = lgr;
    ct.traverse(*ast);
    // if syntax or other error arose, fail and return nullopt
    if (!ct.success) {
        return std::nullopt;
    }
    // if successful, imbue w/ source_code association
    ct.output.associate(src);
    TAUL_ASSERT(ct.output.associated(src));
    return ct.output;
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::shared_ptr<source_code>& src, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::shared_ptr<source_code>& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::shared_ptr<source_code>& src, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::string& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    auto sc = std::make_shared<source_code>();
    sc->add_str("<src>", src);
    return compile(ctx, sc, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::string& src, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::string& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::string& src, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::filesystem::path& src_path, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    auto sc = std::make_shared<source_code>();
    bool success = sc->add_file(src_path);
    if (!success) {
        internal::raise_spec_error(
            &ec,
            spec_error::source_code_not_found,
            lgr,
            "failed to load source file at \"{}\"!",
            src_path.string());
        return std::nullopt;
    }
    return compile(ctx, sc, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::filesystem::path& src_path, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(ctx, src_path, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::filesystem::path& src_path, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    return compile(ctx, src_path, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::filesystem::path& src_path, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    spec_error_counter ec{};
    return compile(ctx, src_path, ec, lgr);
}

taul::internal::compile_traverser::compile_traverser() 
    : traverser() {}

void taul::internal::compile_traverser::on_begin() {
    swForDecls.grammar_bias(bias::f);
}

void taul::internal::compile_traverser::on_end() {
    auto spec0 = swForDecls.done();
    auto spec1 = swForDefs.done();
    output = spec::concat(spec0, spec1);
}

void taul::internal::compile_traverser::on_enter(const node& nd, bool& skip_children) {
    if (nd.is_lexical()) {
        //
    }
    else if (nd.is_syntactic()) {
        if (nd.ppr().name == "Spec_SyntaxError") {
            success = false;
            internal::raise_spec_error(
                ec, 
                spec_error::syntax_error, 
                lgr, 
                "syntax error at {}!", 
                *src->location_at(nd.pos()));
        }
        else if (nd.ppr().name == "Clause_LexerSection") {
            lexerSection = true;
        }
        else if (nd.ppr().name == "Clause_ParserSection") {
            lexerSection = false;
        }
        else if (nd.ppr().name == "Clause_Rule") {
            ruleName = "";
            ruleSkip = false;
        }
        else if (nd.ppr().name == "Clause_Rule_Skip") {
            ruleSkip = true;
        }
        else if (nd.ppr().name == "Clause_Rule_Name") {
            ruleName = nd.str();
        }
        else if (nd.ppr().name == "Clause_Rule_Expr") {
            if (lexerSection) {
                swForDecls.lpr_decl(ruleName);
                swForDefs.lpr(ruleName, ruleSkip ? qualifier::skip : qualifier::none);
            }
            else {
                swForDecls.ppr_decl(ruleName);
                swForDefs.ppr(ruleName, ruleSkip ? qualifier::skip : qualifier::none);
            }
        }
        else if (nd.ppr().name == "Expr_Begin") {
            swForDefs.begin();
        }
        else if (nd.ppr().name == "Expr_End") {
            swForDefs.end();
        }
        else if (nd.ppr().name == "Expr_Any") {
            swForDefs.any();
        }
        else if (nd.ppr().name == "Expr_Token") {
            swForDefs.token();
        }
        else if (nd.ppr().name == "Expr_Failure") {
            swForDefs.failure();
        }
        else if (nd.ppr().name == "Expr_String") {
            TAUL_ASSERT(nd.str().length() >= 2);
            swForDefs.string(nd.str().substr(1, nd.str().length() - 2));
        }
        else if (nd.ppr().name == "Expr_Charset") {
            TAUL_ASSERT(nd.str().length() >= 2);
            swForDefs.charset(nd.str().substr(1, nd.str().length() - 2));
        }
        else if (nd.ppr().name == "Expr_Name") {
            swForDefs.name(nd.str());
        }
        else if (nd.ppr().name == "Expr_Group") {
            //
        }
        else if (nd.ppr().name == "Expr_LookAhead") {
            swForDefs.assertion();
        }
        else if (nd.ppr().name == "Expr_LookAheadNot") {
            swForDefs.assertion(polarity::negative);
        }
        else if (nd.ppr().name == "Expr_Not") {
            swForDefs
                .constraint(polarity::negative)
                .any()
                .junction();
        }
        else if (nd.ppr().name == "Expr_Optional") {
            swForDefs.modifier(0, 1);
        }
        else if (nd.ppr().name == "Expr_KleeneStar") {
            swForDefs.modifier(0, 0);
        }
        else if (nd.ppr().name == "Expr_KleenePlus") {
            swForDefs.modifier(1, 0);
        }
        else if (nd.ppr().name == "Expr_Sequence") {
            swForDefs.sequence();
        }
        else if (nd.ppr().name == "Expr_Set") {
            swForDefs.set(bias::f);
        }
    }
    else TAUL_DEADEND;
}

void taul::internal::compile_traverser::on_exit(const taul::node& nd) {
    if (nd.is_lexical()) {
        //
    }
    else if (nd.is_syntactic()) {
        if (nd.ppr().name == "Clause_LexerSection") {
            lexerSection = true;
        }
        else if (nd.ppr().name == "Clause_ParserSection") {
            lexerSection = false;
        }
        else if (nd.ppr().name == "Clause_Rule_Expr") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_Group") {
            //
        }
        else if (nd.ppr().name == "Expr_LookAhead") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_LookAheadNot") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_Not") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_Optional") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_KleeneStar") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_KleenePlus") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_Sequence") {
            swForDefs.close();
        }
        else if (nd.ppr().name == "Expr_Set") {
            swForDefs.close();
        }
    }
    else TAUL_DEADEND;
}

