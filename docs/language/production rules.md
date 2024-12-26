# Production Rules

The top-level syntax of a TAUL spec describes a grammar by defining a list of *production rules*.

Production rules are either *lexer rules* (or *LPRs*) or *parser rules* (or *PPRs*.)

## Basic Syntax

Production rules take the following form:

```
<qualifier> <name> : <expression> ;
```

Where *<name>* specifies the name of the production rule, *<expression>* is the expression specifying
the pattern of the production rule, and *<qualifier>*, which is *optional*, details the qualifier
of the production rule, if any.

```
lexer section:

    A       : 'a' ;
    B       : 'b' ;
    C       : 'c' ;

    skip ws : [ \t]+ ;
	
parser section:

    ABC     : A B C ;
```

## Sections

Whether a production rule is a lexer or parser rule is determined by whether the production rule
is in a *lexer section* or a *parser section*, which is dictated by the last *section specifier*
to appear.

```
lexer section:

    A : ... ; # lexer rule
	
parser section:

    B : ... ; # parser rule
```

Prior to the first section specifier appearing, production rules are considered to be in a lexer section.

```
    A : ... ; # lexer rule
	
parser section:

    B : ... ; # parser rule
```

## Qualifiers

Qualifiers are optional modifiers prepended to production rules to change their semantics.

There are three qualifier:

- `skip` *(lexer only)*

- `support` *(lexer only)*

- `precedence` *(parser only)*

The `skip` qualifier marks a LPR as producing *skip tokens*.

```
lexer section:

    skip ws : [ \t]+ ; # defines whitespace tokens which will get cut
```

The `support` qualifier marks a LPR as existing only to help define other LPRs, but to otherwise
be excluded from lexical analysis.

```
lexer section:

	# HELPER exists to help define ABC, but is otherwise excluded from lexical analysis

	support HELPER  : 'b' ;
	
	ABC             : 'a' HELPER 'c' ;
```

The `precedence` qualifier marks a PPR as *supporting direct left-recursion*.