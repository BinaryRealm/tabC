#define HELP_MESSAGE "\nThis is the C with Python-style-syntax preprocessor (tabC) (Version 0.1)\n"\
"========================================================\n"\
"COMMAND FORMAT:\n\n"\
"tabc [-h -help --help] (prints help info)\n"\
"tabc [input file source] (processes input file and outputs c file to stdout)\n"\
"tabc [input file source] [output file source] (processes input file and outputs C file to specified output files)\n"\
"\nThe preprocessor does not require any specific file extension for the input and output files\n"\
"As an example of integration with a C compiler, the user can enter\n"\
"\"tabc code.tc | gcc -o code -xc -\", which will directly generate an executable.\n"\
"========================================================\n"\
"DIRECTIVES:\n\n"\
"--------------------------------------------------------\n"\
"[0+ tabs] <$> [single line]\na line with this symbol in front of it is completely unprocessed (meaning no semicolon appended to the end of the line or if there are C single line comments, they are not prepended with a semicolon).\n"\
"--------------------------------------------------------\n"\
"[single line] ::\nThe '::' is equivalent in behavior to the '<$>' directive (meaning no processing occurs on the line, except that it is placed at the end of a line of code instead of the start.\n"\
"Exceptional behavior: If a line happens to have '<$>' at the start and '::' at the end, and no line comment directly after '::' they will both be removed.  If a line comment is directly after it, then its priority is decreased compared to '<$>', meaning the line will be unprocessed completely after the '<$>'.\n"\
"Examples:\n"\
"[ int main(void):: ] -> [ int main(void) ]\n"\
"[ <$>int main(void):: ] -> [ int main(void) ]\n"\
"[ int main(void):://comment ] -> [ int main(void)//comment ]\n"\
"[ int main(void)::  //comment ] -> [ int main(void)  ;//comment ] (Error)\n"\
"[ <$>int main(void):://comment ] -> [ int main(void):://comment ] (Error)\n"\
"[ int main(void)::          ] -> [ int main(void)::         ;] (Error)\n"\
"\nMoral: '<$>' is better and safer than '::' (which is a directive under development) , despite the greater ease of typing'::'.   If you want comments with no restrictions on that line, use '<$>'.\n"\
"--------------------------------------------------------\n"\
"<$$> [newline]\n[unprocessed multiline code]\n</$$> [newline]\n"\
"\nAll lines between the tags are unmodified in output.  Note that this no-process directive has the highest priority, meaning any '<$>' and '::' directives in the line will appear in the output.  Not only are lines not processed but indentations are not converted to brackets as well.  The slash in the closing tag is mandatory to end no processing.  There cannot be any whitespace in front of the opening and closing tags, or it will be treated like C code.  Also, the newline after the tags is mandatory, or the tag will not not be treated as a barrier and will instead be treated like C code.  If there is an opening tag (or several of them) but no closing tag, everything will be unprocessed to the end of the file.  A lone closing tag will simply disappear in output C code.\n"\
"========================================================\n"\
"SPECIAL KEYWORDS:\n"\
"\nKeywords (if at the front of the line) that cause the line to be unprocessed by default (meaning a semicolon to not be placed at the end or in front of the line comment within the line): \n"\
"if\nelse\nfor\nwhile\ndo\nswitch\ncase\ndefault\n# (for C preprocessor)\n//\n\" (for a multiline string)\nenum\nstruct\nunion\n\n"\
"\nThese keywords are only recognized if there are >=0 tabs preceding it and there is a space, '(', or newline in after it.As with directives, any extra non-tab spaces in front of it would make it unrecognized.\n"\
"--------------------------------------------------------\n"\
"The ':' keyword: \n  ':' for C labels acts similarly in the sense that a semicolon is not appended after it, but a semicolon is also not placed in front of a line comment that is placed directly after a ':'.\n"\
"What this means is that if by some chance there is a critical need for a single line comment after a C label, there can not be any spaces between the label and the comment, or else the comment will get a semicolon placed in front of it, causing an error.  If you want spaces, use <$> to override processing the line.  See comment examples below.\n"\
"========================================================\n"\
"LINE AND MULTILINE COMMENTS:\n"\
"\nC multiline comments are not implemented yet.  Of course they can still be used, but expect appended semicolons.  To safely use them, wrap them with the no-process directive tags.\n"\
"\nC Line comments work visually exactly the same.  However, it should be known that unless the comment line symbol is in between two string quotes like this: \"...//...\" or is immediately after a ':', a semicolon will be prepended to the comment.  If the line comment is at the start of a line, it is treated the same as any other special keyword (in being unprocessed completely).\n\n"\
"The comment in quotes exception:\n"\
"Though this is the most obscure case that can happen with tabc, if there is a comment in a string, and there is also a line comment on that line, the real comment is ignored and a semicolon is appended after it instead of in front of it.  So either use the no-process directive or write the comment above that line, for example.  See comment examples below.\n\n"\
"Example 1: [ return 1//exits the function ] -> [ return 1;//exits the function) ]\n"\
"Example 2: [ printf(\"//test\") ] -> [ printf(\"//test\"); ]\n"\
"Example 3: [ printf(\"//test\")//comment] -> [ printf(\"//test\")//comment;] (this will cause an error)\n"\
"Example 4: [ label://comment ] -> [ label://comment ]\n"\
"Example 5: [ label:  //comment ] -> [ label:  ;//comment] (this will cause an error)\n"\
"========================================================\n"\
"EXPLANATION OF HOW TO WRITE TABC CODE\n"\
"\nFirstly, never place semicolons at the end of a statement.  Each statement should be on a new line.  If it is needed to have multiple statements on one line , then semicolons will still be needed in between them, but not at the end.  Secondly, do not use curly brackets.  When opening a new block, use a newline and then a tab.  Unlike Python, If statements, loops, and other block statements do not need colons after them, because tabc recognizes the keywords.  However, declaring a C function requires a '<$>' at the front or a '::' at the end to not be treated as a standard line of code.  If one of these two directives is not placed, the function header will get semicolons placed accordingly.  That is why when prototyping a function, all that needs to be done is declare it without a semicolon as usual.  Regarding line comments and multiline comments, the behavior is slightly different than in standard C, and reading the above help explanations will clarify the differences.  In conclusion, programming in tabc is meant to be very self explanatory, and testing it out for a few minutes hopefully should be enough to understand how to use it efficiently.\n\nNote on text editors and file extension:\nThough not mandatory, using the tabc file extension '.tc' will make it easier to distinguish from standard '.c' files.  However, if your text editor does not handle customizing, there may not be much choice but to use '.c'.  A solution is to use a configurable text editor such as vim that allows new file extensions to be recognized.  However, since tabc is still a new format, there is no official plugin or configuration for how to use it in a text editor such as vim.  When configuring, C syntax highlighting should be the same and using true tabs instead of spaces makes programming easier.  Shortening the displayed width of tabs improves readability even further.  For the vim editor, a basic, but usable configuration for a ~/.vimrc file is shown below.\n"\
"========================================================\n"\
"EXAMPLE ~/.vimrc CONFIGURATION FOR VIM TEXT EDITOR WITH TABC\n\n"\
"set nocompatible\n"\
"set ruler\n"\
"set number\n"\
"set showcmd\n"\
"set autoindent\n"\
"set history=50\n"\
"colorscheme desert\n"\
"if &t_Co > 2 || has(\"gui_running\")\n"\
"  syntax on\n"\
"  set hlsearch\n"\
"endif\n"\
"filetype plugin indent on\n"\
"set noexpandtab\n"\
"set shiftwidth=0\n"\
"set tabstop=4\n"\
"\"tabc custom file type (treat like C)\n"\
"au BufNewFile,BufRead *.c,*.tc 		call filetype#FTlpc()\n"\
"set mouse=a\n"\
"========================================================\n"
