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

CREATE_TEST("lines", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i;

	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 1);
	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 2);
	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 4);
	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 6);

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
