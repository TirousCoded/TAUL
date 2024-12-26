# Production Rules

The top-level syntax of a TAUL spec describes a grammar by defining a list of *production rules*.

Production rules are either *lexer rules* (or *LPRs*) or *parser rules* (or *PPRs*.)

## Basic Syntax

*Synopsis:*

```
<name> : <expression> ;
<qualifier> <name> : <expression> ;
```

*Where:*

- *\<name\>* is the name of the production rule.

- *\<expression\>* is the expression defining the pattern of the production rule.

- *\<qualifier\>*, if any, is the qualifier of the production rule.

```
lexer section:

    A       : 'a' ; # A, B and C are lexer rules
    B       : 'b' ;
    C       : 'c' ;

    skip ws : [ \t]+ ; # ws has 'skip' qualifier
	
parser section:

    ABC     : A B C ; # ABC is a parser rule
```

## Sections

*Synopsis:*

```
lexer section :
parser section :
```

Whether a production rule is a lexer or parser rule is determined by whether the production rule
is in a *lexer section* or a *parser section*, which is dictated by the last *section specifier*
to appear prior to the production rule.

```
lexer section:

    A : ... ; # lexer rule
	
parser section:

    B : ... ; # parser rule
```

Prior to the first section specifier appearing, production rules are in a *lexer section*.

```
    A : ... ; # lexer rule
	
parser section:

    B : ... ; # parser rule
```

## Qualifiers

*Synopsis:*

```
skip
support
precedence
```

Qualifiers change production rule semantics.

The `skip` qualifier *(lexer rules only)* marks a LPR as producing *skip tokens*.

```
lexer section:

    skip ws : [ \t]+ ; # defines whitespace tokens which will get cut
```

The `support` qualifier *(lexer rules only)* marks a LPR as existing only to help define other LPRs,
but to otherwise be excluded from lexical analysis.

```
lexer section:

    # HELPER exists to help define ABC, but is otherwise excluded from lexical analysis

    support HELPER  : 'b' ;

    ABC             : 'a' HELPER 'c' ;
```

The `precedence` qualifier *(parser rules only)* marks a PPR as *supporting direct left-recursion*.

```
lexer section:

    PLUS            : '+' ;
	MULTIPLY        : '*' ;
	NUMBER          : [0-9]+ ;

    skip ws         : [ \t]+ ;
	
parser section:

    precedence Expr : Expr PLUS Expr
                    | Expr MULTIPLY Expr
                    | NUMBER
                    ;
```

## Expressions

Expressions describe the symbol patterns of production rules.

LPR expressions are patterns of *glyphs*, and PPR expressions are patterns of *tokens*.

### Simple & Composite Expressions

TAUL makes a distinction between *simple* and *composite* expressions:

- A *simple expression* is a primary expression which cannot have subexpressions.

- A *composite expression* is a non-primary expression which can have subexpressions.

### Set-Like Expressions

Certain composite expressions are *set-like*, meaning that the set of possible symbol
sequences matching its pattern **must** be populated only by *sequences of length one*
(ie. there must be no sequences of length zero, nor of length two or greater.)

This guarantee makes set-like expressions operate akin to charsets.

### Expression Precedence

Excluding the *special top-level expression*, the following lists TAUL expressions in order of
*precedence*, from *highest to lowest*:

- *end*

- *any*

- *token*

- *failure*

- *string*

- *charset*

- *name*

- *sequence*

- *lookahead*

- *lookahead-not*

- *not*

- *optional*

- *kleene-star*

- *kleene-plus*

### Top-Level

*Type: Composite*

*Synopsis:*

```
<alt-1> | <alt-2> | ... | <alt-N>
```

*Where:*

- *\<alt-#\>* are the *alternatives* of the expression.

Alternatives are subexpression sequences.

Top-level expressions are a special composite expression which
encapsulates the top-level of LPR/PPR expressions.

Matches the subexpression sequence of the *\<alt-#\>* which matches 
successfully, consuming input.

It is illegal for common prefixes between alternatives to cause
the grammar to be ambiguous.

```

```

### End

*Type: Simple*

*Synopsis:*

```
end
```

Matches the *end-of-input* glyph/token, *consuming no input*.

```

```

### Any

*Type: Simple*

*Synopsis:*

```
any
```

Matches any glyph/token, except *end-of-input*, *consuming input*.

```

```

### Token

*Type: Simple (Parser Rules Only)*

*Synopsis:*

```
token
```

Matches any token except *failure* or *end-of-input* tokens, *consuming input*.

```

```

### Failure

*Type: Simple (Parser Rules Only)*

*Synopsis:*

```
failure
```

Matches any *failure* token, *consuming input*.

```

```

### String

*Type: Simple (Lexer Rules Only)*

*Synopsis:*

