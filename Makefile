name = scone
version = 0.0.0
lib-name = lib$(name).so.$(version)
flags = -ansi -Wall -Wextra -Wpedantic $(CFLAGS)
optimize = 3
tests = $(patsubst %.c, %, $(wildcard tests/*.c))

$(lib-name): scone.o
	$(CC) $(flags) -O$(optimize) -shared -fPIC -o $(lib-name) scone.o

scone.o: scone.c scone.h
	$(CC) $(flags) -O$(optimize) -fPIC -c $< -o $@

build-tests: $(tests)

tests/%: tests/%.c test-template.h $(lib-name)
	$(CC) $(flags) -I. -L. -o $@ $< -l:./$(lib-name)

.PHONY: clean
clean:
	$(RM) scone.o $(lib-name) $(tests)
