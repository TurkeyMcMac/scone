/* Test that empty keys are considered valid. */

#include <test-template.h>

#define KEYSIZE_MAX 1
#define VALSIZE_MAX 1

#define N_KEYS 1

const char *const keys[] = {
	""
};

CREATE_TEST("empty-keys", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i;

	for (i = 0; i < 5; ++i)
		assert(scone_read(conf, &keynum, value, &valsize) == 0);

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);

	assert(conf->line == 8);
}
