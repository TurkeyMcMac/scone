#include <test-template.h>

#define KEYSIZE_MAX 5
#define VALSIZE_MAX 1

#define N_KEYS 4

const char *const keys[] = {
	"key 0",
	"key 1",
	"key 2",
	"key 3"
};

CREATE_TEST("front-pad", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i;
	for (i = 0; i < N_KEYS; ++i) {
		assert(scone_read(conf, &keynum, value, &valsize) == 0);
		assert(keynum == i);
	}
	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
