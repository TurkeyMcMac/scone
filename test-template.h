/* This file is intended for use by tests to decrease redundancy in the test
 * code. the integers KEYSIZE_MAX, VALSIZE_MAX, and N_KEYS, the array keys must
 * be present for this template to work. */

#include <assert.h>
#include <scone.h>

/* Declare a function to run in the tests. The user is left to fill in the body.
 * Arguments:
 * 	name: The name of the test. The string given will be concatenated in the
 * 		sequence "tests/" name ".scone", and will be opened in the test.
 * 	conf_: The name of the argument to the function which will be declared.
 */
#define CREATE_TEST(name, conf_)				\
static void test_config(struct scone *);			\
int main(void)							\
{								\
	struct scone conf;					\
	char keybuf[KEYSIZE_MAX];				\
								\
	conf.keys = keys;					\
	conf.n_keys = N_KEYS;					\
	conf.keybuf = keybuf;					\
	conf.keysize_max = KEYSIZE_MAX;				\
	conf.valsize_max = VALSIZE_MAX;				\
								\
	assert(scone_open(&conf, "tests/"name".scone") == 0);	\
	test_config(&conf);					\
	assert(scone_close(&conf) == 0);			\
	return 0;						\
}								\
static void test_config(struct scone *conf_)
