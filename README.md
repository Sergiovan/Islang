# Islang

## Description

This is a lexer/parser/interpreter for a language I made up for my own amusement, Islang. It was made to generate random-ish environments given parameters, and some added useless features simply to see if I was able to do it. 

Also included under `.spec` is a syntax highlight for [Notepad++](https://notepad-plus-plus.org/), for convenience. 

Run with `Islang.exe path/to/file.isl` to run `path/to/file.isl`.

## Language

[The language is specified in ecbn format under `.spec`](https://github.com/Sergiovan/Islang/blob/master/.spec/spec.ebnf), although I'm fairly certain I have implemented it slightly differently than how it is in the spec file right now. 

[Read the wiki for more info on the language tho](https://github.com/Sergiovan/Islang/wiki)

## CLI
* `run`: Will run the interpreted program once
* `exit`: Will exit the program

## FAQ

### Why tho?
I was bored.

### Why the python syntax?
It seemed harder than using semicolons. Wanted to try it.

### Why make it a typed language?
Same reason as the whitespace dependency. Seemed harder and thus more interesting to me.

### Why can I not do math?
This wasn't designed to calculate things, rather to regenerate roleplay locations for a game I know. So no math was required. If it is in the future I'll make sure to add it.

### I hate this.
That's not a question.

### I hate this?
I don't know. Maybe you do?

## What's next
- **Functions-ish**
  - Group `repr` statements together
  - Parameters for the function only (typed, of course)

## TODO
- **Syntax**
  - Convert lists with `identifier{probabilities}` syntax
  - Make enum-to-list work without percentages, somehow
  - Picking from enums directly
  - Allow `opts` to have probabilities in its elements 
  - Allow `repr` and `opts` to take variables, or any value, really
  - Allow `repr` and `opts` to be used interchangeably (Add if expression to opts, and allow them to be `else`'d)
  - Fix program structure being fixed
  - Fix the ecbn spec
- **Interpreter**
  - Stupid-proof reading of files in `Scanner.cpp` (Not that I'd ever read myself out of memory or   anything...)
  - Fix the showing of the parser errors, they're all over the place
  - Fix errors when lines are too damn long
- **CLI**
  - Add CLI interpreting while running the program
  - Add CLI reading of files if none specified
  - Add CLI everything, actually
  - Ctrl+C support
- **Bugs**
  - ~~Fix this random access error? Where is this even coming from?~~
  - Pretty sure there's a memory leak or two somewhere
  - Fix empty blocks showing text on the console
- **Testing**
  - Test how good the probabilities are
- **Git**
  - ~~Add examples~~
- **Misc**
  - Think of a name for non-percentages
  - Think of a name for the magical enum-to-list expression