```
<string>
```

The *\<string\>* is the *string literal* specifying the string.

Matches the glyph sequence described by *\<string\>*, *consuming input*.

```

```

### Charset

*Type: Simple (Lexer Rules Only)*

*Synopsis:*

```
<charset>
```

The *\<charset\>* is the *charset literal* specifying the charset.

Matches a glyph in *\<charset\>*, *consuming input*.

```

```

### Name

*Type: Simple*

*Synopsis:*

```
<name>
```

The *\<name\>* is an identifier specifying a LPR or PPR.

*If in a LPR*, *\<name\>* must specify another LPR.

*If in a LPR*, matches LPR *\<name\>*, *consuming input*.

*If in a PPR*, and *\<name\>* specifies a LPR, matches a token which is associated
with LPR *\<name\>*.

*If in a PPR*, and *\<name\>* specifies a PPR, matches PPR *\<name\>*, *consuming input*,
and with this evaluation being associated with a new branch being output.

```

```

### Sequence

*Type: Composite*

*Synopsis:*

```
( <alt-1> | <alt-2> | ... | <alt-N> )
```

*Where:*

- *\<alt-#\>* are the *alternatives* of the expression.

Alternatives are subexpression sequences.

Matches the subexpression sequence of the *\<alt-#\>* which matches 
successfully, *consuming input*.

It is illegal for common prefixes between alternatives to cause the 
grammar to be ambiguous.

```

```

### LookAhead

*Type: Composite (Set-Like)*

*Synopsis:*

```
&<subexpr>
```

*Where:*

- *\<subexpr\>* is the subexpression.

These expressions are for *peeking* at what the next symbol is.

Matches a set *S*, *consuming no input*, where *S* is the *FIRST set* of *\<subexpr\>*.

The *FIRST set* of *\<subexpr\>* may not contain the *end-of-input*.

This choice to forbid the *FIRST set* of *\<subexpr\>* from containing the *end-of-input*
was made deliberately as expressions like *lookahead-not* and *not* may be confusing to use
if *end-of-input* is present, and so the decision was made to forbid it in general, requiring
the end-user to specify it elsewhere in their production rules.

```

```

### LookAhead-Not

*Type: Composite (Set-Like)*

*Synopsis:*

```
-<subexpr>
```

*Where:*

- *\<subexpr\>* is the subexpression.

These expressions are for *peeking* at what the next symbol is not.

Matches a set *S*, *consuming no input*, where *S* is the set of all symbols,
except the *end-of-input*, and except those in the *FIRST set* of *\<subexpr\>*.

The *FIRST set* of *\<subexpr\>* may not contain the *end-of-input*.

This choice to forbid the *FIRST set* of *\<subexpr\>* from containing the *end-of-input*
was made deliberately as expressions like *lookahead-not* and *not* may be confusing to use
if *end-of-input* is present, and so the decision was made to forbid it in general, requiring
the end-user to specify it elsewhere in their production rules.

```

```

### Not

*Type: Composite (Set-Like)*

*Synopsis:*

```
~<subexpr>
```

*Where:*

- *\<subexpr\>* is the subexpression.

These expressions are similar almost identical to *lookahead-not*, except that
these consume input, and are used to invert charsets and set-like composite
expressions, and not for *merely peeking* at the next symbol.

Matches a set *S*, *consuming input*, where *S* is the set of all symbols,
except the *end-of-input*, and except those in the *FIRST set* of *\<subexpr\>*.

The *FIRST set* of *\<subexpr\>* may not contain the *end-of-input*.

This choice to forbid the *FIRST set* of *\<subexpr\>* from containing the *end-of-input*
was made deliberately as expressions like *lookahead-not* and *not* may be confusing to use
if *end-of-input* is present, and so the decision was made to forbid it in general, requiring
the end-user to specify it elsewhere in their production rules.

```

```

### Optional

*Type: Composite*

*Synopsis:*

```
<subexpr>?
```

*Where:*

- *\<subexpr\>* is the subexpression.

Matches a sequence of *\<subexpr\>* zero or one times, *consuming input*.

Matching is performed on a *greedy basis*, consuming as much input as possible.

```

```

### Kleene-Star

*Type: Composite*

*Synopsis:*

```
<subexpr>*
```

*Where:*

- *\<subexpr\>* is the subexpression.

Matches a sequence of *\<subexpr\>* zero or more times, *consuming input*.

Matching is performed on a *greedy basis*, consuming as much input as possible.

```

```

### Kleene-Plus

*Type: Composite*

*Synopsis:*

```
<subexpr>+
```

*Where:*

- *\<subexpr\>* is the subexpression.

Matches a sequence of *\<subexpr\>* one or more times, *consuming input*.

Matching is performed on a *greedy basis*, consuming as much input as possible.

```

```