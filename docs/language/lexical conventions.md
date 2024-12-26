# Lexical Conventions

This document details lexical conventions in TAUL.

## Character Set

TAUL source code may contain *any valid Unicode codepoint*.

The term *character* herein refers to decoded Unicode codepoints.

## Escape Sequences

TAUL supports the following *escape sequences*:

```
\0              <- Null
\a              <- Bell (Alert)
\b              <- Backspace
\f              <- Form Feed
\n              <- Line Feed
\r              <- Carriage Return
\t              <- Horizontal Tab
\v              <- Vertical Tab
\'              <- Single Quotation
\]              <- Right Square Bracket
\-              <- Minus
\\              <- Backslash
\xhh            <- Hex Literal (8-bit)
\uhhhh          <- Hex Literal (16-bit)
\uhhhhhhhh      <- Hex Literal (32-bit)
```

Escaped characters which do not otherwise form one of the above escape 
sequences are instead *literalized*, meaning that the opening `\` is 
simply removed.

TAUL doesn't support *octal literal* escape sequences.

TAUL *hex literal* escape sequences are *case-insensitive*.

```
\xaf
\xAf
\xaF
\xAF
```

TAUL *hex literal* escape sequences which are malformed are interpreted as
*literalized* `x`, `u` or `U` characters, and the remaining characters thereafter
are not otherwise considered to be escaped.

```
\x3g            <- interpreted as literalized x
```

## Tokens

### Whitespace

A *whitespace token* is a sequence of one or more ASCII *space* or *tab* characters.

### Newlines

A *newline token* is either a *LF*, *CR*, or *CRLF* character sequence, preferring the *longest possible match*.

### Comments

A *comment token* is a `#` followed by any characters not forming newlines.

TAUL does not support multiline comments.

### Keywords

A *keyword token* is any of the following:

- `lexer`
- `parser`
- `section`
- `skip`
- `support`
- `precedence`
- `end`
- `any`
- `token`
- `failure`

### Operators

An *operator token* is any of the following:

- `.` *(dot)*
- `:` *(colon)*
- `;` *(semicolon)*
- `|` *(vbar)*
- `?`
- `*`
- `+`
- `&`
- `-` *(minus)*
- `~` *(tilde)*
- `(`
- `)`

### Identifiers

An *identifier token* is a sequence of ASCII letters, digits, and underscores, which does not
start with a digit, and which is not a keyword.

```
Abc
OP_COLON
ab123cd
__abc
```

### Strings

A *string (literal) token* is a pair of *single-quotes* (ie. not *double-quotes*) with
*visible characters* between them, including *escape sequences*.

```
'this is an example string 123 !@#'
```

In string literals, single-quotes must be escaped.

```
'abc\'def'
```

In string literals, double-quotes need-not be escaped.

```
'abc"def'
```

String literals are forbidden from containing *non-visible ASCII characters*, in order to
protect the end-user from potentially *hard to diagnose issues* arising from string
literals containing *hidden control characters*.

String literals are forbidden from containing *newlines* (see *newline tokens*) in order to
ensure that OS differences regarding newlines doesn't result in *portability issues* the end-user
might otherwise have to diagnose (ie. from something not working.)

### Charsets

A *charset (literal) token* is a pair of `[` and `]` characters with *visible characters* between
them, including *escape sequences*.

Modelled off *character sets in ANTLR*, a *charset* (aka. *character set*) is a pattern matching a
*unit-length portions of text* containing *one character* which exists in the *set of characters*
described by the literal.

```
[abc]
[a-zA-Z0-9_]
```

In charset literals, `]` characters must be escaped.

```
[abc\]def]
```

Charset literals are forbidden from containing *non-visible ASCII characters* or *newlines* (see
*newline tokens*) for the same reasons as string literals.

The *set of characters* of the charset are expressed by combinations of *individual characters*
and *character ranges*.

An *individual character* adds said character to the set.

```
[abc]           <- contains a, b and c
```

A *character range* adds characters an *inclusive* (ie. not *exclusive*) range of characters
to the set.

Character ranges take the form `X-Y`, where *X* and *Y* are the ends of the range, with the range
being the range *[X, Y]*. Such a character range adds characters *X*, *Y*, and *all characters
between X and Y* to the set.

```
[a-zA-Z]        <- contains all uppercase and lowercase latin alphabet characters
```

In the above character range `X-Y`, take note that *X* need-not be a lower Unicode codepoint than
*Y*. The characters *X* and *Y* may be equal, and *X* may even be larger than *Y*.

The following charsets are equivalent:

```
[a-zA-Z0-9]
[z-aZ-A9-0]
```

Character ranges may include escape sequences.

The following charsets are equivalent:

```
[a-z]
[\x61-z]
[a-\x7a]
[\x61-\x7a]
```

The `-` character must be escaped for use as a regular character in a charset.

```
[a\-z]          <- contains a, -, z and nothing else
```

There are three cases where `-` may be used as a regular character without
needing to escape it as `\-`:

1. If it appears at the vary start of the charset.

2. If it appears as the right value of a character range (ie. the *Y* in `X-Y`.)

3. If it appears as the escape sequence `\x2d` (or the equivalent 16- or 32-bit version.)

```
[-a-z]          <- contains - and all lowercase latin alphabet characters
[a--]           <- contains a, -, and all characters between them
[a\x2dz]        <- contains a, z, and -
```

Individual characters and character ranges may be used together.

```
[a-f123g-l_ef]  <- contains a through f, 1, 2, 3, g through l, _, e and f
```

Charsets may contain multiple instances of a given character being added to it.

The following charsets are equivalent:

```
[a-f]
[aa-faaaa]
[a-fa-ffffffa-fa-faaaa]
```