# Left-Recursion Support

TAUL supports left-recursive grammars via *left-recursion rewriting*.

This system is modelled heavily off of *ANTLR*, so see
[this](https://github.com/antlr/antlr4/blob/master/doc/left-recursion.md)
and [this](https://www.antlr.org/papers/allstar-techreport.pdf) (the part at the
bottom) for details about how their implementation works.

## Basics

PPRs with the `precedence` qualifier are allowed to have *direct left-recursion*.

Additionally, such PPRs incorporate a *precedence system* to allow for the easier
writing of things like *binary operators* in *precedence hierarchies*. Take note that
when listing *alternatives* for these, the precedence hierarchy described goes from
*least to greatest precedence*.

```
lexer section:

    PLUS            : '+' ;
    MULTIPLY        : '*' ;
    EQUALS          : '=' ;
    QUESTION        : '?' ;
    COLON           : ':' ;
    L_ROUND         : '(' ;
    R_ROUND         : ')' ;
	
    NUMBER          : [0-9]+ ;

    skip WS         : [ \t]+ ;

parser section:

    precedence Expr : Expr EQUALS Expr
                    | Expr QUESTION Expr COLON Expr # ternary operator
                    | Expr PLUS Expr
                    | Expr MULTIPLY Expr
                    | L_ROUND Expr R_ROUND
                    | NUMBER
                    ;
```

These are achieved by the system performing an invisible *rewrite* of the production
rule into a different, non-left-recursive form. Note that this rewriting changing the
parsing behaviour associated with the PPR.

This system is powerful, and basic use is fairly straightforward. However, explaining
the *particulars* of how it works, as well as explaining the *strict technical limits*
it imposes regarding what left-recursion *is/isn't allowed* will require the introducing
of some concepts, and associated notation.

## Concepts/Notation

Note that the below new notation is *not real TAUL syntax*, but instead exists solely
for this document to use to explain things.

### Terminology

Within a *precedence PPR*, the following terminology is used:

- Recursive self-referential name expression are called *self-refs*.

- Left-recursive self-refs are called *prefix-refs*.

- Right-recursive self-refs are called *suffix-refs*.

- *Top-level alternatives* (ie. not an alternative of a sequence expression) are
*recurse alternatives* if they have a prefix-ref, and *base alternatives* otherwise.

### Precedence Values

When a precedence PPR is matched, it is parameterized by an invisible argument value
passed to it called a *precedence value*.

When matched by the end-user, or by a name expression in a PPR other than itself, this
precedence value is 0.

When matched by a name expression in the PPR itself, the notation `<name>[<value>]`
will be used in place of regular name expression syntax to explicitly specify the
precedence value, where *\<value\>* is said precedence value.

### Precedence Predicates

These are attached to *alternatives* within a precedence PPR, and specify a precedence
value that, if the precedence value of the PPR were to exceed it, would result in that
alternative being *disabled* (ie. this is used to implement *precedence hierarchies*.)

Precedence predicates will use the notation `{ <=<value> }`, where *\<value\>* is the
precedence value above which the alternative is disabled.

## The Rewriting Itself

The following shows an example of the rewriting process itself:

*Before:*

```
lexer section:

    # NOTE: omitted

parser section:

    # NOTE: our *special notation* introduced isn't used here

    precedence Expr : Expr EQUALS Expr
                    | Expr QUESTION Expr COLON Expr # ternary operator
                    | Expr PLUS Expr
                    | Expr MULTIPLY Expr
                    | L_ROUND Expr R_ROUND
                    | NUMBER
                    ;
```

*After:*

```
lexer section:

    # NOTE: omitted

parser section:

    precedence Expr :
                    ( L_ROUND Expr[0] R_ROUND
                    | NUMBER
                    )
                    ( { <=0 } EQUALS Expr[1]
                    | { <=1 } QUESTION Expr[0] COLON Expr[2]
                    | { <=2 } PLUS Expr[3]
                    | { <=3 } MULTIPLY Expr[4]
                    )*
                    ;
```

For all the technical details as to what *exactly* TAUL does during rewriting,
*go read the source code*.

## Parse Behaviour

The above example result of precedence PPR rewriting specifies the ways in which
rewriting modifies the observed parsing behaviour of the PPR, but for further
clarity, this section provides a quick example of expected behaviour.

*Grammar:*

```
lexer section:

    # NOTE: omitted

parser section:

    precedence Expr : Expr EQUALS Expr
                    | Expr QUESTION Expr COLON Expr # ternary operator
                    | Expr PLUS Expr
                    | Expr MULTIPLY Expr
                    | L_ROUND Expr R_ROUND
                    | NUMBER
                    ;
```

*Input:*

```

```

*Output:*

```

```

TODO: after above, write sections for all the tiny nuances