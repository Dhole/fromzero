#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define OK 0

typedef void (vector_deleter_t)(void *);

typedef struct vector_t {
	void *data;
	uint16_t length;
	uint16_t capacity;
	uint16_t elem_size;
	vector_deleter_t *deleter;
} vector_t;

vector_t *
vector_new(size_t elem_size, uint16_t init_capacity, vector_deleter_t *deleter)
{
	vector_t *v;
	v = calloc(1, sizeof(vector_t));
	v->data = malloc((size_t) init_capacity * elem_size);
	if (v->data == NULL && init_capacity > 0) {
		return NULL;
	}
	v->elem_size = (uint16_t) elem_size;
	v->length = 0;
	v->capacity = init_capacity;
	v->deleter = deleter;
	return v;
}

int
vector_resize(vector_t *v, size_t new_capacity)
{
	if (new_capacity < v->length) {
		return -1;
	}
	void *old_data = v->data;
	v->data = malloc(new_capacity * (size_t) v->elem_size);
	if (v->data == NULL) {
		return -1;
	}
	v->capacity = new_capacity;
	memcpy(v->data, old_data, v->elem_size * v->length);
	if (old_data != NULL) {
		free(old_data);
	}
	return 0;
}

void *
vector_get(vector_t *v, size_t index)
{
	assert(index < v->length);
	return v->data + v->elem_size * index;
}

void
vector_delete(vector_t *v)
{
	int i;
	if (v == NULL) {
		return;
	}
	if (v->deleter != NULL) {
		for (i = 0; i < v->length; i++) {
			v->deleter(vector_get(v, i));
		}
	}
	if (v->capacity > 0) {
		free(v->data);
	}
	free(v);
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

typedef struct foo_t {
	void *data;
} foo_t;

int
foo_init(foo_t *f)
{
	f->data = malloc(10);
	if (f->data == NULL) {
		return -1;
	}
	// printf("> Init foo: %p, f->data: %p\n", f, f->data);
	return 0;
}

void
foo_free(foo_t *f)
{
	// printf("> Free foo: %p, f->data: %p\n", f, f->data);
	free(f->data);
}

int
main()
{
	struct vector_t *v;
	assert_not_null(v = vector_new(sizeof(int), 0, NULL));
	int x;
	int i;
	for (i = 0; i < 10; i++) {
		x = i;
		assert_equal(vector_push(v, &x), OK);
	}
	assert_equal((int) v->length, 10);
	for (i = 0; i < 10; i++) {
		x = * (int *) vector_get(v, i);
		assert_equal(x, i);
	}
	vector_delete(v);

	foo_t f;
	assert_not_null(v = vector_new(sizeof(foo_t), 0, (void (*)(void *)) foo_free));
	for (i = 0; i < 10; i++) {
		assert_equal(foo_init(&f), OK);
		assert_equal(vector_push(v, &f), OK);
	}
	vector_delete(v);
}
#endif
