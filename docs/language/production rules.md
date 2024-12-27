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

### Alternatives

Certain composite expressions contain nested *alternatives*.

Alternatives are sequences of nested subexpressions, and act somewhat like composite expressions.

### FIRST Sets

All *expressions* and *alternatives* have *FIRST sets*, which given the set of possible symbol
sequences matching an expression's/alternative's pattern, is the set of all the *first symbols*
in said sequences (excluding empty sequences.)

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

*Type: Composite (Uses Alternatives)*

*Synopsis:*

```
<alt-1> | <alt-2> | ... | <alt-N>
```

*Where:*

- *\<alt-#\>* are the *alternatives* of the expression.

Top-level expressions are special composite expressions encapsulating
the *ultimate patterns of production rules*.

Matches the subexpression sequence of the *\<alt-#\>* which can match
successfully, *consuming input*.

```
lexer section:

    A       : 'a' | 'A' ;
    B       : 'b' | 'B' ;
    C       : 'c' | 'C' ;
    D       : 'd' | 'D' ;

parser section:

    Syntax  : A B_Or_D C ;
	
	B_Or_D  : B | D ;
```

### End

*Type: Simple*

*Synopsis:*

```
end
```

Matches the *end-of-input* glyph/token, *consuming no input*.

```
lexer section:

    A   : 'a' ;
    B   : 'b' ;
    C   : 'c' ;
	
    END : end ; # creates END token when end-of-input is reached

parser section:

    ABC : A B C end ; # expects end-of-input following 'abc'
```

### Any

*Type: Simple*

*Synopsis:*

```
any
```

Matches any glyph/token, except *end-of-input*, *consuming input*.

```
lexer section:

    # A matches 'a?c', where ? is any character

    A       : 'a' any 'c' ;

parser section:

    # Syntax matches 'a?ca?ca?c???', where ? is any character

    Syntax  : A A A any any any ;
```

### Token

*Type: Simple (Parser Rules Only)*

*Synopsis:*

```
token
```

Matches any token except *failure* or *end-of-input* tokens, *consuming input*.

```
lexer section:

    A      : 'a' ;
    B      : 'b' ;
    C      : 'c' ;
	
parser section:

    Syntax : token token token ;
```

### Failure

*Type: Simple (Parser Rules Only)*

*Synopsis:*

```
failure
```

Matches any *failure* token, *consuming input*.

```
lexer section:

    A      : 'a' ;
    B      : 'b' ;
    C      : 'c' ;
	
parser section:

    # Syntax matches 'a?b?c', where ? is a sequence of >=1 characters which fail
    # lexical analysis

    Syntax : A failure B failure C ;
```

### String

*Type: Simple (Lexer Rules Only)*

*Synopsis:*

```
<string>
```

*Where:*

- *\<string\>* is the *string literal*.

Matches the glyph sequence described by *\<string\>*, *consuming input*.

```
lexer section:

    ABC : 'abc' ;
```

### Charset

*Type: Simple (Lexer Rules Only)*

*Synopsis:*

```
<charset>
```

*Where:*

- *\<charset\>* is the *charset literal*.

Matches a glyph in *\<charset\>*, *consuming input*.

```
lexer section:

    LETTER : [a-zA-Z] ;
```

### Name

*Type: Simple*

*Synopsis:*

```
<name>
```

*Where:*

- *\<name\>* is the *name identifier* specifying a production rule.

*If in a LPR*:

- *\<name\>* must specify another LPR.

- Matches LPR *\<name\>*, *consuming input*.

*If in a PPR*:

- If *\<name\>* specifies a LPR: matches a token which is associated with LPR *\<name\>*.

- If *\<name\>* specifies a PPR: matches PPR *\<name\>*, *consuming input*, with this
evaluation being *associated with a new branch being output*.

```
lexer section:

    A               : HELPER ;
	B               : 'b' ;
	C               : 'c' ;

    support HELPER  : 'a' ;
	
parser section:

    Syntax          : ABC end ;
	
	ABC             : A B C ;
```

### Sequence

*Type: Composite (Uses Alternatives)*

*Synopsis:*

```
( <alt-1> | <alt-2> | ... | <alt-N> )
```

*Where:*

- *\<alt-#\>* are the *alternatives* of the expression.

Matches the subexpression sequence of the *\<alt-#\>* which can match
successfully, *consuming input*.

```
lexer section:

    ABC     : 'a' ( 'b' | 'd' ) 'c' ;

    B       : 'b' ;
	
parser section:

    Syntax  : B ( ABC | B B B ) ABC end ;
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

Matches the next glyph/token, *consuming no input*, if and only if said said glyph/token
is in the *FIRST set* of this expression.

This expression's *FIRST set* may not contain the *end-of-input*. The reason is because
*lookahead-not* and *not* expressions can be confusing to reason about if they can include
*end-of-input*, and so the decision was made to forbid it in general.

```
TODO
```

### LookAhead-Not

*Type: Composite (Set-Like)*

*Synopsis:*

```
-<subexpr>
```

*Where:*

- *\<subexpr\>* is the subexpression.

These expressions are for *peeking* at what the next symbol is **not**.

Matches the next glyph/token, *consuming no input*, if and only if said said glyph/token
is **not** in the *FIRST set* of this expression.

This expression's *FIRST set* may not contain the *end-of-input*. The reason is because
*lookahead-not* and *not* expressions can be confusing to reason about if they can include
*end-of-input*, and so the decision was made to forbid it in general.

```
TODO
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
expressions, and not for merely *peeking* at the next symbol.

Matches the next glyph/token, *consuming input*, if and only if said said glyph/token
is **not** in the *FIRST set* of this expression.

This expression's *FIRST set* may not contain the *end-of-input*. The reason is because
*lookahead-not* and *not* expressions can be confusing to reason about if they can include
*end-of-input*, and so the decision was made to forbid it in general.

```
TODO
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
lexer section:

    ABC     : 'a' 'b'? 'c' ; # matches strings 'abc' and 'ac'

parser section:

    Syntax  : ABC? end ;
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
lexer section:

    ABC     : 'a' 'b'* 'c' ; # matches strings 'ac', 'abc', 'abbc', ...

parser section:

    Syntax  : ABC* end ;
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
lexer section:

    ABC     : 'a' 'b'+ 'c' ; # matches strings 'abc', 'abbc', 'abbbc', ...

parser section:

    Syntax  : ABC+ end ;
```