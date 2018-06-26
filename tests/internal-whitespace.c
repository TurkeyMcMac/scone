/* Test that whitespace within strings is preserved. */

#include <test-template.h>

#define KEYSIZE_MAX 7
#define VALSIZE_MAX 12

#define N_KEYS 3

const struct scone_key keys[] = {
	{"k  e	 y", 7},
	{"ke    y", 7},
	{"key", 3}
};

CREATE_TEST("internal-whitespace", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;

	assert(scone_read(conf, &keynum, value, &valsize) == 0);
	assert(keynum == 0);
	assert(valsize == 12);
	assert(scone_read(conf, &keynum, value, &valsize) == 0);
	assert(keynum == 1);
	assert(valsize == 7);
	assert(scone_read(conf, &keynum, value, &valsize) == 0);
	assert(keynum == 2);
	assert(valsize == 5);

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
