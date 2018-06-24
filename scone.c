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

/* This matches both spaces and tabs in switch blocks. */
#define ANY_WHITESPACE ' ': case '\t'

static int skip_line(struct scone *self);
static int eof_retval(FILE *file);
static int parse_pair(struct scone *self,
	int first_ch,
	size_t *key,
	char *value, size_t *valsize);
static int escape_char(struct scone *self);
int scone_read(struct scone *self, size_t *key, char *value, size_t *valsize)
{
	int ch;
	*valsize = 0;
	while (1) {
		switch(ch = fgetc(self->file)) {
		case '\n':
			++self->line;
			continue;
		case ANY_WHITESPACE:
			continue;
		case SCONE_COMMENT:
			if (skip_line(self))
				return eof_retval(self->file);
			else {
				++self->line;
				continue;
			}
		case EOF:
			return eof_retval(self->file);
		case SCONE_BINDING:
			++self->line;
			skip_line(self);
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

static int skip_line(struct scone *self)
{
	while (1) {
		switch(fgetc(self->file)) {
		case SCONE_ESCAPE:
			(void)fgetc(self->file);
			++self->line;
			continue;
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
	int ch = first_ch;
	for (i = 0; i < self->keysize_max; ch = fgetc(self->file)) {
		switch (ch) {
		case EOF:
			if (!feof(self->file))
				goto err_io;
			else
				goto err_no_value;
		case SCONE_COMMENT:
			skip_line(self);
			/* Fallthrough */
		case '\n':
			goto err_no_value;
		case SCONE_BINDING:
			goto find_value;
		case ANY_WHITESPACE:
			break;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (!ch)
				continue;
			/* Fallthrough */
		default:
			key_len = i + 1;
			break;
		}
		self->keybuf[i] = ch;
		++i;
	}
	while (1) {
		switch (ch) {
		case SCONE_BINDING:
			goto find_value;
		case ANY_WHITESPACE:
			break;
		case EOF:
			if (!feof(self->file))
				goto err_io;
			else
				goto err_no_value;
		case SCONE_COMMENT:
			skip_line(self);
			/* Fallthrough */
		case '\n':
			goto err_no_value;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (!ch)
				break;
			/* Fallthrough */
		default:
			skip_line(self);
			goto err_long_key;
		}
		ch = fgetc(self->file);
	}
find_value:
	first_ch = fgetc(self->file);
	while (1) {
		switch (first_ch) {
		case ANY_WHITESPACE:
			break;
		case EOF:
			if (!feof(self->file))
				goto err_io;
			else
				goto err_no_value;
		case SCONE_COMMENT:
			skip_line(self);
			/* Fallthrough */
		case '\n':
			goto err_no_value;
		case SCONE_ESCAPE:
			first_ch = escape_char(self);
			if (!first_ch)
				break;
			/* Fallthrough */
		default:
			goto parse_value;
		}
		first_ch = fgetc(self->file);
	}
parse_value:
	value[0] = first_ch;
	*valsize = 1;
	for (i = 1, ch = fgetc(self->file); i < self->valsize_max;
		ch = fgetc(self->file))
	{
		switch (ch) {
		case ANY_WHITESPACE:
			break;
		case EOF:
			if (!feof(self->file))
				goto err_io;
			else
				goto match_key;
		case SCONE_COMMENT:
			skip_line(self);
			/* Fallthrough */
		case '\n':
			goto match_key;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (!ch)
				continue;
			/* Fallthrough */
		default:
			*valsize = i + 1;
			break;
		}
		value[i] = ch;
		++i;
	}
	while (1) {
		switch (ch) {
		case ANY_WHITESPACE:
			break;
		case EOF:
			if (feof(self->file))
				goto match_key;
			else
				goto err_io;
		case SCONE_COMMENT:
			skip_line(self);
			/* Fallthrough */
		case '\n':
			goto match_key;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (!ch)
				break;
			/* Fallthrough */
		default:
			skip_line(self);
			goto err_long_value;
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

err_long_key:
	return SCONE_LONG_KEY;

err_no_value:
	return SCONE_NO_VALUE;

err_long_value:
	return SCONE_LONG_VALUE;
}

static int escape_char(struct scone *self)
{
	int ch;
	switch (ch = fgetc(self->file)) {
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 'e':
		return '\x1B';
	case 'f':
		return '\f';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 't':
		return '\t';
	case 'v':
		return '\v';
	case '\n':
		++self->line;
		return '\0';
	default:
		return ch;
	}
}
