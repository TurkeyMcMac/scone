#include "scone.h"

#include <errno.h>

int scone_open(struct scone *self, const char *name)
{
	self->file = fopen(name, "r");
	if (!self->file)
		return -errno;
	self->line = 0;
}

static int skip_line(FILE *self, int *ret);
static int eof_retval(FILE *file);
static int parse_pair(struct scone *self,
	int first_ch,
	size_t *key,
	char *value, size_t *valsize,
	int *ret);
int scone_read(struct scone *self, size_t *key, char *value, size_t *valsize)
{
	int ch, ret = 0;
	while (1) {
		switch(ch = fgetc(self->file)) {
		case '\n':
			++self->line;
		case ' ':
		case '\t':
			continue;
		case SCONE_COMMENT:
			if (skip_line(self->file, &ret))
				return ret;
			else {
				++self->line;
				continue;
			}
		case EOF:
			return eof_retval(self->file);
		default:
			++self->line;
			parse_pair(self, ch, key, value, valsize, &ret);
			return ret;
		}
	}
	return 0;
}

int scone_close(struct scone *self)
{
	if (fclose(self->file))
		return -errno;
}

static int skip_line(FILE *file, int *ret)
{
	while (1) {
		switch(fgetc(file)) {
		case '\n':
			*ret = 0;
			return 0;
		case EOF:
			*ret = eof_retval(file);
			return 1;
		default:
			continue;
		}
	}
}

static int eof_retval(FILE *file)
{
	return feof(file) ? SCONE_DONE : -errno;
}


static int compare_key(const char *key, const char *str, size_t len)
{
	size_t i;
	//printf("comparing %.*s to %s\n", len, key, str);
	for (i = 0; i < len; ++i) {
		char kc = key[i], sc = str[i];
		//printf("'%c' <=> '%c'\n", kc, sc);
		if (kc > sc)
			return 1;
		else if (kc < sc)
			return -1;
	}
	//printf("length done, str[%lu] == %d\n", i, str[i]);
	return -(str[i] != '\0');
}
static size_t binary_search(const char *str, size_t len,
	const char *const *items, size_t n_items)
{
	size_t min, mid, max;
	min = 0;
	max = n_items;
	//printf("binary search\n");
	while (min < max) {
		int cmp;
		mid = (min + max) / 2;
		cmp = compare_key(str, items[mid], len);
		if (cmp > 0)
			min = mid + 1;
		else if (cmp < 0)
			max = mid;
		else
			return mid;
	}
	return -1;
}
static int parse_pair(struct scone *self,
	int first_ch,
	size_t *key,
	char *value, size_t *valsize,
	int *ret)
{
	size_t i, key_len = 0;
	int ch;
	self->keybuf[0] = first_ch;
	for (i = 1; i < self->keysize_max; ++i) {
		switch (ch = fgetc(self->file)) {
		case EOF:
			if (!feof(self->file))
				goto err_io;
		case '\n':
			goto err_no_value;
		case SCONE_BINDING:
			goto find_value;
		default:
			key_len = i + 1;
		case ' ':
		case '\t':
			break;
		}
		self->keybuf[i] = ch;
	}
	while (1) {
		switch (fgetc(self->file)) {
		case SCONE_BINDING:
			goto find_value;
		case ' ':
		case '\t':
			continue;
		case EOF:
			if (!feof(self->file))
				goto err_io;
		case '\n':
			goto err_no_value;
		default:
			skip_line(self->file, ret);
			*ret = SCONE_LONG_KEY;
			return 1;
		}
	}
find_value:
	//printf("key_len: %lu\n", key_len);
	while (1) {
		switch (first_ch = fgetc(self->file)) {
		case ' ':
		case '\t':
			continue;
		case EOF:
			if (!feof(self->file))
				goto err_io;
		case '\n':
			goto err_no_value;
		default:
			goto parse_value;
		}
	}
parse_value:
	for (i = 0, ch = first_ch; i < self->valsize_max; ++i) {
		switch (ch) {
		case EOF:
			if (!feof(self->file))
				goto err_io;
		case '\n':
			goto match_key;
		case SCONE_COMMENT:
			skip_line(self->file, ret);
			goto match_key;
		default:
			*valsize = i + 1;
		case ' ':
		case '\t':
			break;
		}
		value[i] = ch;
		ch = fgetc(self->file);
	}
	while (1) {
		switch (ch) {
		case ' ':
		case '\t':
			break;
		case EOF:
			if (!feof(self->file))
				goto err_io;
		case '\n':
			goto match_key;
		case SCONE_COMMENT:
			skip_line(self->file, ret);
			goto match_key;
		default:
			skip_line(self->file, ret);
			*ret = SCONE_LONG_VALUE;
			return 1;
		}
		ch = fgetc(self->file);
	}
match_key:
	i = binary_search(self->keybuf, key_len, self->keys, self->n_keys);
	if (i == -1) {
		*ret = SCONE_BAD_KEY;
	} else {
		*key = i;
	}
	return 1;

err_io:
	*ret = -errno;
	return 1;

err_no_value:
	*ret = SCONE_NO_VALUE;
	return 1;
}
