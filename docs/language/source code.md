# Source Code

This document details TAUL source code.

## Encoding

TAUL source code may be encoded with any UTF-8, UTF-16, or UTF-32 encoding.

TAUL source files may conceptually be used likewise, but with the TAUL API at present
supporting only UTF-8 and UTF-8 BOM encoded source files.

## Newlines

TAUL source code accepts *LF*, *CR*, or *CRLF* newlines, with these being handled
via the TAUL language grammar.

Notice that TAUL forbids non-escaped newlines within *string and charset literals*.
This is done in order to decouple said literals from OS newline conventions.

## Source Files

Source files by convention use the `.taul` extension.