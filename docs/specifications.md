# Specifications

TAUL *specifications* or *specs* are translation units from which grammars
are generated.

## Specification Forms

Specs come in two forms:

- *Source Code* (including *Source Files*)

- A Non-Portable *Binary Form* (see `binary specs.md`)

Specs do not have a notion of establishing dependency relationships between
one another. Each spec is *semantically an island*.

## Compilation & Loading

The term *compilation* refers to the compiling of TAUL source code into a
binary spec.

The term *loading* refers to the generating of a grammar from a binary spec.

The act of translating source code directly into a grammar is also referred
to as *loading* (though the term *compilation* is also used to refer to this
process.)