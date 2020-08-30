#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#include "error.h"

typedef struct {
	char *data;
	uint8_t length;
} string_t;

void string_init(string_t *s);
string_t *string_new();
void string_free(string_t *s);
void string_delete(string_t *s);
error_t string_set(string_t *s, string_t *src);
error_t string_set_s(string_t *s, char *src, uint8_t length);
void string_set_ref(string_t *s, string_t *src);
void string_set_ref_s(string_t *s, char *src, uint8_t length);
void string_set_ref_c(string_t *s, char *src);
bool string_has_prefix(string_t *s, char *prefix);
void string_slice(string_t *src, string_t *dst, uint8_t begin, uint8_t end);

#endif // STRING_H
