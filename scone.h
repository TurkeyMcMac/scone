#include <stddef.h>
#include <stdio.h>

#ifndef _SCONE_STRUCT
#define _SCONE_STRUCT
struct scone {
	FILE *file;
	const char *const *keys;
	char *keybuf;
	size_t valsize_max, keysize_max;
	size_t n_keys;
	unsigned line;
};
#endif /* !defined(_SCONE_STRUCT) */

#define SCONE_COMMENT '#'
#define SCONE_BINDING '='

#define SCONE_DONE	1
#define SCONE_BAD_KEY	2
#define SCONE_NO_VALUE	3
#define SCONE_LONG_KEY	4	
#define SCONE_LONG_VALUE 5

int scone_open(struct scone *self, const char *name);

int scone_read(struct scone *self, size_t *key, char *value, size_t *valsize);

int scone_close(struct scone *self);
