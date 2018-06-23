#include "scone.h"

enum {
	KEY_BAR,
	KEY_BAZ,
	KEY_FOO,

	N_KEYS
};

const char *const keys[] = {
	[KEY_FOO] = "f oo",
	[KEY_BAR] = "bar",
	[KEY_BAZ] = "bazz",
};

#define KEYSIZE_MAX 4
#define VALSIZE_MAX 7

void study_config(struct scone *conf)
{
	char value[VALSIZE_MAX + 1];
	size_t valsize;
	size_t keynum;
	int status;
	while (1) {
		switch (status = scone_read(conf, &keynum, value, &valsize)) {
		case 0:
			value[valsize] = '\0';
			printf("Line %u: '%s' = '%s'\n",
				conf->line, keys[keynum], value);
			break;
		case SCONE_DONE:
			goto done;
		case SCONE_BAD_KEY:
			printf("Line %u: unknown key\n", conf->line);
			break;
		case SCONE_NO_VALUE:
			printf("Line %u: missing value\n", conf->line);
			break;
		case SCONE_LONG_KEY:
			printf("Line %u: key over %lu bytes long\n",
				conf->line, conf->keysize_max);
			break;
		case SCONE_LONG_VALUE:
			printf("Line %u: value over %lu bytes long\n",
				conf->line, conf->valsize_max);
			break;
		}
	}
done:
	printf("status: %d\n", status);
}

int main(void)
{
	struct scone conf;
	char keybuf[KEYSIZE_MAX];

	conf.keys = keys;
	conf.n_keys = N_KEYS;
	conf.keybuf = keybuf;
	conf.keysize_max = KEYSIZE_MAX;
	conf.valsize_max = VALSIZE_MAX;

	scone_open(&conf, "config");
	study_config(&conf);
	scone_close(&conf);
}
