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
codepoint, and is simply TAUL's alternative to using *EOF* for the *end-of-input*.

- LPR ID symbols include a special *end-of-input sentinel* symbol.

- LPR ID symbols include a special *failure* symbol. This is used to define *failure tokens*.

## Conflation With Glyphs And Tokens

In TAUL source code and docs, *glyphs* and *tokens* are often conflated with *symbol IDs*, including with the
term *symbol* often being used as a common name for glyphs/tokens.

Strictly speaking, glyphs/tokens *contain* symbol IDs, plus *metadata*, acting as *carriers* for them.

Nevertheless, this conflation is made, as these terms *play together fairly nicely*.