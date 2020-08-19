#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "string.h"

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
string_set(string_t *s, char *src, uint8_t length)
{
	s->data = malloc((size_t) length);
	if (s->data == NULL) {
		return ERR_NO_MEM;
	}
	memcpy(s->data, src, (size_t) length);
	s->length = length;
	return OK;
}

void
string_set_ref(string_t *s, char *src, uint8_t length)
{
	s->data = src;
	s->length = length;
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

#ifdef TEST

#include "assertion-macros.h"

int
main()
{
	string_t *s;
	char *src = "Hello world!";

	assert_not_null(s = string_new());
	assert_equal(string_set(s, src, strlen(src)), OK);
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
