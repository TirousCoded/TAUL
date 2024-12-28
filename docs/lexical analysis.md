# Lexical Analysis

This document details TAUL *lexical analysis* semantics.

## Basics

Lexical analysis in TAUL operates according to the usual *LL(1) parsing* and
*pipeline* semantics.

Lexing is performed by attempting each LPR in the grammar, stopping if one of them
matches successfully, and using said result as the result for that round of lexical
analysis.

The order in which LPRs are performed is the order in which they appear in the spec.

LPRs with the `support` qualifier are *excluded* from this traversal of the LPRs
of the grammar.

```
lexer section:

    # lexical analysis will only involve ABC and DEF; HELPER will be excluded

    ABC             : 'a' HELPER 'c' ;
    DEF             : 'def' ;

    support HELPER  : 'b' ;
```

The act of traversal stopping upon finding a successful match causes the earlier
LPRs to *shadow* the later ones, thus implementing TAUL's notion of LPR *shadowing*.

```
lexer section:

    # keywords
    ABC         : 'abc' ;
    DEF         : 'def' ;
    GHI         : 'ghi' ;

    # IDENTIFIER can match any input matched by ABC, DEF or GHI, but them appearing
    # first means they get priority and will *shadow* IDENTIFIER, meaning they will
    # get matched instead of it

    IDENTIFIER  : [a-zA-Z_][0-9a-zA-Z_]* ;
```

## Skip Tokens

If the lexer matches a token using a LPR with the `skip` qualifier, the token is
marked as a *skip token*.

Skip tokens are *by default* excluded from the lexer's output. The lexer will keep
performing tokenization until it finds a non-skip token which it can use as the
output for that round of lexical analysis.

```
lexer section:

    A       : 'a' ;
    B       : 'b' ;
    C       : 'c' ;

    # WS matches whitespace, and being *skip* qualified, WS tokens will be discarded
    # rather than output, allowing end-user to reason about A, B and C token sequences,
    # w/out having to think about any whitespace in between

    skip WS : [ \t]+ ;
```

A token does not get marked as a skip token unless the LPR which it is associated
with was the one called directly at the top-level by the lexer, not any called
indirectly therein.

```
lexer section:

    # the WS reference in AC *does not* result in skip tokens, instead just acting
    # as a component of AC's pattern, w/ AC tokens having no skip token semantics

    AC      : 'a' WS 'c' ;
	
    # outside of the above WS reference in AC, WS operates as normal, producing skip
    # tokens which get discarded

    skip WS : [ \t]+ ;
```

Skip tokens will be observed the *observer* if any, attached to the lexer, regardless
of whether skip tokens are kept or discarded.

## End-Of-Input Tokens

Upon exhausting its upstream source, the lexer will output *end-of-input tokens*
thereafter, and will *no longer advance*.

## Failure Tokens

If the lexer is unable to find a token to match against a portion of input, and the
upstream source has not been exhausted, the lexer will generate a *failure token* to
be its output.

Upon generating the failure token, it will *start off unit-length*, and will then be
*extended* if the lexer detects that the next round of lexical analysis would result in
another failure token, repeating this process of extension until either the upstream
source is exhausted, or until the lexer detects that the next token will not be a
failure token.

## Zero-Length Tokens

If the lexer matches a *token of zero-length* which is *not an end-of-input token*,
the next round of lexical analysis will, up-front, behave as though it failed lexical
analysis, in order to avoid what would otherwise be a *deterministic infinite loop*.