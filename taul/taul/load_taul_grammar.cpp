

#include "load_taul_grammar.h"

#include "spec.h"
#include "load.h"


taul::grammar taul::load_taul_grammar(const std::shared_ptr<logger>& lgr) {
    spec s =
        spec_writer()
        .lpr_decl("VIS_ASCII")
        .lpr_decl("LETTER")
        .lpr_decl("DIGIT")
        .lpr_decl("ANY_IDENTIFIER")
        .lpr_decl("IDENTIFIER")
        .lpr_decl("INTEGER")
        .lpr_decl("STRING")
        .lpr_decl("CHARSET")
        .lpr_decl("WHITESPACE")
        .lpr_decl("NEWLINE")
        .lpr_decl("SL_COMMENT")
        .lpr_decl("ML_COMMENT")
        .lpr_decl("KEYWORD")
        .lpr_decl("EOK")
        .lpr_decl("KW_OPTIONS")
        .lpr_decl("KW_LEXER")
        .lpr_decl("KW_PARSER")
        .lpr_decl("KW_BIAS")
        .lpr_decl("KW_SKIP")
        .lpr_decl("KW_SUPPORT")
        .lpr_decl("KW_FL")
        .lpr_decl("KW_FS")
        .lpr_decl("KW_LL")
        .lpr_decl("KW_LS")
        .lpr_decl("KW_F")
        .lpr_decl("KW_L")
        .lpr_decl("KW_BEGIN")
        .lpr_decl("KW_END")
        .lpr_decl("KW_ANY")
        .lpr_decl("KW_TOKEN")
        .lpr_decl("KW_FAILURE")
        .lpr_decl("KW_LOCALEND")
        .lpr_decl("OPERATORS") // this is currently unused
        .lpr_decl("OP_PERIOD")
        .lpr_decl("OP_COMMA")
        .lpr_decl("OP_COLON")
        .lpr_decl("OP_SEMICOLON")
        .lpr_decl("OP_VBAR")
        .lpr_decl("OP_QUESTION")
        .lpr_decl("OP_ASTERISK")
        .lpr_decl("OP_PLUS")
        .lpr_decl("OP_EXCLAM")
        .lpr_decl("OP_TILDE")
        .lpr_decl("OP_AMPERSAND")
        .lpr_decl("OP_MINUS")
        .lpr_decl("OP_L_ROUND")
        .lpr_decl("OP_R_ROUND")
        .lpr_decl("OP_L_CURLY")
        .lpr_decl("OP_R_CURLY")
        .lpr("VIS_ASCII", qualifier::support)
        .range('!', '~')
        .close()
        .lpr("LETTER", qualifier::support)
        .set()
        .range('a', 'z')
        .range('A', 'Z')
        .close()
        .close()
        .lpr("DIGIT", qualifier::support)
        .range('0', '9')
        .close()
        .lpr("ANY_IDENTIFIER", qualifier::support)
        .set()
        .name("LETTER")
        .string("_")
        .close()
        .modifier(0, 0)
        .set()
        .name("DIGIT")
        .name("LETTER")
        .string("_")
        .close()
        .close()
        .close()
        .lpr("IDENTIFIER")
        .constraint(polarity::negative)
        .name("ANY_IDENTIFIER")
        .junction()
        .name("KEYWORD")
        .close()
        .close()
        .lpr("INTEGER")
        .name("DIGIT")
        .modifier(0, 0)
        .sequence()
        .modifier(0, 1)
        .string("_")
        .close()
        .name("DIGIT")
        .close()
        .close()
        .assertion(polarity::negative)
        .set()
        .name("LETTER")
        .string("_")
        .close()
        .close()
        .close()
        .lpr("STRING")
        .string("'")
        .modifier(0, 0)
        .set()
        .sequence()
        .modifier(0, 1)
        .string("\\")
        .close()
        .constraint(polarity::negative)
        .set()
        .name("VIS_ASCII")
        .name("WHITESPACE")
        .name("NEWLINE")
        .close()
        .junction()
        .string("'")
        .close()
        .close()
        .string("\\\\")
        .string("\\'")
        .close()
        .close()
        .string("'")
        .close()
        .lpr("CHARSET")
        .string("[")
        .modifier(0, 0)
        .set()
        .sequence()
        .modifier(0, 1)
        .string("\\")
        .close()
        .constraint(polarity::negative)
        .set()
        .name("VIS_ASCII")
        .name("WHITESPACE")
        .name("NEWLINE")
        .close()
        .junction()
        .string("]")
        .close()
        .close()
        .string("\\\\")
        .string("\\]")
        .close()
        .close()
        .string("]")
        .close()
        .lpr("WHITESPACE", qualifier::skip)
        .charset(" \t")
        .close()
        .lpr("NEWLINE", qualifier::skip)
        .set()
        .charset("\r\n")
        .string("\r\n")
        .close()
        .close()
        .lpr("SL_COMMENT", qualifier::skip)
        .string("#")
        .modifier(0, 0)
        .constraint(polarity::negative)
        .any()
        .junction()
        .name("NEWLINE")
        .close()
        .close()
        .close()
        .lpr("ML_COMMENT", qualifier::skip)
        .string("!#")
        .modifier(0, 0)
        .constraint(polarity::negative)
        .any()
        .junction()
        .string("#!")
        .close()
        .close()
        .modifier(0, 1)
        .string("#!")
        .close()
        .close()
        .lpr("KEYWORD", qualifier::support)
        .set()
        .name("KW_OPTIONS")
        .name("KW_LEXER")
        .name("KW_PARSER")
        .name("KW_BIAS")
        .name("KW_SKIP")
        .name("KW_SUPPORT")
        .name("KW_FL")
        .name("KW_FS")
        .name("KW_LL")
        .name("KW_LS")
        .name("KW_F")
        .name("KW_L")
        .name("KW_BEGIN")
        .name("KW_END")
        .name("KW_ANY")
        .name("KW_TOKEN")
        .name("KW_FAILURE")
        .name("KW_LOCALEND")
        .close()
        .close()
        .lpr("EOK", qualifier::support)
        .assertion(polarity::negative)
        .set()
        .name("DIGIT")
        .name("LETTER")
        .string("_")
        .close()
        .close()
        .close()
        .lpr("KW_OPTIONS")
        .string("options")
        .name("EOK")
        .close()
        .lpr("KW_LEXER")
        .string("lexer")
        .name("EOK")
        .close()
        .lpr("KW_PARSER")
        .string("parser")
        .name("EOK")
        .close()
        .lpr("KW_BIAS")
        .string("bias")
        .name("EOK")
        .close()
        .lpr("KW_SKIP")
        .string("skip")
        .name("EOK")
        .close()
        .lpr("KW_SUPPORT")
        .string("support")
        .name("EOK")
        .close()
        .lpr("KW_FL")
        .string("fl")
        .name("EOK")
        .close()
        .lpr("KW_FS")
        .string("fs")
        .name("EOK")
        .close()
        .lpr("KW_LL")
        .string("ll")
        .name("EOK")
        .close()
        .lpr("KW_LS")
        .string("ls")
        .name("EOK")
        .close()
        .lpr("KW_F")
        .string("f")
        .name("EOK")
        .close()
        .lpr("KW_L")
        .string("l")
        .name("EOK")
        .close()
        .lpr("KW_BEGIN")
        .string("begin")
        .name("EOK")
        .close()
        .lpr("KW_END")
        .string("end")
        .name("EOK")
        .close()
        .lpr("KW_ANY")
        .string("any")
        .name("EOK")
        .close()
        .lpr("KW_TOKEN")
        .string("token")
        .name("EOK")
        .close()
        .lpr("KW_FAILURE")
        .string("failure")
        .name("EOK")
        .close()
        .lpr("KW_LOCALEND")
        .string("localend")
        .name("EOK")
        .close()
        .lpr("OPERATORS", qualifier::support)
        .set()
        .name("OP_PERIOD")
        .name("OP_COMMA")
        .name("OP_COLON")
        .name("OP_SEMICOLON")
        .name("OP_VBAR")
        .name("OP_QUESTION")
        .name("OP_ASTERISK")
        .name("OP_PLUS")
        .name("OP_EXCLAM")
        .name("OP_TILDE")
        .name("OP_AMPERSAND")
        .name("OP_MINUS")
        .name("OP_L_ROUND")
        .name("OP_R_ROUND")
        .name("OP_L_CURLY")
        .name("OP_R_CURLY")
        .close()
        .close()
        .lpr("OP_PERIOD")
        .string(".")
        .close()
        .lpr("OP_COMMA")
        .string(",")
        .close()
        .lpr("OP_COLON")
        .string(":")
        .close()
        .lpr("OP_SEMICOLON")
        .string(";")
        .close()
        .lpr("OP_VBAR")
        .string("|")
        .close()
        .lpr("OP_QUESTION")
        .string("?")
        .close()
        .lpr("OP_ASTERISK")
        .string("*")
        .close()
        .lpr("OP_PLUS")
        .string("+")
        .close()
        .lpr("OP_EXCLAM")
        .string("!")
        .close()
        .lpr("OP_TILDE")
        .string("~")
        .close()
        .lpr("OP_AMPERSAND")
        .string("&")
        .close()
        .lpr("OP_MINUS")
        .string("-")
        .close()
        .lpr("OP_L_ROUND")
        .string("(")
        .close()
        .lpr("OP_R_ROUND")
        .string(")")
        .close()
        .lpr("OP_L_CURLY")
        .string("{")
        .close()
        .lpr("OP_R_CURLY")
        .string("}")
        .close()
        .done();
    auto ld = load(s, lgr);
    TAUL_ASSERT(ld);
    return std::move(*ld);
}

