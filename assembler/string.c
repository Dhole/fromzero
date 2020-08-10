#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define OK 0
#define ERR_STRING_REF -2

typedef struct string_t {
	char *data;
	uint8_t length;
	uint8_t capacity;
} string_t;

bool
string_is_ref(string_t *s)
{
	return s->length > 0 && s->capacity == 0;
}

int
string_resize(string_t *s, uint8_t new_capacity)
{
	if (string_is_ref(s)) {
		return ERR_STRING_REF;
	}
	char *old_data = s->data;
	s->data = malloc((size_t) new_capacity);
	if (s->data == NULL) {
		return -1;
	}
	s->capacity = new_capacity;
	if (s->length > 0) {
		if (new_capacity < s->length) {
			s->length = new_capacity;
		}
		memcpy(s->data, old_data, (size_t) s->length);
		free(old_data);
	}
	return 0;
}

string_t *
string_new()
{
	string_t *s;
	s = calloc(1, sizeof(string_t));
	return s;
}

void
string_free_data(string_t *s)
{
	if (!string_is_ref(s)) {
		if (s->data != NULL) {
			free(s->data);
		}
	}
	s->data = NULL;
	s->capacity = 0;
	s->length = 0;
}

void
string_delete(string_t *s)
{
	string_free_data(s);
	free(s);
}


int
string_set(string_t *s, char *src, uint8_t length)
{
	if (string_is_ref(s)) {
		return ERR_STRING_REF;
	}
	string_free_data(s);
	if (string_resize(s, length) != OK) {
		return -1;
	}
	memcpy(s->data, src, (size_t) length);
	s->length = length;
	return 0;
}

void
string_set_ref(string_t *s, char *src, uint8_t length)
{
	string_free_data(s);
	s->data = src;
	s->length = length;
	s->capacity = 0;
}

#ifdef TEST

#include "assertion-macros.h"

int
main()
{
	struct string_t *s;
	char *src = "Hello world!";

	s = string_new();
	assert_equal(string_set(s, src, strlen(src)), OK);
	string_delete(s);

	s = string_new();
	string_set_ref(s, src, strlen(src));
	string_delete(s);
}
#endif
