#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdio.h>

#include "error.h"
#include "vector.h"

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
void string_lower(string_t *src);
void string_slice(string_t *src, string_t *dst, uint8_t begin, uint8_t end);
// lexicographical comparison following ASCII position of each character
cmp_t string_cmp(string_t *a, string_t *b);
cmp_t string_cmp_c(string_t *a, char *_b);
error_t string_write(string_t *s, FILE *stream);

#endif // STRING_H
