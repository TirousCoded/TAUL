# Pipelines

When the end-user wants to use the TAUL API to tokenize/parse something, they do so
by building a *pipeline* of *components*, then using this to perform the task.

Pipelines are mainly composed of *pull-based components* which *produce/consume streams
of resources*. Alongside these are secondary *push-based components* which exist to
observe pipeline state by *responding to events*. There are also other components which
do not fall into either category.

## Reset Pipelines Upon Structural Change

This is put here at the top as it's really important (though it does reference things
explained later in this document.)

Upon changing the *structure* of a pipeline, the *downstream-most components* should be
*reset* in order to ensure that all components in the pipeline operate correctly following
said structural change.

Here *structural changes* refers to things like the adding/removing of pull-based components,
or making changes to existing components which replace the current stream with a new one,
such as changing the external source a reader component receives input from.

A good example of why this is important is with a *token filter* used to implement
*automatic semicolon insertion*.

Such a filter likely operates by:

1. Consuming a token from upstream.

2. Checking if a semicolon token should be inserted behind this token.

3. If so, *caching* the token, returning a semicolon token instead, and then the next
time it's pulled on, returning said cached token rather than pulling from upstream.

The above means that such a filter will have *cached data* between uses. This is an
example of a scenario where not resetting following structural changes to the pipeline
can result in *unexpected things occurring*, even if in this case the unexpected
behaviour would likely be quite minor.

## Glyphs, Tokens And Parse Trees

Pipelines have three main resources that they *produce and/or consume*:

1. *Glyphs*

2. *Tokens*

3. *Parse Trees*

Glyphs encapsulate individual *characters* (ie. *Unicode codepoints*) decoded from
some input source, alongside *metadata*.

Tokens encapsulate sequences of characters *discerned from a sequence of glyphs*,
alongside *metadata*.

Parse trees encapsulate tree data structures describing syntactic structure *discerned
from a sequence of tokens*, alongside *metadata*.

## Readers, Lexers And Parsers

A pipeline will have three *main types of components*:

1. *Readers*

2. *Lexers*

3. *Parsers*

*(Take note that you can do things like exclude parser components when lexical
analysis is all that's wanted.)*

Readers consume *some external input source* and produce a stream of *glyphs*.

Lexers consume an *upstream glyph stream* and produce a stream of *tokens*.

Parsers consume an *upstream token stream* and produce *parse trees*.

## Listeners

Parsers can have *listener* components attached to them, which are *push-based
components* which *listen for events* which arise during parsing.

Listeners exist to provide the end-user with a mechanism by which to receive output
from a parser in the form of a series of events, allowing the end-user to choose
to forgo the allocation of a parse tree.

## Error Handlers

Error handlers are special components which can be attached to parsers to provide
them with *error handling services*.

Error handlers are *neither push-based nor pull-based components*.

## Glyph/Token Filters

*Filter components* are *pull-based components* which consume upstream input, and
produce a stream of that same resource type, *filtering* the upstream stream.

Between *readers* and *lexers*, one or more *glyph filter* components may be attached
to manipulate the glyph stream before it reaches the lexer.

Between *lexers* and *parsers*, one or more *token filter* components may be attached
to manipulate the token stream before it reaches the parser.

Things like token filter components can be used to implement features like *automatic
semicolon insertion*.

## Glyph/Token Observers

*Observer components* are *push-based components* which are attached to *pull-based
components* to observe them by receiving events.

One can attach *glyph observers* to *readers* or *glyph filters*.

One can attach *token observers* to *lexers* or *token filters*.