#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "string.h"
#include "vector.h"

#include "error.h"

void
string_init(string_t *s)
{
	s->data = NULL;
	s->length = 0;
}

string_t *
string_new()
{
	string_t *s;
	s = malloc(sizeof(string_t));
	if (s == NULL) {
		return NULL;
	}
	string_init(s);
	return s;
}

void
string_free(string_t *s)
{
	if (s->data != NULL) {
		free(s->data);
		s->data = NULL;
	}
	s->length = 0;
}

void
string_delete(string_t *s)
{
	if (s == NULL) {
		return;
	}
	string_free(s);
	free(s);
}

error_t
string_set_s(string_t *s, char *src, uint8_t length)
{
	s->data = malloc((size_t) length);
	if (s->data == NULL) {
		return ERR_NO_MEM;
	}
	memcpy(s->data, src, (size_t) length);
	s->length = length;
	return OK;
}

error_t
string_set(string_t *s, string_t *src)
{
	return string_set_s(s, src->data, src->length);
}

void
string_set_ref_s(string_t *s, char *src, uint8_t length)
{
	s->data = src;
	s->length = length;
}

void
string_set_ref(string_t *s, string_t *src)
{
	return string_set_ref_s(s, src->data, src->length);
}

void
string_set_ref_c(string_t *s, char *src)
{
	s->data = src;
	s->length = strlen(src);
}

void
string_slice(string_t *src, string_t *dst, uint8_t begin, uint8_t end)
{
	if (end == 0) {
		end = src->length;
	}
	assert(begin < end);
	assert(end <= src->length);
	dst->data = &(src->data[begin]);
	dst->length = end - begin;
}

void
string_lower(string_t *s)
{
	int i;
	char *c;
	for (i = 0; i < s->length; i++) {
		c = &(s->data[i]);
		if (*c >= 'A' && *c <= 'Z') {
			*c += ('a' - 'A');
		}
	}
}

bool
string_has_prefix(string_t *s, char *prefix)
{
	int i;
	for (i = 0; i < s->length; i++) {
		if (prefix[i] == '\0') {
			return true;
		}
		if (s->data[i] != prefix[i]) {
			return false;
		}
	}
	if (prefix[i] == '\0') {
		return true;
	}
	return false;
}

// lexicographical comparison following ASCII position of each character
cmp_t
string_cmp(string_t *a, string_t *b)
{
	int i;
	uint8_t char_a, char_b;
	for (i = 0; i < a->length && i < b->length; i++) {
		char_a = (uint8_t) a->data[i];
		char_b = (uint8_t) b->data[i];
		if (char_a < char_b) {
			return LESS;
		} else if (char_a > char_b) {
			return GREATER;
		}
	}
	if (a->length < b->length) {
		return LESS;
	} else if (a->length == b->length) {
		return EQUAL;
	} else {
		return GREATER;
	}
}

cmp_t
string_cmp_c(string_t *a, char *_b)
{
	string_t b;
	b.data = _b;
	b.length = strlen(_b);
	return string_cmp(a, &b);
}

error_t
string_write(string_t *s, FILE *stream)
{
	size_t res;
	int rem = s->length;

	while (rem > 0) {
		res = fwrite(s->data + s->length - rem, 1, rem, stream);
		if (res == -1) {
			return ERR_WRITE;
		}
		rem -= res;
	}
	return OK;
}

#ifdef TEST

#include "assertion-macros.h"

int
main()
{
	string_t *s;
	char *src = "Hello world!";

	assert_not_null(s = string_new());
	assert_equal(string_set_s(s, src, strlen(src)), OK);
	string_delete(s);

	string_t s1;
	string_set_ref_c(&s1, "0x12");
	assert_equal(string_has_prefix(&s1, "0x"), true);
	assert_equal(string_has_prefix(&s1, ""), true);
	assert_equal(string_has_prefix(&s1, "0x12"), true);
	assert_equal(string_has_prefix(&s1, "0x123"), false);
	assert_equal(string_has_prefix(&s1, "1"), false);
}
#endif
