This is a simple parsing library for simple configuration. It parses no types
nor hierarchy, so you'll have to do that stuff yourself. All that exist are
key-value pairs which are iterated through sequentially.

Details of the API can be found in scone.h. I've tried to keep the API quite
simple.

The entire project should be able to be built in ANSI C with the warning options
-Wall, -Wextra, and -Wpedantic enabled.

To build/run the tests, use the run-tests shell script.
