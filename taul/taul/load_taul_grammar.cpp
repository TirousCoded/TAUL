

#include "load_taul_grammar.h"

#include "spec.h"
#include "load.h"


taul::grammar taul::load_taul_grammar(const std::shared_ptr<logger>& lgr) {
    spec_writer sw;
    sw
        .grammar_bias(bias::f); // need this for LL(1) semantics
    sw
        .lpr_decl("KW_LEXER")
        .lpr_decl("KW_PARSER")
        .lpr_decl("KW_SECTION")
        .lpr_decl("KW_SKIP")
        .lpr_decl("KW_BEGIN")
        .lpr_decl("KW_END")
        .lpr_decl("KW_ANY")
        .lpr_decl("KW_TOKEN")
        .lpr_decl("KW_FAILURE");
    sw
        .lpr_decl("OP_PERIOD")
        .lpr_decl("OP_COMMA")
        .lpr_decl("OP_COLON")
        .lpr_decl("OP_SEMICOLON")
        .lpr_decl("OP_VBAR")
        .lpr_decl("OP_QUESTION")
        .lpr_decl("OP_ASTERISK")
        .lpr_decl("OP_PLUS")
        .lpr_decl("OP_TILDE")
        .lpr_decl("OP_AMPERSAND")
        .lpr_decl("OP_MINUS")
        .lpr_decl("OP_L_ROUND")
        .lpr_decl("OP_R_ROUND");
    sw
        .lpr_decl("IDENTIFIER")
        .lpr_decl("STRING")
        .lpr_decl("CHARSET")
        .lpr_decl("WHITESPACE")
        .lpr_decl("NEWLINE")
        .lpr_decl("SL_COMMENT")
        .lpr_decl("ML_COMMENT");
    sw
        .ppr_decl("Spec")
        .ppr_decl("Spec_SyntaxError");
    sw
        .ppr_decl("Clause")
        .ppr_decl("Clause_LexerSection")
        .ppr_decl("Clause_ParserSection")
        .ppr_decl("Clause_Rule")
        .ppr_decl("Clause_Rule_Skip")
        .ppr_decl("Clause_Rule_Name")
        .ppr_decl("Clause_Rule_Expr");
    sw
        .ppr_decl("Expr")
        .ppr_decl("Expr_Primary")
        .ppr_decl("Expr_Begin")
        .ppr_decl("Expr_End")
        .ppr_decl("Expr_Any")
        .ppr_decl("Expr_Token")
        .ppr_decl("Expr_Failure")
        .ppr_decl("Expr_String")
        .ppr_decl("Expr_Charset")
        .ppr_decl("Expr_Name")
        .ppr_decl("Expr_Group")
        .ppr_decl("Expr_LookAhead")
        .ppr_decl("Expr_LookAheadNot")
        .ppr_decl("Expr_Not")
        .ppr_decl("Expr_Optional")
        .ppr_decl("Expr_Optional_Expr")
        .ppr_decl("Expr_KleeneStar")
        .ppr_decl("Expr_KleeneStar_Expr")
        .ppr_decl("Expr_KleenePlus")
        .ppr_decl("Expr_KleenePlus_Expr")
        .ppr_decl("Expr_Sequence")
        .ppr_decl("Expr_Sequence_Expr")
        .ppr_decl("Expr_Set")
        .ppr_decl("Expr_Set_Expr");
    auto add_keyword_lpr =
        [&](const char* kwName, const char* s) {
        sw
            .lpr(kwName)
            .string(s)
            //-[0-9a-zA-Z_]
            .assertion(polarity::negative)
            .set()
            .range('0', '9')
            .range('a', 'z')
            .range('A', 'Z')
            .string("_")
            .close()
            .close()
            .close();
        };
    add_keyword_lpr("KW_LEXER", "lexer");
    add_keyword_lpr("KW_PARSER", "parser");
    add_keyword_lpr("KW_SECTION", "section");
    add_keyword_lpr("KW_SKIP", "skip");
    add_keyword_lpr("KW_BEGIN", "begin");
    add_keyword_lpr("KW_END", "end");
    add_keyword_lpr("KW_ANY", "any");
    add_keyword_lpr("KW_TOKEN", "token");
    add_keyword_lpr("KW_FAILURE", "failure");
    auto add_operator_lpr =
        [&](const char* opName, const char* s) {
        sw
            .lpr(opName)
            .string(s)
            .close();
        };
    add_operator_lpr("OP_PERIOD", ".");
    add_operator_lpr("OP_COMMA", ",");
    add_operator_lpr("OP_COLON", ":");
    add_operator_lpr("OP_SEMICOLON", ";");
    add_operator_lpr("OP_VBAR", "|");
    add_operator_lpr("OP_QUESTION", "?");
    add_operator_lpr("OP_ASTERISK", "*");
    add_operator_lpr("OP_PLUS", "+");
    add_operator_lpr("OP_TILDE", "~");
    add_operator_lpr("OP_AMPERSAND", "&");
    add_operator_lpr("OP_MINUS", "-");
    add_operator_lpr("OP_L_ROUND", "(");
    add_operator_lpr("OP_R_ROUND", ")");
    sw
        .lpr("IDENTIFIER")
        // [a-zA-Z_] [0-9a-zA-Z_]*
        .set()
        .range('a', 'z')
        .range('A', 'Z')
        .string("_")
        .close()
        .modifier(0, 0)
        .set()
        .range('0', '9')
        .range('a', 'z')
        .range('A', 'Z')
        .string("_")
        .close()
        .close()
        .close();
    sw
        .lpr("STRING")
        // '\'' ( ~'\'' | '\\' any )* '\''
        .string("'")
        .modifier(0, 0)
        .set()
        // ~'\''
        .sequence()
        .assertion(polarity::negative)
        .string("'")
        .close()
        .any()
        .close()
        // '\\' any
        .sequence()
        .string("\\")
        .any()
        .close()
        .close()
        .close()
        .string("'")
        .close();
    sw
        .lpr("CHARSET")
        // '[' ( ~']' | '\\' any )* ']'
        .string("[")
        .modifier(0, 0)
        .set()
        // ~']'
        .sequence()
        .assertion(polarity::negative)
        .string("]")
        .close()
        .any()
        .close()
        // '\\' any
        .sequence()
        .string("\\")
        .any()
        .close()
        .close()
        .close()
        .string("]")
        .close();
    sw
        .lpr("WHITESPACE", qualifier::skip)
        // [ \t]
        .charset(" \t")
        .close();
    sw
        .lpr("NEWLINE", qualifier::skip)
        // '\r\n' | [\r\n]
        .set(bias::f)
        .string("\r\n")
        .charset("\r\n")
        .close()
        .close();
    sw
        .lpr("SL_COMMENT", qualifier::skip)
        // '#' ~[\r\n]*
        .string("#")
        .modifier(0, 0)
        // ~[\r\n]
        .sequence()
        .assertion(polarity::negative)
        .charset("\r\n")
        .close()
        .any()
        .close()
        .close()
        .close();
    sw
        .lpr("ML_COMMENT", qualifier::skip)
        // '!#' ( ~'#' | '#' -'!' )* '#!'?
        .string("!#")
        .modifier(0, 0)
        .set(bias::f)
        // ~'#'
        .sequence()
        .assertion(polarity::negative)
        .string("#")
        .close()
        .any()
        .close()
        // '#' -'!'
        .sequence()
        .string("#")
        .assertion(polarity::negative)
        .string("!")
        .close()
        .close()
        .close()
        .close()
        .modifier(0, 1)
        .string("#!")
        .close()
        .close();
    sw
        .ppr("Spec")
        // begin Clause* ( end | Spec_SyntaxError )
        .begin()
        .modifier(0, 0)
        .name("Clause")
        .close()
        .set(bias::f)
        .end()
        .name("Spec_SyntaxError")
        .close()
        .close();
    sw
        .ppr("Spec_SyntaxError")
        // any
        .any()
        .close();
    sw
        .ppr("Clause")
        .set(bias::f)
        .name("Clause_LexerSection")
        .name("Clause_ParserSection")
        .name("Clause_Rule")
        .close()
        .close();
    sw
        .ppr("Clause_LexerSection")
        // KW_LEXER KW_SECTION OP_COLON
        .name("KW_LEXER")
        .name("KW_SECTION")
        .name("OP_COLON")
        .close();
    sw
        .ppr("Clause_ParserSection")
        // KW_PARSER KW_SECTION OP_COLON
        .name("KW_PARSER")
        .name("KW_SECTION")
        .name("OP_COLON")
        .close();
    sw
        .ppr("Clause_Rule")
        // Clause_Rule_Skip? Clause_Rule_Name OP_COLON Clause_Rule_Expr OP_SEMICOLON
        .modifier(0, 1)
        .name("Clause_Rule_Skip")
        .close()
        .name("Clause_Rule_Name")
        .name("OP_COLON")
        .name("Clause_Rule_Expr")
        .name("OP_SEMICOLON")
        .close();
    sw
        .ppr("Clause_Rule_Skip")
        // KW_SKIP
        .name("KW_SKIP")
        .close();
    sw
        .ppr("Clause_Rule_Name")
        // IDENTIFIER
        .name("IDENTIFIER")
        .close();
    sw
        .ppr("Clause_Rule_Expr")
        // Expr
        .name("Expr")
        .close();
    sw
        .ppr("Expr")
        .set(bias::f)
        .name("Expr_Set")
        .name("Expr_Sequence")
        .name("Expr_KleenePlus")
        .name("Expr_KleeneStar")
        .name("Expr_Optional")
        .name("Expr_Not")
        .name("Expr_LookAheadNot")
        .name("Expr_LookAhead")
        .name("Expr_Group")
        .name("Expr_Primary")
        .close()
        .close();
    sw
        .ppr("Expr_Primary")
        .set(bias::f)
        .name("Expr_Begin")
        .name("Expr_End")
        .name("Expr_Any")
        .name("Expr_Token")
        .name("Expr_Failure")
        .name("Expr_String")
        .name("Expr_Charset")
        .name("Expr_Name")
        .close()
        .close();
    sw
        .ppr("Expr_Begin")
        .name("KW_BEGIN")
        .close();
    sw
        .ppr("Expr_End")
        .name("KW_END")
        .close();
    sw
        .ppr("Expr_Any")
        .name("KW_ANY")
        .close();
    sw
        .ppr("Expr_Token")
        .name("KW_TOKEN")
        .close();
    sw
        .ppr("Expr_Failure")
        .name("KW_FAILURE")
        .close();
    sw
        .ppr("Expr_String")
        .name("STRING")
        .close();
    sw
        .ppr("Expr_Charset")
        .name("CHARSET")
        .close();
    sw
        .ppr("Expr_Name")
        .name("IDENTIFIER")
        .close();
    sw
        .ppr("Expr_Group")
        // OP_L_ROUND Expr OP_R_ROUND
        .name("OP_L_ROUND")
        .name("Expr")
        .name("OP_R_ROUND")
        .close();
    sw
        .ppr("Expr_LookAhead")
        // OP_AMPERSAND Expr
        .name("OP_AMPERSAND")
        .name("Expr")
        .close();
    sw
        .ppr("Expr_LookAheadNot")
        // OP_MINUS Expr
        .name("OP_MINUS")
        .name("Expr")
        .close();
    sw
        .ppr("Expr_Not")
        // OP_TILDE Expr
        .name("OP_TILDE")
        .name("Expr")
        .close();
    sw
        .ppr("Expr_Optional")
        // Expr_Optional_Expr OP_QUESTION
        .name("Expr_Optional_Expr")
        .name("OP_QUESTION")
        .close();
    sw
        .ppr("Expr_Optional_Expr")
        .set(bias::f)
        .name("Expr_Not")
        .name("Expr_LookAheadNot")
        .name("Expr_LookAhead")
        .name("Expr_Group")
        .name("Expr_Primary")
        .close()
        .close();
    sw
        .ppr("Expr_KleeneStar")
        // Expr_KleeneStar_Expr OP_ASTERISK
        .name("Expr_KleeneStar_Expr")
        .name("OP_ASTERISK")
        .close();
    sw
        .ppr("Expr_KleeneStar_Expr")
        .set(bias::f)
        .name("Expr_Optional")
        .name("Expr_Not")
        .name("Expr_LookAheadNot")
        .name("Expr_LookAhead")
        .name("Expr_Group")
        .name("Expr_Primary")
        .close()
        .close();
    sw
        .ppr("Expr_KleenePlus")
        // Expr_KleenePlus_Expr OP_PLUS
        .name("Expr_KleenePlus_Expr")
        .name("OP_PLUS")
        .close();
    sw
        .ppr("Expr_KleenePlus_Expr")
        .set(bias::f)
        .name("Expr_KleeneStar")
        .name("Expr_Optional")
        .name("Expr_Not")
        .name("Expr_LookAheadNot")
        .name("Expr_LookAhead")
        .name("Expr_Group")
        .name("Expr_Primary")
        .close()
        .close();
    sw
        .ppr("Expr_Sequence")
        // Expr_Sequence_Expr Expr_Sequence_Expr+
        .name("Expr_Sequence_Expr")
        .modifier(1, 0)
        .name("Expr_Sequence_Expr")
        .close()
        .close();
    sw
        .ppr("Expr_Sequence_Expr")
        .set(bias::f)
        .name("Expr_KleenePlus")
        .name("Expr_KleeneStar")
        .name("Expr_Optional")
        .name("Expr_Not")
        .name("Expr_LookAheadNot")
        .name("Expr_LookAhead")
        .name("Expr_Group")
        .name("Expr_Primary")
        .close()
        .close();
    sw
        .ppr("Expr_Set")
        // Expr_Set_Expr ( OP_VBAR Expr_Set_Expr )+
        .name("Expr_Set_Expr")
        .modifier(1, 0)
        .sequence()
        .name("OP_VBAR")
        .name("Expr_Set_Expr")
        .close()
        .close()
        .close();
    sw
        .ppr("Expr_Set_Expr")
        .set(bias::f)
        .name("Expr_Sequence")
        .name("Expr_KleenePlus")
        .name("Expr_KleeneStar")
        .name("Expr_Optional")
        .name("Expr_Not")
        .name("Expr_LookAheadNot")
        .name("Expr_LookAhead")
        .name("Expr_Group")
        .name("Expr_Primary")
        .close()
        .close();
    auto ld = load(sw.done(), lgr);
    TAUL_ASSERT(ld);
    return std::move(*ld);
}

