# CLI Driver App

TAUL comes packaged with a small yet useful CLI driver app called *taulc.exe* or just *taulc*, which provides services like grammar error checking and grammar transcompilation into C++, alongside of course its usage in scripts.

## Commands

### Help

*Synopsis:*

```
taulc help <command>
```

Explains form and function of *\<command\>*.

### Version

*Synopsis:*

```
taulc version
```

Explains the TAUL library version the *taulc* was compiled under.

### Check

*Synopsis:*

```
taulc check <source-path>
```

Checks if TAUL spec file at *\<source-path\>*, if any, compiles correctly.

Any compilation errors which arise are reported.

All stages of TAUL spec compilation and loading are tested for errors.

### Compile

*Synopsis:*

```
taulc compile <fetcher> <source-path> <output-path>
```

Compiles the TAUL spec file at *\<source-path\>*, if any, and if successful outputs a C++ header file at *\<output-path\>*, with this generated header file encapsulating a TAUL fetcher function named *\<fetcher\>*.

Any existing file at *\<output-path\>* is overwritten.

No checks are made to ensure that *\<fetcher\>* is a valid C++ identifier.

Any compilation errors which arise are reported.

All stages of TAUL spec compilation and loading are tested for errors.
