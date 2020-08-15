#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

#ifdef TEST

#include "assertion-macros.h"

int
main()
{
	struct string_t *s;
	char *src = "Hello world!";

	assert_not_null(s = string_new());
	assert_equal(string_set(s, src, strlen(src)), OK);
	string_delete(s);
}
#endif
