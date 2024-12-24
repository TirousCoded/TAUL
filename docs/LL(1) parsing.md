# LL(1) Parsing

TAUL is an *LL(1) parser*, meaning it is a *top-down* *left-derivative* parser which does not *backtrack*, and which is not *recursive*.

See [this](https://en.wikipedia.org/wiki/LL_parser#Parser), [this](https://www.geeksforgeeks.org/construction-of-ll1-parsing-table/),
and [this](https://www.naukri.com/code360/library/predictive-parsing) for a detailed explanation of the nature of LL(1) parsing
(and *predictive parsing* more broadly.) Take note that those new to *formal language theory* (ie. the math behind parsing) may have
to do more thorough research into the topic before these and other explanations actually start to *make sense*.

Taking the wikipedia article linked above as a base, this document aims to explain how TAUL's implementation of LL(1) parsing
differs from the model described therein.

This document is not a tutorial for LL(1) parsing.

## FIRST/FOLLOW Sets

*FIRST sets* is defined as described in the wikipedia article.

*FOLLOW sets* differ from the wikipedia article's description, which proscribes that FOLLOW sets start off with only the *end-of-input*,
and are then populated by symbols which *follow* instances of the non-terminal as it appears in the grammar.

By contrast, given some non-terminal *A*, *FOLLOW(A)* in TAUL is defined as *the set of all terminal symbols, except those in FIRST(A)*.

The subtraction of *FIRST(A)* is needed to prevent *ambiguity*.

This alternate definition allows for *any* non-terminal to be the *start rule*.

This alternate definition is also *required* for TAUL's lexer to work. Tokens *can* be followed by an end-of-input, but they can also
be followed by *any arbitrary symbol*, meaning for it to work the FOLLOW set must be *liberally populated* as described above.

## Prefix Sets

Alongside FIRST/FOLLOW sets, TAUL production rules also define a *prefix set*.

Prefix sets are defined as the set of symbols which are actually used to construct the parse table:

- If the FIRST set *does not* contain *epsilon*, then the prefix set is *identical to the FIRST set*.

- If the FIRST set *does* contain *epsilon*, then the prefix set is the *union of the FIRST and FOLLOW sets*.

## The Parse Stack

When the *parse stack* is being initialized (be it in the context of lexing or parsing) the *end-of-input*
terminal is NOT pushed ahead of the *start rule*, in fact, it's not pushed *at all*.

There are two reasons for this:

- Pushing the end-of-input doesn't work with how TAUL's lexer works, for reasons akin to the above details about changes TAUL makes
to how FOLLOW sets are defined.

- Matching the end-of-input *after* the start rule is completely matched doesn't work with how TAUL operates regarding how `taul::listener`
reports events, as doing so would require it to report the end-of-input terminal outside of the *opening/closing* of the start rule, which
doesn't play nice with how it or `taul::parse_tree` are designed.