name = scone
version = $(shell cat version)
lib-name = lib$(name).so.$(version)
flags = -ansi -Wall -Wextra -Wpedantic $(CFLAGS)
optimize = 3
tests = $(patsubst %.c, %.test, $(wildcard tests/*.c))

$(lib-name): scone.c scone.h
	$(CC) $(flags) -O$(optimize) -fPIC -c $< -o scone.o
	$(CC) $(flags) -O$(optimize) -shared -fPIC -o $(lib-name) scone.o
	rm scone.o

build-tests: $(tests)

tests/%.test: tests/%.c test-template.h scone.h
	$(CC) $(flags) -I. -L. -o $@ $< -l:./$(lib-name)

.PHONY: clean
clean:
	$(RM) $(lib-name) $(tests)
