# Sections

The top-level syntax of a TAUL spec describes a grammar by defining a list of production rules,
with *sections* specifying if a given production rule is a LPR or PPR.

## Section Specifiers

The below details the syntax used to specify if production rules are being defined in
a *lexer section* or a *parser section*.

```
lexer section: # <- lexer section specifier

	A : ... ; # lexer rule
	
parser section: # <- parser section specifier

	B : ... ; # parser rule
```

## Lexer Section By Default

Prior to the first explicit section specifier, production rules are considered to be in a lexer section.

```
    A : ... ; # lexer rule (in an implicit lexer section)

parser section:

    B : ... ; # parser rule
```