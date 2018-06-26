/* Test that escaped line breaks are ignored. */

#include <test-template.h>

#define KEYSIZE_MAX 3
#define VALSIZE_MAX 5

#define N_KEYS 1

const struct scone_key keys[] = {
	{"key", 3}
};

CREATE_TEST("escape-line-breaks", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i;

	for (i = 0; i < 4; ++i) {
		assert(scone_read(conf, &keynum, value, &valsize)
			== 0);
		assert(valsize == 5);
	}

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
