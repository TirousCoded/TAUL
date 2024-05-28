## Text Analysis Utility Language (TAUL)
TAUL is a language/library which seeks to provide the capabilities of a parser generator with the conveniences of a scripting language.

## Project Status
TAUL is currently in Version 0.1 (beta), meaning that the frontend is pretty much complete, but I still reserve the right to make breaking changes to it liberally, as the frontend is still open to be iterated upon as needed.

TAUL will need to be worked on to ensure that it's stable, optimized, portable, and refactored so it can be properly extended, alongside a bunch of other things of course.

I intend to return to this project to finish up Version 1.0 later on, but I'm taking a break for now to get started on my next project: a scripting language called Yama.
I'm planning on using Yama as a kind of testing ground to put TAUL to the test, to see what works and what doesn't.

## Getting Started
TAUL isn't ready yet, but this is here nevertheless for those who want to try it out.

First, clone the repo via `git clone https://github.com/TirousCoded/TAUL.git`.

Don't worry about dependencies: TAUL has no external dependencies (that's a selling point, btw.)

Next, use TAUL's build system [premake](https://premake.github.io/) to build the solution.

Please see `docs/manual.txt` for a (not 100% complete, but mostly done) reference manual for TAUL (the language, API, CLI driver app, etc.)

TAUL is not done yet, so much of the API is not yet documented, except with documentation comments in the source code.
So ya, you're on your own for right now regarding API docs, but you can still read sandbox or unit test code for some examples, and you can read the frontend source itself, which is fairly well documented with documentation comments.

Again, TAUL's not done yet, but I hope this helps anyone who finds this humble little repo and is interested in trying it out.