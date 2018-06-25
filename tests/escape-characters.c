/* Test that escaped characters are treated like other characters. */

#include <test-template.h>

#define KEYSIZE_MAX 2
#define VALSIZE_MAX 2

#define KEYS_PER_SET 2
#define N_KEY_SETS 7

#define N_KEYS (KEYS_PER_SET * N_KEY_SETS)

const char *const keys[] = {
	"\xFa", "\xFa\xFa",
	"\t", "\t\t",
	"\n", "\n\n",
	"\r", "\r\r",
	" ", "  ",
	"#", "##",
	"=", "==",
};

const size_t lengths[KEYS_PER_SET] = {1, 2};

CREATE_TEST("escape-characters", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i, j;

	for (i = 0; i < N_KEYS; i += j) {
		for (j = 0; j < KEYS_PER_SET; ++j) {
			assert(scone_read(conf, &keynum, value, &valsize) == 0);
			assert(keynum == i + j);
			assert(valsize == j + 1);
		}
	}

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
