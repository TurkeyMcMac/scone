name = scone
lib-name = lib$(name).so
optimize = 3
tests = $(patsubst %.c, %, $(wildcard tests/*.c))

$(lib-name): scone.o
	$(CC) $(CFLAGS) -O$(optimize) -shared -fPIC -o $(lib-name) scone.o

scone.o: scone.c scone.h
	$(CC) $(CFLAGS) -O$(optimize) -fPIC -c $< -o $@

build-tests: $(tests)

tests/%: tests/%.c $(lib-name)
	$(CC) $(CFLAGS) -I. -L. -o $@ $< -lscone
