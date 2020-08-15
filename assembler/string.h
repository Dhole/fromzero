#ifndef STRING_H
#define STRING_H

#include "error.h"
#include "stdint.h"

typedef struct string_t {
	char *data;
	uint8_t length;
} string_t;

void string_init(string_t *s);
string_t *string_new();
void string_free(string_t *s);
void string_delete(string_t *s);
error_t string_set(string_t *s, char *src, uint8_t length);

#endif // STRING_H
