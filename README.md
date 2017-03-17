# Islang
--------

## Description

This is a lexer/parser/interpreter for a language I made up for my own amusement, Islang. It was made to generate random-ish environments given parameters, and some added useless features simply to see if I was able to do it. 

Also included under `.spec` is a syntax highlight for [Notepad++](https://notepad-plus-plus.org/), for convenience. 

Run with `Islang.exe path/to/file.isl` to run `path/to/file.isl`.

## Language

[The language is specified in ecbn format under `.spec`](https://github.com/Sergiovan/Islang/blob/master/.spec/spec.ebnf), although I'm fairly certain I have implemented it slightly differently than how it is in the spec file right now. 

### Program structure

An Islang program consists of 4 main parts:
* [Imports]()
* [Global declarations](#Variables)
* [Main location]()
* [Generate statement]()

The order of these parts is important (*Will probably change in the future, if I get to it*), and they are all optional. This means a blank file is technically valid, although a bit unhelpful.
Whitespace at the start of the line is important, as it indicates blocks and scopes. Consecutive lines indented the same amount are part of the same block, and probably the same scope as well. 
Statements end with a newline, rather than a semicolon or any other symbol. This means you may not put newlines anywhere you want, as that will change the meaning of your program. There is an exception to this rule when declaring a list or an enum, as newlines between the delimiters (square brackets or brackets, respectively) will be ignored, for convenience.

###### Comments
Line comments start with a `#`, and block comments start with `/*` and end with `*/`. Additionally, block comments can be nested.
```
# This is a line comment
/* A block comment*/ # And then a line comment
/* /* This is perfectly */ fine */
```

### Variables

Islang is a [typed language](# "Strongly? Dynamically? Weakly? I don't know, but there's types involved, and you type the typenames, alright?"). You declare a variable like so:

```
typename variablename -> value
```

There are 6 different types: 
* `num`: Number. A floating point number, used for all numbers. 
* `str`: String. A string.
* `bool`: Boolean. A boolean value.
* `list`: List. A list (Like an array) of values of the same type.
* `enum`: Enumeration. An enumeration.
* `val`: Enumeration **val**ue. Represents an enumerator value.

###### Numbers
Numbers are pretty easy. They are written like you'd write them in python, for example. `1`, `-27.2761` and `888888` are all possible values for numbers.
```
num hours_spent -> 100
```
###### Strings
Strings are delimited by `""` quotes. There's no other way to indicate a string. You may escape special characters with `\`, like `\"` if you want to show a quote, or `\{` if you want to open a bracket. Additionally, `\n` is a newline (Which will probably mess with the program's output) and `\t` is a tab.

Strings can also contain arbitrary values in them, which will be converted to string format when printed onto the screen. To do this, simply write the value in curly brackets inside the string, like so: 
```
str some_string -> "String string string {some_variable} string string string {100003}"
```
`some_variable` will be evaluated when the string has to be printed, and so will the number `100003`. If `some_variable` was, say, `1`, the string in the end will be `String string string 1 string string string 100003`.
###### Booleans
Booleans hold a true/false value, or a probability. If they hold a probability, it will be evaluated once when needed. 
`true` and `always` are keywords that signify a value of 1, or true. On the other hand, `false` and `never` mean 0, or false. These keywords always evaluate to the same.

Probabilities exist in 2 main forms, percentages and [...not percentages](# "Note to self, think of name for these"). 
Percentages are written, as would be expected, in the form `X%`, for example, `50%` or `0%` or `8.3762%`. They will evaluate to true `X` percent of the time, that is, `X` times out of 100. 
Non-percentages are written in the form `X^`, for example `50^`, `2^` or `1.004^`. They will evaluate to true `100/X` percent of the time, that is, 1 out of `X` times they're evaluated.
Additionally, every value in a program can evaluate to either true or false. The number zero, an empty string, the boolean value `false` and the empty list evaluate to false. Everything else evaluates to true.
```
bool warm -> 80%
```
###### Lists
Lists hold several values of the same type at once. To make a list, simply open square brackets, write your values separated by commas, and then close square brackets, like so:
```
list colors -> ["Red", "Blue", "Yellow", "Green"]
```
Ta-dah. Lists can hold other lists as well, as long as the lists are of the same type: 
```
list list_of_lists -> [[0,1,2,3], [4,5,6,7]]
```
You might be tempted to declare an empty list with `[]`, but that won't work (Since there's absolutely no reason for empty lists). Don't try it, you'll make the interpreter sad.
To access a list element, index it by writing the element number between brackets after the list, like `colors[0]`. Lists are 0-indexed, of course (As it should be). There's other ways to access elements discussed [here](#List operators).
List elements may also have a probability assigned to them, for use with the pick operators. To assign a probability to an element of a list, write `~ probability` after it, like so:
```
list colors_biased -> ["Red" ~ 5%, "Blue" ~ 85%, "Yellow" ~ 5%, "Green" ~ 5%]
```
Probabilities count towards the list type (That is, you cannot mix elements with and without probabilities in a list, or have a list of lists, some with probabilities, some without)
###### Enumerations
Enumerations are simply a convenient way of storing arbitrary values that are related. To make one, simply open brackets, write your enumeration elements separated by commas, then close brackets, like so:
```
enum colors -> {RED, BLUE, YELLOW, GREEN}
```
Then you can access any of the values like so: `enum_name.enum_value`, for example, `colors.RED`.
Enumerations can be made into lists with a *(TODO: THINK OF NAME !important)* expression. You do that like so: `enum_name{prob1, prob2 ... probn}`, where n is the amount of values the enum holds.This makes a list where the first value of `enum_name` has `prob1`, the second value has `prob2`, etc. This is convenient if you want to pick.
```
list colors? -> colors{5%, 85%, 5%, 5%}
```
###### Enumeration Values
Enumeration values hold, well, enumeration values. They may only be assigned values from an enumeration. Pretty easy, right? 
```
val color -> colors.BLUE
```
### Operators
Islang has a bunch of operators, so for convenience I'll divide them in 3 categories: Boolean operators (which return a boolean), List operators (which operate on list) and other operators (Whose function is different from the others)
###### Boolean operators
`x` and `y` represent arbitrary values.
- `and`: And. The logical and, used like so: `x and y`. It will return true if both `x` and `y` evaluate to true, and false otherwise. 
- `or`: Or. The logical or, used like so: `x or y`. It will return true if either `x` or `y` evaluates to true, and false otherwise.
- `not`: Not. The logical negator, used like so: `not x`. It will return true if `x` evaluates to false, and false if `x` evaluates to true.
- `=`: Equals. The equality check, used like so: `x = y`. It will return true if `x` and `y` are both the same type and evaluate to the same value, and false otherwise. List are deep-compared element by element. 
- `~=`: Not equals. The inequality check, used like so: `x ~= y`. It will return true if `x` and `y` are either different types or do not evaluate to the same value, and false otherwise. List are deep-compared element by element.

###### List operators
`x` and `y` represent arbitrary values. `l` represents an arbitrary list.
- `*`: Pick. The biased, unexclusive pick, used like so: `l*x` or `x*l` or `*l` or `l*`. It will pick `x` elements from `l`, according to `l`'s element's probabilities if they have any, or randomly if they don't. If `x` is 1, it will return the picked value, if it's greater than 1 it will return a list with all chosen values. If `x` is not present, only 1 element will be picked.
*TODO FINISH LANGUAGE SYNTAX AND THOSE SHENANIGANS*

## CLI
* `run`: Will run the interpreted program once
* `exit`: Will exit the program

## TODO
- ~~Add examples~~
- Fix the ecbn spec
- Fix the showing of the parser errors, they're all over the place
- Pretty sure there's a memory leak or two somewhere
- Add CLI interpreting while running the program
- Add CLI reading of files if none specified
- Add CLI everything, actually
- Picking from enums directly
- Convert lists with `identifier{probabilities}` syntax
- Allow repr and opts to take variables, or any value, really
- Test how good the probabilities are
- Fix empty blocks showing text on the console
- Fix errors when lines are too damn long
- Stupid-proof reading of files in `Scanner.cpp` (Not that I'd ever read myself out of memory or anything...)
- Ctrl+C support
- ~~Fix this random access error? Where is this even coming from?~~
- Fix program structure being fixed
- Think of a name for non-percentages
- Think of a name for the magical enum-to-list expression
- Make enum-to-list work without percentages, somehow