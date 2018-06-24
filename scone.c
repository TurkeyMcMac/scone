#include "scone.h"

#include <errno.h>

int scone_open(struct scone *self, const char *name)
{
	self->file = fopen(name, "r");
	if (!self->file)
		return -errno;
	self->line = 0;
	return 0;
}

static int skip_line(FILE *self);
static int eof_retval(FILE *file);
static int parse_pair(struct scone *self,
	int first_ch,
	size_t *key,
	char *value, size_t *valsize);
int scone_read(struct scone *self, size_t *key, char *value, size_t *valsize)
{
	int ch;
	while (1) {
		switch(ch = fgetc(self->file)) {
		case '\n':
			++self->line;
			continue;
		case ' ':
		case '\t':
			continue;
		case SCONE_COMMENT:
			if (skip_line(self->file))
				return eof_retval(self->file);
			else {
				++self->line;
				continue;
			}
		case EOF:
			return eof_retval(self->file);
		case SCONE_BINDING:
			++self->line;
			skip_line(self->file);
			return SCONE_BAD_KEY;
		default:
			++self->line;
			return parse_pair(self, ch, key, value, valsize);
		}
	}
	return 0;
}

int scone_close(struct scone *self)
{
	if (fclose(self->file))
		return -errno;
	return 0;
}

static int skip_line(FILE *file)
{
	while (1) {
		switch(fgetc(file)) {
		case '\n':
			return 0;
		case EOF:
			return -1;
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
	for (i = 0; i < len; ++i) {
		char kc = key[i], sc = str[i];
		if (kc > sc)
			return 1;
		else if (kc < sc)
			return -1;
	}
	return -(str[i] != '\0');
}
static size_t binary_search(const char *str, size_t len,
	const char *const *items, size_t n_items)
{
	size_t min, mid, max;
	min = 0;
	max = n_items;
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
	char *value, size_t *valsize)
{
	size_t i, key_len = 0;
	int ch;
	self->keybuf[0] = first_ch;
	for (i = 1; i < self->keysize_max; ++i) {
		switch (ch = fgetc(self->file)) {
		case EOF:
			if (!feof(self->file))
				goto err_io;
			else
				goto err_no_value;
		case SCONE_COMMENT:
			skip_line(self->file);
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
			else
				goto err_no_value;
		case SCONE_COMMENT:
			skip_line(self->file);
		case '\n':
			goto err_no_value;
		default:
			skip_line(self->file);
			return SCONE_LONG_KEY;
		}
	}
find_value:
	while (1) {
		switch (first_ch = fgetc(self->file)) {
		case ' ':
		case '\t':
			continue;
		case EOF:
			if (!feof(self->file))
				goto err_io;
			else
				goto err_no_value;
		case SCONE_COMMENT:
			skip_line(self->file);
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
			else
				goto match_key;
		case SCONE_COMMENT:
			skip_line(self->file);
		case '\n':
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
			if (feof(self->file))
				goto match_key;
			else
				goto err_io;
		case SCONE_COMMENT:
			skip_line(self->file);
		case '\n':
			goto match_key;
		default:
			skip_line(self->file);
			return SCONE_LONG_VALUE;
		}
		ch = fgetc(self->file);
	}
match_key:
	i = binary_search(self->keybuf, key_len, self->keys, self->n_keys);
	if (i == (size_t)-1)
		goto err_bad_key;
	else
		*key = i;
	return 0;

err_bad_key:
	return SCONE_BAD_KEY;

err_io:
	return -errno;

err_no_value:
	return SCONE_NO_VALUE;
}
