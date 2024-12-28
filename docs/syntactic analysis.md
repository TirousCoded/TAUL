# Syntactic Analysis

This document details TAUL *syntactic analysis* semantics.

## Basics

Syntactic analysis in TAUL operates according to the usual *LL(1) parsing* and
*pipeline* semantics.

Parsing is performed by matching a specified PPR, here called the *start rule*,
against the input, resulting in the generation of a *parse tree* and/or the
generation of a stream of *events* reported to a *listener*, if any, attached
to the parser.

## Error Handling & Aborting

If during parsing the parser detects a syntax error in the input, and is thus
unable to proceed due to the input no longer matching the expected pattern, the
parser may, if one is attached, invoke an *error handler* to try and resolve
the error.

When invoked, the error handler is able to do the following:

1. View the current input state.

2. Modify the current input state (by doing things like consuming tokens.)

3. Check whether the current input state is still in error.

The error handler is expected to use the above to try and resolve the error,
returning either once it succeeds, or if it has given up trying.

Upon returning, the parser will check to see if the error has been resolved.
If it has been, parsing continues. If it hasn't been, then the parser *aborts*
parsing.

Aborting parsing reports a special *abort event* to the listener, if any,
and still output a parse tree, just one that is incomplete, and marked as
the result of an aborted parsing process.

## Normal Event Sequence

This part of the document details the *normal event order* the end-user should
expect parsing to adhere to.

PPRs with the `precedence` qualifier adhere to this event order, but get *rewritten*
such that their event order *effectively changes*.

Upon the beginning/ending of parsing, a *startup* and *shutdown* event are
reported, respectively.

Upon the beginning/ending of the evaluation of a particular PPR during parsing,
a *syntactic* and *close* event are reported, respectively. Between these two
events are events corresponding to the contents nested within the PPR's pattern.

Upon the evaluation of a token, a *lexical* event is reported.

Alongside these are the abort event, and *other events* pertaining to syntactic
errors which are detected.