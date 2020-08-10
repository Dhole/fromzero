#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define OK 0

typedef struct vector_t {
	void *data;
	uint16_t length;
	uint16_t capacity;
	uint16_t elem_size;
} vector_t;

int
vector_init(vector_t *v, size_t elem_size, uint16_t init_capacity)
{
	v->data = malloc((size_t) init_capacity * elem_size);
	if (v->data == NULL) {
		return -1;
	}
	v->elem_size = (uint16_t) elem_size;
	v->length = 0;
	v->capacity = init_capacity;
	return 0;
}

int
vector_resize(vector_t *v, size_t new_capacity)
{
	void *old_data = v->data;
	v->data = malloc(new_capacity * (size_t) v->elem_size);
	if (v->data == NULL) {
		return -1;
	}
	v->capacity = new_capacity;
	if (new_capacity < v->length) {
		v->length = new_capacity;
	}
	memcpy(v->data, old_data, v->elem_size * v->length);
	if (old_data != NULL) {
		free(old_data);
	}
	return 0;
}

void
vector_delete(vector_t *v)
{
	if (v->data != NULL) {
		free(v->data);
		v->data = NULL;
	}
	v->length = 0;
	v->capacity = 0;
}

void *
vector_get(vector_t *v, size_t index)
{
	assert(index < v->length);
	return v->data + v->elem_size * index;
}

int
vector_push(vector_t *v, void *elem)
{
	size_t new_capacity;
	if (v->capacity == v->length) {
		if (v->capacity == 0) {
			new_capacity = 1;
		} else {
			new_capacity = v->capacity * 2;
		}
		if (vector_resize(v, new_capacity) != OK) {
			return -1;
		}
	}
	memcpy(v->data + v->elem_size * v->length, elem, v->elem_size);
	v->length++;
	return 0;
}


#ifdef TEST

#include "assertion-macros.h"

int
main()
{
	struct vector_t v;
	assert_equal(vector_init(&v, sizeof(int), 0), OK);
	int x;
	int i;
	for (i = 0; i < 10; i++) {
		x = i;
		assert_equal(vector_push(&v, &x), OK);
	}
	assert_equal((int) v.length, 10);
	for (i = 0; i < 10; i++) {
		x = * (int *) vector_get(&v, i);
		assert_equal(x, i);
	}
	vector_delete(&v);
}
#endif
