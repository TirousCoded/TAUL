# Ambiguity Errors

This document explains when *illegal ambiguity errors* arise, so as to clear up potential confusion.

## Alternation & Grammar Ambiguity

When a composite expression with *alternatives* is matched, one of the alternatives is selected to
use for the match.

Each alternative has a *FIRST set*, and the way the system selects which alternative to use is based
on which alternative's *FIRST set*, if any, contains the next symbol being matched against.

When two or more alternatives for a given composite expression contain the same symbol in their
*FIRST sets*, the grammar is deemed *ambiguous*.

If the grammar is ambiguous, that means that for the offending composite expressions, the system
won't be able to find *only one match* when selecting an alternative to use. This breaks the underlying
mechanism of the *LL(1) parsing system* TAUL is built upon, and thus *grammar ambiguity is illegal*.

*The* `precedence` *qualifier notwithstanding*, these rules apply to LPRs and PPRs *equally*.

The following is a basic example of ambiguity:

```
lexer section:

    # ABC is ambiguous as 3 of its alternatives all include 'a'

    ABC : 'a' ( 'b' | 'd' ) 'c' # <- FIRST set includes 'a'
        | 'abc'                 # <- FIRST set includes 'a'
        | 'adc'                 # <- FIRST set includes 'a'
        ;
```

The following is a slightly more nuanced example of ambiguity:

```
lexer section:

    ABC : 'a' ( 'b' | 'd' ) 'c' # <- FIRST set includes 'a'
        | ADC                   # <- FIRST set includes 'a'
        ;

    support ADC : 'adc' ; # <- note how ADC's FIRST set affects ABC's
```

The above rules also mean that *left-recursion* is also illegal, as it makes the grammar ambiguous,
for slightly more nuanced reasons:

```
lexer section:

    PLUS        : '+' ;
    MULTIPLY    : '*' ;
    NUMBER      : [0-9]+ ;
	
parser section:

    Expr        : Expr '+' Expr # <- ambiguous for PPR w/out 'precedence' qualifier!
                | Expr '*' Expr # <- ambiguous for PPR w/out 'precedence' qualifier!
                | NUMBER
                ;
```

## Ambiguity In Precedence PPRs

In *precedence PPRs* keep in mind that ambiguity could be based on *ambiguity between recurse
alternatives*, in which case you need to compare the expressions *after the prefix-ref*.

Currently, TAUL error messages don't really make this distinction clear, and incorrectly report
said ambiguity to be *"over the inputs of..."*, which is not correct wording.