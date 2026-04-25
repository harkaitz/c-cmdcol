# SCAL

cmdcol - Print the output of commands in columns.

# Synopsis

    cmdcol [-RWc:] CMD [COLS] [CMD [COLS] [...]]

# Description

The cmdcol utility executes the specified commands and prints their
output in columns. The number of columns can be specified for each
command, or it will be determined automatically based on the terminal
width.

The escape sequences in the output of the commands are preserved, allowing for colored output and
are also used to determine the width of the output for proper column alignment.

The following options are available:

- *-R*      Do not remove ANSI escape sequences from the output when piping.
- *-W*      Do not wrap the output to terminal width.
- *-c COL*  Specify the terminal width.

Examples:

    $ cmdcol "scal -c" 25 "rem | sed '3,\${ /^\$/d }'"
    
         April 2026          Reminders for Tuesday, 28th April, 2026 (today):
    Mo Tu We Th Fr Sa Su
           1  2  3  4  5     Do your taxes (8 days)
     6  7  8  9 10 11 12     Update OpenBSD server (373 days ago!)
    13 14 15 16 17 18 19
    20 21 22 23 24 25 26
    27 28<29 30

# Authors

Harkaitz Agirre Ezama <harkaitz.aguirre@gmail.com>.

# Collaboration

Feel free to open bug reports and feature/pull requests.

More software like this here:

1. [https://harkadev.com/prj/](https://harkadev.com/prj/)
2. [https://devreal.org](https://devreal.org)

# See also

**SCAL(1)**, **REMIND(1)**
