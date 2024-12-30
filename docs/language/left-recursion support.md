# Left-Recursion Support

TAUL supports left-recursive grammars via *left-recursion rewriting*.

This system is modelled heavily off of *ANTLR*, so see
[this](https://github.com/antlr/antlr4/blob/master/doc/left-recursion.md)
and [this](https://www.antlr.org/papers/allstar-techreport.pdf) (the part at the
bottom) for details about how their implementation works.

## Basics

PPRs with the `precedence` qualifier are allowed to have *direct left-recursion*
(ie. *indirect left-recursion is still illegal*.)

Additionally, such PPRs incorporate a *precedence system* to allow for the easier
writing of things like *binary operators* in *precedence hierarchies*. Take note that
when listing *alternatives* for these, the precedence hierarchy described goes from
*least to greatest precedence*.

TAUL also includes a `right_assoc` keyword which can be *placed at the start* of
alternatives to mark it as having *right associativity* (the default is for *left
associativity*.)

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

    precedence Expr : right_assoc Expr EQUALS Expr
                    | right_assoc Expr QUESTION Expr COLON Expr # ternary operator
                    | Expr PLUS Expr
                    | Expr MULTIPLY Expr
                    | L_ROUND Expr R_ROUND
                    | NUMBER
                    ;
```

These are achieved by the system performing an invisible *rewrite* of the production
rule into a different, non-left-recursive form. Note that this *rewriting changes the
parsing behaviour* associated with the PPR.

This system is powerful, and basic use is fairly straightforward. However, explaining
the *particulars* of how it works, will require the introducing of some concepts, and
associated notation.

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

    precedence Expr : right_assoc Expr EQUALS Expr
                    | right_assoc Expr QUESTION Expr COLON Expr # ternary operator
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
                    ( { <=0 } EQUALS Expr[0]
                    | { <=1 } QUESTION Expr[0] COLON Expr[1]
                    | { <=2 } PLUS Expr[3]
                    | { <=3 } MULTIPLY Expr[4]
                    )*
                    ;
```

For all the technical details as to what *exactly* TAUL does during rewriting,
*go read the source code*.

## Particulars

Below goes over some *technical details* of TAUL's left-recursion support.

### Recurse Alternatives With Only Prefix-Ref Are Ignored

If a *recurse alternative* has nothing but a prefix-ref name expression, that alternative
will be ignored.

```
lexer section:

    # NOTE: omitted

parser section:

    # NOTE: our *special notation* introduced isn't used here

    precedence Expr : right_assoc Expr EQUALS Expr
                    | right_assoc Expr QUESTION Expr COLON Expr
                    | Expr # <- this alternative will be *ignored*
                    | Expr PLUS Expr
                    | Expr MULTIPLY Expr
                    | L_ROUND Expr R_ROUND
                    | NUMBER
                    ;
```

### Prefix/Suffix-Ref Are Recognized In Composite Expressions

If you put a self-ref in a composite expression, the system *will not acknowledge if it's
a prefix-ref or suffix-ref*, even if it would otherwise qualify.

Keep this in mind, or your grammar may behave *unexpectedly*.

```
lexer section:

    # NOTE: omitted

parser section:

    # NOTE: our *special notation* introduced isn't used here

    # the usage of '( Expr )' below will cause grammar to not recognize prefix/suffix-refs!
	
    precedence Expr : right_assoc Expr EQUALS ( Expr )
                    | right_assoc ( Expr ) QUESTION Expr COLON Expr
                    | ( Expr ) PLUS ( Expr )
                    | Expr MULTIPLY ( Expr )
                    | L_ROUND Expr R_ROUND
                    | NUMBER
                    ;
```

### Self-Refs Which Are Not Prefix/Suffix-Ref Have Precedence Value Zero

Pretty self-explanatory, but self-refs which are not prefix/suffix-refs have *precedence value 0*.

This means that ALL of their alternatives become available.

*(Examples of this can be seen in above part about rewrite of grammar with ternary operator.)*