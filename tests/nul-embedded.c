/* Test that NUL characters can be embedded in string literals. */

#include <test-template.h>

#define KEYSIZE_MAX 3
#define VALSIZE_MAX 200

#define N_KEYS 4

const struct scone_key keys[] = {
	{"\x00", 1},
	{"\x00\x00", 2},
	{"\x00\x00\x00", 3},
	{"key", 3}
};

CREATE_TEST("nul-embedded", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i;

	for (i = 0; i < N_KEYS; ++i) {
		assert(scone_read(conf, &keynum, value, &valsize) == 0);
		assert(keynum == i);
		assert(valsize == 1);
	}

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
