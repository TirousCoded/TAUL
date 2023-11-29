

#pragma once


#include "lexer.h"


namespace taul {


    // TODO: be sure to incorporate the below into our *official* documentation later

    // TODO: in our *official* documentation, be sure to include details about how tokenize(_into)
    //       functions have special behaviour regarding cutting 'skip tokens' for grammar-wide lexers

    // if a token during tokenization produced has length 0, then to avoid an infinite loop
    // (since the lexer will otherwise deterministically resolve the same token over-and-over)
    // the function will insert a failure token matching the next character into the output

    // if during the above the length 0 token is located at the end of the text, no failure token
    // will be inserted

    // if a match during tokenization matches a failure token, of any length, then a unit length 
    // failure token at the position of the failed match will be output, unless the failure 
    // occurs at the vary end of text, in which case no such insertion will occur

    // if multiple failure tokens created during tokenization are contiguous, they will be
    // concatenated into a single failure token, inheriting the position value of the first (as 
    // all such tokens are expected to exist contiguosly regarding their positions)


    void tokenize_into(
        std::vector<token>& target,
        const lexer& f,
        std::string_view txt,
        const std::shared_ptr<logger>& lgr = nullptr);

    // the 'offset' below refers to where in txt lexical analysis is to begin

    // behaviour is undefined if offset > txt.length()

    void tokenize_into(
        std::vector<token>& target,
        const lexer& f,
        std::string_view txt,
        source_pos offset,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::vector<token> tokenize(
        const lexer& f,
        std::string_view txt,
        const std::shared_ptr<logger>& lgr = nullptr);

    // the 'offset' below refers to where in txt lexical analysis is to begin

    // behaviour is undefined if offset > txt.length()

    std::vector<token> tokenize(
        const lexer& f,
        std::string_view txt,
        source_pos offset,
        const std::shared_ptr<logger>& lgr = nullptr);
}

