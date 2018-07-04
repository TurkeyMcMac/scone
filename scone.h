#include <stddef.h>
#include <stdio.h>

/* The interface for parsing scone configuration files.
 *
 * NOTE: Unless otherwise stated, all pointers mentioned here cannot be NULL. */

#ifndef _SCONE_STRUCTS
#define _SCONE_STRUCTS
/* The structure to hold the parsing state. */
struct scone {
	/* Pointer to the file which configuration will be read from.
	 * This field can be read, but not written. */
	FILE *file;
	/* List of all valid keys. This list must be ALPHABETIZED! Also take care that
	 * no key exceeds a size of keysize_max.
	 * This field can be read or written. */
	const char * const *keys;
	/* Scratch space used internally. This buffer must be at least
	 * keysize_max bytes long.
	 * This field can be read or written, although the contents behind the
	 * pointer are of no use. */
	char *keybuf;
	/* Maximum length of a value in bytes.
	 * This field can be read or written. */
	size_t valsize_max;
	/* Maximum length of a key in bytes.
	 * This field can be read or written. */
	size_t keysize_max;
	/* Length of the list of valid keys.
	 * This field can be read or written. */
	size_t n_keys;
	/* The line in file where the last read() operation took place. The
	 * value is undefined before any read()s have taken place.
	 * This field can be read, but not written. */
	unsigned line;
	/* Scratch space used internally during reading.
	 * This value should neither be read nor written. */
	unsigned move_down;
};
#endif /* !defined(_SCONE_STRUCTS) */

/* The character between a key and a value which binds them together. */
#define SCONE_BINDING ':'
/* The character which begins a comment. The comment will extend from this
 * character to the end of the line. */
#define SCONE_COMMENT '#'
/* The character to begin an escape sequence such as '\n'. */
#define SCONE_ESCAPE '\\'

/* Initialize the file and line fields in a scone. All other fields should be
 * initialized manually.
 * Arguments:
 * 	self: The structure to initialize.
 *  	name: The name of the file to open.
 * Return value: On success, 0 is returned. On failure, the return value is a
 * negated error code from <errno.h>.
 * */
int scone_open(struct scone *self, const char *name);

/* Scone-specific error codes. */
/* This one isn't really an error. It just indicates that no key-value pairs
 * are left. */
#define SCONE_DONE	1
/* Indicates that the key which was just parsed was not on the list of valid
 * keys. */
#define SCONE_BAD_KEY	2
/* Indicates that the key which was just parsed was not paired with a value. */
#define SCONE_NO_VALUE	3
/* Indicates that the key which was just parsed was longer than keysize_max. */
#define SCONE_LONG_KEY	4	
/* Indicates that the value which was just parsed was longer than keysize_max.
 */
#define SCONE_LONG_VALUE 5

/* Read one key-value pair from the file associated with the scone.
 * Arguments:
 * 	self: The scone in question.
 *	key: The location where the key code will be put. The key code is an
 *		index into the array keys (the index of the matching key.)
 *	value: The buffer to fill with the value. This must be at least
 *		valsize_max bytes long. The resulting string is NOT NULL-
 *		TERMINATED! Adding a null terminator is up to the caller.
 *	valsize: The location where the size in bytes of the read value will be
 *		put. This will be less than or equal to valsize_max.
 * Return value: On success, 0 is returned. In the event of an I/O error, a
 * negated error code from <errno.h> is returned. In the case of a parsing
 * error, the return value is a positive error code for one of the errors shown
 * above. When either kind of error occurs, the value behind every argument
 * excluding the first is uncertain.
 * */
int scone_read(struct scone *self, size_t *key, char *value, size_t *valsize);

/* Close the file behind a scone. The scone can later be open()ed again without
 * manually resetting any fields.
 * Arguments:
 * 	self: The scone in question.
 * Return value: On success, 0 is returned. On failure, the return value is a
 * negated error code from <errno.h>.
 * */
int scone_close(struct scone *self);
