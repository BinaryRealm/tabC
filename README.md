# tabC
This is the C with Python-style-syntax preprocessor (tabC) (Version 0.1)

## Updates

* *Version 0.2 is currently under development*
* *tabC 0.2 is a major overhaul of the previous version, and it will:*
  * *have a more intuitive syntax*
  * *provide syntax error feedback*
  * *handle singleline and multiline comments seamlessly*
  * *allow the user to write multiline statements*

## Command Format

* `tabc [-h -help --help]` (prints help info)

* `tabc [input file source]` (processes input file and outputs c file to stdout)

* `tabc [input file source] [output file source]` (processes input file and outputs C file to specified output files)

The preprocessor does not require any specific file extension for the input and output files.

As an example of integration with a C compiler, the user can enter

* `tabc code.tc | gcc -o code -xc -`, which will directly generate an executable.

## Directives

#### The `<$>` and `::` Directives

* `[0+ tabs] <$> [single line]`

A line with this symbol in front of it is completely unprocessed (meaning no semicolon appended to the end of the line or if there are C single line comments, they are not prepended with a semicolon).


* `[single line] ::` 

The `::` is equivalent in behavior to the `<$>` directive (meaning no processing occurs on the line, except that it is placed at the end of a line of code instead of the start.

*Currently*, If a line happens to have `<$>` at the start and `::` at the end, and no line comment directly after `::` they will both be removed.

*Currently*, `<$>` is better and safer than `::` (which is a directive under development), despite the greater ease of typing `::`.   If you want comments with no restrictions on that line, use `<$>`.

Examples:

1. `[ int main(void):: ] -> [ int main(void) ]`

2. `[ <$>int main(void):: ] -> [ int main(void) ]`

3. `[ int main(void):://comment ] -> [ int main(void)//comment ]`

4. `[ int main(void)::  //comment ] -> [ int main(void)  ;//comment ]` (Error)

5. `[ <$>int main(void):://comment ] -> [ int main(void):://comment ]` (Error)

6. `[ int main(void)::(whitespace)] -> [ int main(void)::(whitespace);]` (Error)



#### The `<$$></$$>` Directive

```
<$$> [newline]
[unprocessed multiline code]
</$$> [newline]
```
* All lines between the tags are unmodified in output.  
* Note that this no-process directive has the highest priority, meaning any '<$>' and '::' directives in the line will appear in the output.  
* Not only are lines not processed but indentations are not converted to brackets as well.  
* The slash in the closing tag is mandatory to end no processing.  
* There cannot be any whitespace in front of the opening and closing tags, or it will be treated like C code.  
* Also, the newline after the tags is mandatory, or the tag will not not be treated as a barrier and will instead be treated like C code.  
* If there is an opening tag (or several of them) but no closing tag, everything will be unprocessed to the end of the file.  A lone closing tag will simply disappear in output C code.

## Keyword Behavior

Keywords (if at the front of the line) that cause the line to be unprocessed by default (meaning a semicolon to not be placed at the end or in front of the line comment within the line):

* `if `
* `else `
* `for` 
* `while `
* `do` 
* `switch` 
* `case `
* `default`
* `#...` (for C preprocessor)
* `//...` (comments)
* `"...` (for a multiline string)
* `enum`
* `struct`
* `union`

These keywords are only recognized if there are >=0 tabs preceding it and there is a space, '(', or newline after it.  As with directives, any extra non-tab spaces in front of it would make it unrecognized.

#### The ':' (label) keyword

* ':' for C labels acts similarly to the tabC `::` directive in the sense that a semicolon is not appended after it, but a semicolon is also not placed in front of a line comment that is placed directly after a ':'.

* *Currently*, what this means is that if by some chance there is a critical need for a single line comment after a C label, there can not be any spaces between the label and the comment, or else the comment will get a semicolon placed in front of it, causing an error.  If you want spaces, use `<$>` to override processing the line.  See comment examples below.

## Line and Multiline Comments

* C multiline comments are not implemented yet.  Of course they can still be used, but expect appended semicolons.  To safely use them, wrap them with the no-process directive tags (`<$$></$$>`).

* C Line comments can be used.  However, it should be known that unless the comment line symbol is in between two string quotes like this: `"...//...\"` or is immediately after a C label or `::` tabC directive, a semicolon will be prepended to the comment.  

* If the line comment is at the start of a line, it is treated the same as any other special keyword (in being unprocessed completely).

*Currently*, whitespace after a C label or a `::` directive will cause a semicolon to be printed on the line, which will cause an error in compilation.  See comment examples below.

*Currently*, if there is a comment in a string, and there is also a line comment on that line, the real comment is ignored and a semicolon is appended after it, instead of in front of it.  So either use the `<$>` directive or write the comment above that line, for example.  See comment examples below.

#### Comment Examples
1. `[ return 1//exits the function ] -> [ return 1;//exits the function) ]`
2. `[ printf("//test") ] -> [ printf("//test"); ]`
3. `[ printf("//test")//comment] -> [ printf("//test")//comment;]` (this will cause an error)
4. `[ label://comment ] -> [ label://comment ]`
5. `[ label:(whitespace)//comment ] -> [ label:(whitespace);//comment]` (this will cause an error)


## How to Write tabC Code

Firstly, never place semicolons at the end of a statement.  Each statement should be on a new line.  If it is needed to have multiple statements on one line, then semicolons will still be needed in between them, but not at the end.

Secondly, do not use curly brackets.  When opening a new block, use a newline and then a tab.  Unlike Python, If statements, loops, and other block statements do not need colons after them, because tabc recognizes the keywords.  However, declaring a C function requires a `<$>` at the front or a `::` at the end to not be treated as a standard line of code.  If one of these two directives is not placed, the function header will get semicolons placed accordingly.  That is why when prototyping a function, all that needs to be done is declare it without a semicolon as usual.

Regarding line comments and multiline comments, the behavior is slightly different than in standard C, and reading the above explanations will clarify the differences.

In conclusion, using tabC should be very simple, and testing it out for a few minutes hopefully should be enough to understand how to use it efficiently.  If there happen to be any bugs for now, using `<$>` will fix most of them immediately.

## A Note on Text Editors and File Extensions

When possible, using a file extension such as '.tc' or '.tabc' can make it easier to distinguish from standard '.c' files.  However, if your text editor does not handle customizing, there may not be much choice but to use '.c'.  One possible solution is to use a configurable text editor such as vim that allows new file extensions to be recognized.  However there are still no official plugins or configurations for how to use it in text editors, such as vim.  When configuring, C syntax highlighting and using true tabs instead of spaces makes programming easier.  Depending on the editor, shortening the displayed width of tabs can improve readability.  For the vim editor an example configuration for a ~/.vimrc file is shown below.


#### Example ~/.vimrc Configuration for the vim Text Editor Using tabC
```
set nocompatible
set ruler
set number
set showcmd
set autoindent
set history=50
colorscheme desert
if &t_Co > 2 || has(\"gui_running\")
  syntax on
  set hlsearch
endif
filetype plugin indent on
set noexpandtab
set shiftwidth=0
set tabstop=4
"tabc custom file type (treat like C)
au BufNewFile,BufRead *.c,*.tc 		call filetype#FTlpc()
set mouse=a
```
