#include "scone.h"

#include <errno.h>

int scone_open(struct scone *self, const char *name)
{
	self->file = fopen(name, "r");
	if (!self->file)
		return -errno;
	self->line = 1;
	self->move_down = 0;
	return 0;
}

/* This matches both spaces and tabs in switch blocks. */
#define ANY_WHITESPACE ' ': case '\t'

static int skip_line(FILE *file, unsigned *line);
static int eof_retval(FILE *file);
static int parse_pair(struct scone *self,
	int first_ch,
	size_t *key,
	char *value, size_t *valsize);
#define ESCAPE_NEWLINE (-0xbeef)
static int escape_char(struct scone *self);
int scone_read(struct scone *self, size_t *key, char *value, size_t *valsize)
{
	int ch;
	while (1) {
		switch(ch = fgetc(self->file)) {
		case '\n':
			++self->line;
			continue;
		case ANY_WHITESPACE:
			continue;
		case SCONE_COMMENT:
			if (skip_line(self->file, &self->line))
				return eof_retval(self->file);
			else
				continue;
		case EOF:
			return eof_retval(self->file);
		case SCONE_BINDING:
			skip_line(self->file, &self->line);
			return SCONE_BAD_KEY;
		default:
			self->line += self->move_down;
			*valsize = 0;
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

static int skip_line(FILE *file, unsigned *line)
{
	while (1) {
		switch(fgetc(file)) {
		case SCONE_ESCAPE:
			(void)fgetc(file);
			++*line;
			continue;
		case '\n':
			++*line;
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


static int compare_key(const char *str, size_t len, const char *key)
{
	size_t i;
	for (i = 0; i < len; ++i) {
		unsigned kc = key[i], sc = str[i];
		if (sc > kc)
			return 1;
		else if (sc < kc)
			return -1;
	}
	return -(key[i] != '\0');
}
static size_t binary_search(const char *str, size_t len,
	const char * const *items, size_t n_items)
{
	size_t min, mid, max;
	min = 0;
	max = n_items;
	while (min < max) {
		int cmp;
		mid = (min + max) / 2;
		cmp = compare_key(str, len, items[mid]);
		if (cmp > 0)
			min = mid + 1;
		else if (cmp < 0)
			max = mid;
		else
			return mid;
	}
	return -1;
}

static int parse_key(struct scone *self, int ch, size_t *key_len)
{
	size_t i;
	*key_len = 0;
	for (i = 0; i < self->keysize_max; ch = fgetc(self->file)) {
		switch (ch) {
		case EOF:
			if (feof(self->file))
				return SCONE_NO_VALUE;
			else
				return -errno;
		case SCONE_COMMENT:
			skip_line(self->file, &self->move_down);
			return SCONE_NO_VALUE;
		case '\n':
			++self->move_down;
			return SCONE_NO_VALUE;
		case SCONE_BINDING:
			return 0;
		case ANY_WHITESPACE:
			break;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (ch == ESCAPE_NEWLINE)
				continue;
			/* Fallthrough */
		default:
			*key_len = i + 1;
			break;
		}
		self->keybuf[i] = ch;
		++i;
	}
	while (1) {
		switch (ch) {
		case SCONE_BINDING:
			return 0;
		case ANY_WHITESPACE:
			break;
		case EOF:
			if (feof(self->file))
				return SCONE_NO_VALUE;
			else
				return -errno;
		case SCONE_COMMENT:
			skip_line(self->file, &self->move_down);
			return SCONE_NO_VALUE;
		case '\n':
			++self->move_down;
			return SCONE_NO_VALUE;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (ch == ESCAPE_NEWLINE)
				break;
			/* Fallthrough */
		default:
			skip_line(self->file, &self->move_down);
			return SCONE_LONG_KEY;
		}
		ch = fgetc(self->file);
	}
}

static int find_value(struct scone *self, int *first_ch)
{
	*first_ch = fgetc(self->file);
	while (1) {
		switch (*first_ch) {
		case ANY_WHITESPACE:
			break;
		case EOF:
			if (feof(self->file))
				return SCONE_NO_VALUE;
			else
				return -errno;
		case SCONE_COMMENT:
			skip_line(self->file, &self->move_down);
			return SCONE_NO_VALUE;
		case '\n':
			++self->move_down;
			return SCONE_NO_VALUE;
		case SCONE_ESCAPE:
			*first_ch = escape_char(self);
			if (*first_ch == ESCAPE_NEWLINE)
				break;
			/* Fallthrough */
		default:
			return 0;
		}
		*first_ch = fgetc(self->file);
	}
}

static int parse_value(struct scone *self,
	int ch,
	char *value, size_t *valsize)
{
	size_t i;
	value[0] = ch;
	*valsize = 1;
	for (i = 1, ch = fgetc(self->file); i < self->valsize_max;
		ch = fgetc(self->file))
	{
		switch (ch) {
		case ANY_WHITESPACE:
			break;
		case EOF:
			if (feof(self->file))
				return 0;
			else
				return -errno;
		case SCONE_COMMENT:
			skip_line(self->file, &self->move_down);
			return 0;
		case '\n':
			++self->move_down;
			return 0;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (ch == ESCAPE_NEWLINE)
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
				return 0;
			else
				return -errno;
		case SCONE_COMMENT:
			skip_line(self->file, &self->move_down);
			return 0;
		case '\n':
			++self->move_down;
			return 0;
		case SCONE_ESCAPE:
			ch = escape_char(self);
			if (ch == ESCAPE_NEWLINE)
				break;
			/* Fallthrough */
		default:
			skip_line(self->file, &self->move_down);
			return SCONE_LONG_VALUE;
		}
		ch = fgetc(self->file);
	}
	return 0;
}

static int find_key(struct scone *self, size_t *key, size_t key_len)
{
	size_t i =
		binary_search(self->keybuf, key_len, self->keys, self->n_keys);
	if (i == (size_t)-1)
		return SCONE_BAD_KEY;
	else {
		*key = i;
		return 0;
	}
}

static int parse_pair(struct scone *self,
	int first_ch,
	size_t *key,
	char *value, size_t *valsize)
{
	size_t key_len;
	int ret = 0;
	self->move_down = 0;
	(void)(   (ret = parse_key(self, first_ch, &key_len))
	       || (ret = find_value(self, &first_ch))
	       || (ret = parse_value(self, first_ch, value, valsize))
	       || (ret = find_key(self, key, key_len)));
	return ret;
}

int parse_nibble(FILE *file)
{
	int ch = fgetc(file);
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	else if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	else if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	else
		return EOF;
}

int parse_byte(FILE *file)
{
	/* I hope that this ignorance of poor \x formatting doesn't cause too
	 * many bugs. */
	int high_nib, low_nib;
	high_nib = parse_nibble(file);
	if (high_nib == EOF) {
		(void)getc(file);
		return '\0';
	}
	low_nib = parse_nibble(file);
	if (low_nib == EOF)
		return '\0';
	return (high_nib << 4) | low_nib;
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
		++self->move_down;
		return ESCAPE_NEWLINE;
	default:
		return ch;
	}
}
