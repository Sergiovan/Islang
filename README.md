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
* [Imports](#import)
* [Global declarations](#variables)
* [Main location](#blocks)
* [Generate statement](#generate)

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
To access a list element, index it by writing the element number between brackets after the list, like `colors[0]`. Lists are 0-indexed, of course (As it should be). There's other ways to access elements discussed [here](#list_operators).
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

### Blocks
The main part of an Islang program takes part in blocks. They contain all the "logic" to your program. There's 3 types of blocks, which all work exactly the same, and are only different in name and representation (As each will show its own type name on the console). These are: `location` blocks, `event` blocks and `contains` blocks.
To declare a block, you may do it like so: 
```
block_type block_name (appear_chance):
```
Appear chance may be ommitted, and if the block is empty, the colon at the end may also be ommited.
A block spans all consecutive statements that have the same indentation, which also has to be more indented than the block declaration line.
All variables declared inside a block belong to that block, and to access them you need to prepend the variable name with the block name and a dot, like this: `block_name.variable_name`. If the block belongs to another block, then you prepend that with the parent block, and so forth. As an exception, variables in parent blocks can be accessed without any prefixes, as long as no variable with the same name exists in a block before it. 
```
location blah:
    num pi -> 3.14
    location blah_blah:
        num pi -> 3.2
        location here:
            str pi_str -> "{pi}" # Will find blah.blah_blah.pi and will become "3.2"
            str pi_str_2 -> "{blah.pi}" # Will find blah.pi and will become "3.14"
            num pi -> 4
            str pi_str_3 -> "{pi}" # Will find blah.blah_blah.here.pi and will become "4"
    location there:
        # str other_str -> pi_str # This is an error
        str other_str -> blah.blah_blah.here.pi_str # This works fine
```

Blocks also have internal properties that affect how they are shown in the console. They can be modified using the [block keywords](#block_keywords). These are:
- Name: The name the block will show in the console, if not set, the block's name will be the name you gave it in the program.
- Appear chance: The chance the block and blocks belonging to this block will appear when showing the program. As mentioned, it can also be specified when declaring the block, in parenthesis: `location name(50%):`. The default is `100%`, or to always appear.
- Color: The color the block and the associated strings will show on the console. The default is the console's default color.

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
- `*`: Pick. The biased pick, used like so: `l*x` or `x*l` or `*l` or `l*`. It will pick `x` elements from `l`, according to `l`'s element's probabilities if they have any, or randomly if they don't. If `x` is 1, it will return the picked value, if it's greater than 1 it will return a list with all chosen values. If `x` is not present, only 1 element will be picked.
-`/`: Xpick. The biased, exclusive pick, used like so: `l/x` or `x/l` or `/l` or `l/`. It works exactly like the pick operator, except if it were to take more than one element it'll never pick the same element twice. As a consequence, you may never xpick more elements from a list than the size of the list.
- `**`: Upick. The unbiased pick, used like so: `l**x` or `x**l` or `**l` or `l**`. It functions exactly like the pick operator, except it will ignore probabilities and always pick random elements.
- `//`: Uxpick. The unbiased exclusive pick, used like so: `l//x` or `x//l` or `//l` or `l//`. Does the exact same thing as the xpick operator, except it will ignore probabilities.

###### Other operators
- `::`: The special scope, or magic operator. It lets you access color names and other language built-ins. Use it like so: `::builtin_name`.
- `->`: Is. Used to assign a value to a variable. Normally only used when declaring variables, as they need to be initialized immediately.
- `~`: If. Context sensitive. Inside a list it assigns a probability to a list element. Outside of lists it can be used in cojunction with the `repr` keyword to make it conditional.

### Keywords
##### Global keywords
Global keywords may only be used in the outermost part of the program, or, in the global scope. There's only 2 global keywords.
###### Import
The `import` keyword lets you call upon other files, to "import" their variables and blocks. It is used like so: `import name as alias` or `import "filename" as alias`, where `name` is the name of the file you want to import, without extension (it has to be in the same directory as the file that's being run), `filename` is the full name of a file to import (Can be a path, relative or absolute), and `alias` is the name with which to import the content, as if it were a block. If `as alias` is ommited, the content will be imported under the filename, without extension and without the path. 
```
/* pi.isl */
num pi -> 3.14

/* colors.isl */
list colors -> ["Red", "Blue", "Yellow", "Green"]

/* main.isl */
import "pi.isl"
import colors as color_list

num re_pi -> pi.pi
list re_colors -> color_list.colors
```

###### Generate
The `generate` keyword is placed at the very end of a program, and it tells the program which block is the main block when it needs to print the program to the console. It is used like so: `generate block_name`, where `block_name` is the name of a block.

###### Block keywords
- `name`: To set the name of the block. Used like `name "Name of the block"`. Setting the name more than once per block is not allowed
- `color`: To set the color of the block. Used like `color ::color_name`. Setting the color more than once per block is not allowed.
- `appear`: To set the appearance chance of the block, if not already specified. Used like `appear chance`, where chance is a probability or a not-probability. Setting it more than once per block is not allowed.
- `use`: To "import" variables or blocks into the current block. It can be used like so: `use var_or_block as alias`, where `var_or_block` is the name of a variable or a block, and `alias` is the name you want to use it as. If the alias is not present, it will be accessible by its original name, as if it were in the current scope.
  ```
  location main:
      location sub:
          num phi -> 7
      location sib:
          use main.sub.phi as phi
          use main.sub as sibling
          num phi_1 -> main.sub.phi # Can do
          num phi_2 -> pho # Can do too, using the aliased version
          num phi_3 -> sibling.phi # Also allowed
  ```
- `repr`: The `repr` keyword (Stands for "representation") is used to show text on the console whenever the block it belongs to is printed. It's used like so `repr "Some string" ~ if_expr`, where `if_expr` is a value that evaluates to a boolean. If `~ if_expr` is not present, the string will always be printed to screen, otherwise it will only be printed if `if_expr` evaluates to true. 
- `else`: Always used in conjunction with `repr`, and always used right after a conditional repr statement. Used like so `else repr "Some string" ~ if_expr`. An else repr statement will only be evaluated if the previous repr statement's `if_expr` was evaluated, **and** it evaluated to false. If the previous repr's statmenent condition was not evaluated (Which can happen if the previous statement is also an else repr statement) it won't be evaluated. This means that, for a group of consecutive else repr statements, plus the preceding single repr statement, only one will ever be printed to string.
  ```
  repr "12 + 144 + 20" ~ 10%         # Only one
  else repr "+ 3 * sqrt(4)" ~ 20%    # of these
  else repr "/ 7 + 5 * 11" ~ 30%     # will be 
  else repr "9² + 0" ~ 40%           # eventually printed
  else repr "How's that for poetry?" # to console
  ```
- `opts`: The opts keyword (Stands for "options") works similarly to `repr`, except it takes a list of strings (Of which it will print one as if picked by the `pick` operator). It cannot, however, have a condition attached. It is used like so: `opts ["Option 1", "Option 2", "Option 3"]`

## CLI
* `run`: Will run the interpreted program once
* `exit`: Will exit the program

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
