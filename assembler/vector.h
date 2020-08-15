#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

#include "error.h"

typedef enum cmp_t {
	LESS = -1, // A < B
	EQUAL = 0, // A == B
	GREATER = 1, // A > B
} cmp_t;

typedef cmp_t (elem_cmp_fn_t)(void *, void *);

typedef void (elem_free_fn_t)(void *);

typedef struct vector_t {
	void *data;
	uint16_t length;
	uint16_t capacity;
	uint16_t elem_size;
	elem_free_fn_t *elem_free_fn;
} vector_t;

error_t vector_init(vector_t *v, size_t elem_size, uint16_t init_capacity, elem_free_fn_t *elem_free_fn);
vector_t * vector_new(size_t elem_size, uint16_t init_capacity, elem_free_fn_t *elem_free_fn);
error_t vector_resize(vector_t *v, size_t new_capacity);
void * vector_get(vector_t *v, size_t index);
void vector_clear(vector_t *v);
void vector_free(vector_t *v);
void vector_delete(vector_t *v);
error_t vector_push(vector_t *v, void *elem);
void vector_swap(vector_t *v, size_t index_a, size_t index_b);
void vector_sort(vector_t *v, elem_cmp_fn_t elem_cmp_fn);
void * vector_bin_search(vector_t *v, elem_cmp_fn_t elem_cmp_fn, void *key);

#endif // VECTOR_H
