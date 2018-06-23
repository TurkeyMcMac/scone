#include <test-template.h>

#define KEYSIZE_MAX 3
#define VALSIZE_MAX 1

#define N_KEYS 1

const char *const keys[] = {
	"key",
};

CREATE_TEST("aft-pad", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i;
	for (i = 0; i < 6; ++i) {
		assert(scone_read(conf, &keynum, value, &valsize) == 0);
		assert(valsize == 1);
	}
	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
