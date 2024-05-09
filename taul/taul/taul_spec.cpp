

#include "taul_spec.h"


using namespace taul::string_literals;


taul::spec taul::taul_spec() {
    return 
        spec_writer()
        .lpr_decl("END_OF_KW"_str)
        .lpr_decl("KW_LEXER"_str)
        .lpr_decl("KW_PARSER"_str)
        .lpr_decl("KW_SECTION"_str)
        .lpr_decl("KW_SKIP"_str)
        .lpr_decl("KW_SUPPORT"_str)
        .lpr_decl("KW_END"_str)
        .lpr_decl("KW_ANY"_str)
        .lpr_decl("KW_TOKEN"_str)
        .lpr_decl("KW_FAILURE"_str)
        .lpr_decl("OP_PERIOD"_str)
        .lpr_decl("OP_COLON"_str)
        .lpr_decl("OP_SEMICOLON"_str)
        .lpr_decl("OP_VBAR"_str)
        .lpr_decl("OP_QUESTION"_str)
        .lpr_decl("OP_ASTERISK"_str)
        .lpr_decl("OP_PLUS"_str)
        .lpr_decl("OP_AMPERSAND"_str)
        .lpr_decl("OP_MINUS"_str)
        .lpr_decl("OP_TILDE"_str)
        .lpr_decl("OP_L_ROUND"_str)
        .lpr_decl("OP_R_ROUND"_str)
        .lpr_decl("IDENTIFIER"_str)
        .lpr_decl("STRING"_str)
        .lpr_decl("CHARSET"_str)
        .lpr_decl("WHITESPACE"_str)
        .lpr_decl("NEWLINE"_str)
        .lpr_decl("COMMENT"_str)
        .ppr_decl("Spec"_str)
        .ppr_decl("Clause"_str)
        .ppr_decl("LexerSection"_str)
        .ppr_decl("ParserSection"_str)
        .ppr_decl("Rule"_str)
        .ppr_decl("Rule_Qualifiers"_str)
        .ppr_decl("Rule_Name"_str)
        .ppr_decl("Rule_Alts"_str)
        .ppr_decl("Qualifiers"_str)
        .ppr_decl("Qualifier"_str)
        .ppr_decl("Qualifier_Skip"_str)
        .ppr_decl("Qualifier_Support"_str)
        .ppr_decl("Expr"_str)
        .ppr_decl("Expr_NoSuffix"_str)
        .ppr_decl("Base"_str)
        .ppr_decl("Suffix"_str)
        .ppr_decl("Primary"_str)
        .ppr_decl("End"_str)
        .ppr_decl("Any"_str)
        .ppr_decl("Token"_str)
        .ppr_decl("Failure"_str)
        .ppr_decl("String"_str)
        .ppr_decl("Charset"_str)
        .ppr_decl("Name"_str)
        .ppr_decl("Sequence"_str)
        .ppr_decl("Sequence_Alts"_str)
        .ppr_decl("LookAhead"_str)
        .ppr_decl("LookAheadNot"_str)
        .ppr_decl("Not"_str)
        .ppr_decl("Optional_Suffix"_str)
        .ppr_decl("KleeneStar_Suffix"_str)
        .ppr_decl("KleenePlus_Suffix"_str)
        .ppr_decl("Alts"_str)
        .ppr_decl("Alt_Divider"_str)
        .ppr_decl("Alt"_str)
        .lpr("END_OF_KW"_str, taul::support)
        .lookahead_not()
        .charset("0-9a-zA-Z_"_str)
        .close()
        .alternative()
        .end()
        .close()
        .lpr("KW_LEXER"_str)
        .string("lexer"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_PARSER"_str)
        .string("parser"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_SECTION"_str)
        .string("section"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_SKIP"_str)
        .string("skip"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_SUPPORT"_str)
        .string("support"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_END"_str)
        .string("end"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_ANY"_str)
        .string("any"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_TOKEN"_str)
        .string("token"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("KW_FAILURE"_str)
        .string("failure"_str)
        .name("END_OF_KW"_str)
        .close()
        .lpr("OP_PERIOD"_str)
        .string("."_str)
        .close()
        .lpr("OP_COLON"_str)
        .string(":"_str)
        .close()
        .lpr("OP_SEMICOLON"_str)
        .string(";"_str)
        .close()
        .lpr("OP_VBAR"_str)
        .string("|"_str)
        .close()
        .lpr("OP_QUESTION"_str)
        .string("?"_str)
        .close()
        .lpr("OP_ASTERISK"_str)
        .string("*"_str)
        .close()
        .lpr("OP_PLUS"_str)
        .string("+"_str)
        .close()
        .lpr("OP_AMPERSAND"_str)
        .string("&"_str)
        .close()
        .lpr("OP_MINUS"_str)
        .string("-"_str)
        .close()
        .lpr("OP_TILDE"_str)
        .string("~"_str)
        .close()
        .lpr("OP_L_ROUND"_str)
        .string("("_str)
        .close()
        .lpr("OP_R_ROUND"_str)
        .string(")"_str)
        .close()
        .lpr("IDENTIFIER"_str)
        .charset("a-zA-Z_"_str)
        .kleene_star()
        .charset("0-9a-zA-Z_"_str)
        .close()
        .close()
        .lpr("STRING"_str)
        .string("'"_str)
        .kleene_star()
        .sequence()
        .string("\\\\"_str)
        .any()
        .alternative()
        .not0()
        .charset("\\'\\\\"_str)
        .close()
        .close()
        .close()
        .optional()
        .string("'"_str)
        .close()
        .close()
        .lpr("CHARSET"_str)
        .string("["_str)
        .kleene_star()
        .sequence()
        .string("\\\\"_str)
        .any()
        .alternative()
        .not0()
        .charset("\\]\\\\"_str)
        .close()
        .close()
        .close()
        .optional()
        .string("]"_str)
        .close()
        .close()
        .lpr("WHITESPACE"_str, taul::skip)
        .kleene_plus()
        .charset(" \t"_str)
        .close()
        .close()
        .lpr("NEWLINE"_str, taul::skip)
        .string("\n"_str)
        .alternative()
        .string("\r"_str)
        .optional()
        .string("\n"_str)
        .close()
        .close()
        .lpr("COMMENT"_str, taul::skip)
        .string("#"_str)
        .kleene_star()
        .not0()
        .charset("\r\n"_str)
        .close()
        .close()
        .close()
        .ppr("Spec"_str)
        .kleene_star()
        .name("Clause"_str)
        .close()
        .end()
        .close()
        .ppr("Clause"_str)
        .name("LexerSection"_str)
        .alternative()
        .name("ParserSection"_str)
        .alternative()
        .name("Rule"_str)
        .close()
        .ppr("LexerSection"_str)
        .name("KW_LEXER"_str)
        .name("KW_SECTION"_str)
        .name("OP_COLON"_str)
        .close()
        .ppr("ParserSection"_str)
        .name("KW_PARSER"_str)
        .name("KW_SECTION"_str)
        .name("OP_COLON"_str)
        .close()
        .ppr("Rule"_str)
        .name("Rule_Qualifiers"_str)
        .name("Rule_Name"_str)
        .name("OP_COLON"_str)
        .name("Rule_Alts"_str)
        .name("OP_SEMICOLON"_str)
        .close()
        .ppr("Rule_Qualifiers"_str)
        .name("Qualifiers"_str)
        .close()
        .ppr("Rule_Name"_str)
        .name("IDENTIFIER"_str)
        .close()
        .ppr("Rule_Alts"_str)
        .name("Alts"_str)
        .close()
        .ppr("Qualifiers"_str)
        .kleene_star()
        .name("Qualifier"_str)
        .close()
        .close()
        .ppr("Qualifier"_str)
        .name("Qualifier_Skip"_str)
        .alternative()
        .name("Qualifier_Support"_str)
        .close()
        .ppr("Qualifier_Skip"_str)
        .name("KW_SKIP"_str)
        .close()
        .ppr("Qualifier_Support"_str)
        .name("KW_SUPPORT"_str)
        .close()
        .ppr("Expr"_str)
        .name("Base"_str)
        .kleene_star()
        .name("Suffix"_str)
        .close()
        .close()
        .ppr("Expr_NoSuffix"_str)
        .name("Base"_str)
        .close()
        .ppr("Base"_str)
        .name("Primary"_str)
        .alternative()
        .name("Sequence"_str)
        .alternative()
        .name("LookAhead"_str)
        .alternative()
        .name("LookAheadNot"_str)
        .alternative()
        .name("Not"_str)
        .close()
        .ppr("Suffix"_str)
        .name("Optional_Suffix"_str)
        .alternative()
        .name("KleeneStar_Suffix"_str)
        .alternative()
        .name("KleenePlus_Suffix"_str)
        .close()
        .ppr("Primary"_str)
        .name("End"_str)
        .alternative()
        .name("Any"_str)
        .alternative()
        .name("Token"_str)
        .alternative()
        .name("Failure"_str)
        .alternative()
        .name("String"_str)
        .alternative()
        .name("Charset"_str)
        .alternative()
        .name("Name"_str)
        .close()
        .ppr("End"_str)
        .name("KW_END"_str)
        .close()
        .ppr("Any"_str)
        .name("KW_ANY"_str)
        .close()
        .ppr("Token"_str)
        .name("KW_TOKEN"_str)
        .close()
        .ppr("Failure"_str)
        .name("KW_FAILURE"_str)
        .close()
        .ppr("String"_str)
        .name("STRING"_str)
        .close()
        .ppr("Charset"_str)
        .name("CHARSET"_str)
        .close()
        .ppr("Name"_str)
        .name("IDENTIFIER"_str)
        .close()
        .ppr("Sequence"_str)
        .name("OP_L_ROUND"_str)
        .name("Sequence_Alts"_str)
        .name("OP_R_ROUND"_str)
        .close()
        .ppr("Sequence_Alts"_str)
        .name("Alts"_str)
        .close()
        .ppr("LookAhead"_str)
        .name("OP_AMPERSAND"_str)
        .name("Expr_NoSuffix"_str)
        .close()
        .ppr("LookAheadNot"_str)
        .name("OP_MINUS"_str)
        .name("Expr_NoSuffix"_str)
        .close()
        .ppr("Not"_str)
        .name("OP_TILDE"_str)
        .name("Expr_NoSuffix"_str)
        .close()
        .ppr("Optional_Suffix"_str)
        .name("OP_QUESTION"_str)
        .close()
        .ppr("KleeneStar_Suffix"_str)
        .name("OP_ASTERISK"_str)
        .close()
        .ppr("KleenePlus_Suffix"_str)
        .name("OP_PLUS"_str)
        .close()
        .ppr("Alts"_str)
        .name("Alt"_str)
        .kleene_star()
        .sequence()
        .name("Alt_Divider"_str)
        .name("Alt"_str)
        .close()
        .close()
        .close()
        .ppr("Alt_Divider"_str)
        .name("OP_VBAR"_str)
        .close()
        .ppr("Alt"_str)
        .kleene_star()
        .name("Expr"_str)
        .close()
        .close()
        .done();
}
