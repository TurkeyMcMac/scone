name = scone
version = $(shell cat version)
lib-name = lib$(name).so.$(version)
lib-path = $(shell pwd)/$(lib-name)
lib-dir = /usr/lib
include-dir = /usr/include
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

.PHONY: install
install: $(include-dir)/scone/scone.h $(lib-dir)/lib$(name).so

$(include-dir)/scone/scone.h: scone.h
	sudo mkdir -p $(include-dir)/scone
	sudo cp -f $< $@

$(lib-dir)/lib$(name).so: $(lib-dir)/$(lib-name)
	sudo ln -fsT $< $@

$(lib-dir)/$(lib-name): version
	sudo cp -f $(lib-name) $(lib-dir)

.PHONY: clean
clean:
	$(RM) $(lib-name) $(tests)
