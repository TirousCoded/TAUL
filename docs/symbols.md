# Symbols

TAUL defines a system of *symbol IDs* which are used throughout the frontend/backend to describe terminal/non-terminal
symbols in a variaty of circumstances, such as when querying the FIRST/FOLLOW/prefix sets of production rules, or when
symbol information needs to be communicated between pipeline components.

## Symbol Types

Symbol IDs can identify one of three things:

- A *Unicode codepoint*.

- A *LPR ID*.

- A *PPR ID*.

## Special Symbols

Certain *special symbols* are defined:

- Unicode codepoint symbols include a special *end-of-input sentinel* symbol. This sentinel symbol is **not** a valid Unicode
codepoint, and is TAUL's alternative to using *EOF* for the end-of-input.

- LPR ID symbols include a special *end-of-input sentinel* symbol.

- LPR ID symbols include a special *failure* symbol. This is used to define *failure tokens*.

## Glyphs, Tokens And Parse Trees

During *lexical analysis*, terminals are identified by Unicode codepoint symbols, while non-terminals are identified
by LPR ID symbols.

During *syntactic analysis*, terminals are identified by LPR ID symbols, while non-terminals are identified by PPR
ID symbols.

*Glyphs* are the inputs to lexical analysis, encapsulating Unicode codepoint symbols.

*Tokens* are the outputs from lexical analysis, and the inputs to syntactic analysis, encapsulating LPR ID symbols.

*Parse Trees* are the outputs from syntactic analysis, their nodes encapsulating PPR ID symbols.