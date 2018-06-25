/* Test that escaped hex bytes have correct values. */

#include <test-template.h>

#define KEYSIZE_MAX 3
#define VALSIZE_MAX 1

#define N_KEY_PAIRS 3

#define N_KEYS 2

const char *const keys[] = {
	"\t", "key"
};

CREATE_TEST("escape-hex", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynums[2], valsizes[2];
	size_t i;

	for (i = 0; i < N_KEY_PAIRS; ++i) {
		assert(scone_read(conf, &keynums[0], value, &valsizes[0]) == 0);
		assert(scone_read(conf, &keynums[1], value, &valsizes[1]) == 0);
		assert(keynums[0] == keynums[1]);
		assert(valsizes[0] == valsizes[1]);
	}

	assert(scone_read(conf, &keynums[0], value, &valsizes[0])
		== SCONE_DONE);
}
