/* Test that the right errors are thrown at the right times. */

#include <test-template.h>

#define KEYSIZE_MAX 4
#define VALSIZE_MAX 1

#define N_KEYS 1

 const char *const keys[] = {
	"key"
};

CREATE_TEST("errors", conf)
{
	char value[VALSIZE_MAX + 1];
	size_t keynum, valsize;
	size_t i;

	/* BAD_KEY */
	for (i = 0; i < 3; ++i)
		assert(scone_read(conf, &keynum, value, &valsize)
			== SCONE_BAD_KEY);
	/* NO_VALUE */
	for (i = 0; i < 10; ++i)
		assert(scone_read(conf, &keynum, value, &valsize)
			== SCONE_NO_VALUE);
	/* LONG_KEY */
	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_LONG_KEY);
	/* LONG_VALUE */
	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_LONG_VALUE);

	assert(scone_read(conf, &keynum, value, &valsize) == SCONE_DONE);
}
