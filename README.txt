    ,---/   ,---/  ,---,  /   /  .---/
   /       /      /   /  /|  /  /
  `---.   /      /   /  / | /  /__
      /  /      /   /  /  |/  /
     /  /      /   /  /   /  /
/---*  `---/  `---*  /   /  `---/

OVERVIEW
--------
This is a simple parsing library for simple configuration. It parses no types
nor hierarchy, so you'll have to do that stuff yourself. All that exist are
key-value pairs which are iterated through sequentially.


FILE FORMAT
-----------
Scone files consist only of comments and key-value pairs. Key-value span one
line each and have the following format:

<key> = <value>

Comments can be on the same line as a key-value pair after the end of the value.

Following is an example of a valid file. Letters in parentheses at the
beginnings of lines are not part of this hypothetical file.

(a)	# Known information:
(b)	Name:           John Doe
	Birth date:     02-03-1984
(c)	Height:         8'11"
	Favorite color: yellow

(a) shows the syntax for comments, which extend to the end of the line.

Notice how on line (b) that the value is multiple words. Any string literal,
whether it be a key or a value, can span multiple words. Whitespace between
words is kept, while leading and trailing whitespace is removed.

On line (c), the value contains quotes. These have no special meaning to the
parser, and will be carried through to the program.

Several escape sequences, listed below, are supported. They all begin with a
backslash.

Character  Description
-------------------------------------------------------------------------------
'a'        Bell
'b'        Backspace
'e'        Escape
'f'        Form feed
'n'        New lne
'r'        Carriage return
't'        Horizontal tab
'v'        Vertical tab
newline    Ignore this line break, although still count it in the line counter.
any other  Ignore this character and move on. For example, \# reduces to '#'.


API
---
Details of the API can be found in scone.h. I've tried to keep the API quite
simple.


C STANDARD
----------
The entire project should be able to be built in ANSI C with the warning options
-Wall, -Wextra, and -Wpedantic enabled.


TESTS
-----
To build/run the tests, use the run-tests shell script.
