# Islang

## Description

This is a lexer/parser/interpreter for a language I made up for my own amusement, Islang. It was made to generate random-ish environments given parameters, and some added useless features simply to see if I was able to do it. 

Also included under `.spec` is a syntax highlight for [Notepad++](https://notepad-plus-plus.org/), for convenience. 

Run with `Islang.exe path/to/file.isl` to run `path/to/file.isl`.

## Language

[The language is specified in ecbn format under `.spec`](https://github.com/Sergiovan/Islang/blob/master/.spec/spec.ebnf), although I'm fairly certain I have implemented it slightly differently than how it is in the spec file right now. 

*TODO ADD LANGUAGE SYNTAX AND THOSE SHENANIGANS*

## CLI
* `run`: Will run the interpreted program once
* `exit`: Will exit the program

## TODO
- Add examples
- Fix the ecbn spec
- Fix the showing of the parser errors, they're all over the place
- Pretty sure there's a memory leak or two somewhere
- Add CLI interpreting while running the program
- Add CLI reading of files if none specified
- Add CLI everything, actually
- Picking from enums directly
- Convert lists with `identifier{probabilities}` syntax
- Allow repr and opts to take variables
- Test how good the probabilities are
- Fix empty blocks showing text on the console
- Fix errors when lines are too damn long
- Stupid-proof reading of files in `Scanner.cpp` (Not that I'd ever read myself out of memory or anything...)
- Ctrl+C support
- Fix this random access error? Where is this even coming from?