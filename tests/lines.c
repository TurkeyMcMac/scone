/* Test that lines are counted properly. */

#include <test-template.h>

#define KEYSIZE_MAX 5
#define VALSIZE_MAX 1

#define N_KEYS 4

const struct scone_key keys[] = {
	{"key 0", 5},
	{"key 1", 5},
	{"key 2", 5},
	{"key 3", 5}
};

CREATE_TEST("lines", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;

	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 2);
	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 3);
	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 6);
	scone_read(conf, &keynum, value, &valsize);
	assert(conf->line == 8);

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
