#include <assert.h>
#include <scone.h>

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
}								\
static void test_config(struct scone *conf_)
