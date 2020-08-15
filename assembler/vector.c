#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "errors.h"

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

vector_t *
vector_new(size_t elem_size, uint16_t init_capacity, elem_free_fn_t *elem_free_fn)
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
	v->elem_free_fn = elem_free_fn;
	return v;
}

error_t
vector_resize(vector_t *v, size_t new_capacity)
{
	if (new_capacity < v->length) {
		return ERR_NO_MEM;
	}
	void *old_data = v->data;
	v->data = malloc(new_capacity * (size_t) v->elem_size);
	if (v->data == NULL) {
		return ERR_NO_MEM;
	}
	v->capacity = new_capacity;
	memcpy(v->data, old_data, v->elem_size * v->length);
	if (old_data != NULL) {
		free(old_data);
	}
	return OK;
}

void *
vector_get(vector_t *v, size_t index)
{
	assert(index < v->length);
	return v->data + v->elem_size * index;
}

void
vector_free(vector_t *v)
{
	int i;
	if (v->elem_free_fn != NULL) {
		for (i = 0; i < v->length; i++) {
			v->elem_free_fn(vector_get(v, i));
		}
	}
	if (v->capacity > 0) {
		free(v->data);
	}
}

void
vector_delete(vector_t *v)
{
	if (v == NULL) {
		return;
	}
	vector_free(v);
	free(v);
}

error_t
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
			return ERR_NO_MEM;
		}
	}
	memcpy(v->data + v->elem_size * v->length, elem, v->elem_size);
	v->length++;
	return OK;
}

void
vector_swap(vector_t *v, size_t index_a, size_t index_b)
{
	if (index_a == index_b) {
		return;
	}
	uint8_t tmp[v->elem_size];
	void *a = vector_get(v, index_a);
	void *b = vector_get(v, index_b);
	memcpy(tmp, b, v->elem_size);
	memcpy(b, a, v->elem_size);
	memcpy(a, tmp, v->elem_size);
}

size_t
_vector_partition(vector_t *v, elem_cmp_fn_t cmp_fn, size_t lo, size_t hi)
{
	void *pivot = vector_get(v, (hi + lo) / 2);
	// printf("p=%d, pivot=%d\n", (hi + lo) / 2, * (int *) pivot);
	void *elem;
	int i = lo - 1;
	int j = hi + 1;
	while (true) {
		do {
			i++;
			elem = vector_get(v, (size_t) i);
			// printf("i=%d, v=%d\n", i, * (int *) elem);
		} while (cmp_fn(elem, pivot) == LESS);
		do {
			j--;
			elem = vector_get(v, (size_t) j);
			// printf("j=%d, v=%d\n", j, * (int *) elem);
		} while (cmp_fn(elem, pivot) == GREATER);
		if (i >= j) {
			// printf("_vector_partition lo=%d, hi=%d, p=%d\n", lo, hi, j);
			return j;
		}
		vector_swap(v, (size_t) i, (size_t) j);
	}
}

void
_vector_quicksort(vector_t *v, elem_cmp_fn_t cmp_fn, size_t lo, size_t hi)
{
	// printf("_vector_quicksort lo=%d, hi=%d\n", lo, hi);
	size_t p;
	if (lo < hi) {
		p = _vector_partition(v, cmp_fn, lo, hi);
		// printf("p=%d\n", p);
		_vector_quicksort(v, cmp_fn, lo, p);
		_vector_quicksort(v, cmp_fn, p + 1, hi);
	}
}

void
vector_sort(vector_t *v, elem_cmp_fn_t elem_cmp_fn)
{
	_vector_quicksort(v, elem_cmp_fn, 0, v->length - 1);
}

void *
vector_bin_search(vector_t *v, elem_cmp_fn_t elem_cmp_fn, void *key)
{
	int lo = 0, hi = v->length - 1;
	int p;
	void *elem;
	cmp_t cmp;
	while (true) {
		p = (lo + hi) / 2;
		elem = vector_get(v, (size_t) p);
		cmp = elem_cmp_fn(key, elem);
		// printf("vector_bin_search: p=%d, lo=%d, hi=%d, cmp=%d\n", p, lo, hi, cmp);
		if (cmp == EQUAL) {
			return elem;
		} else if (cmp == LESS) {
			hi = p - 1;
		} else if (cmp == GREATER) {
			lo = p + 1;
		}
		// printf("lo=%d, hi=%d\n", lo, hi);
		if (lo > hi) {
			return NULL;
		}
	}
}

#ifdef TEST

#include "assertion-macros.h"

typedef struct foo_t {
	void *data;
} foo_t;

error_t
foo_init(foo_t *f)
{
	f->data = malloc(10);
	if (f->data == NULL) {
		return ERR_NO_MEM;
	}
	// printf("> Init foo: %p, f->data: %p\n", f, f->data);
	return OK;
}

void
foo_free(foo_t *f)
{
	// printf("> Free foo: %p, f->data: %p\n", f, f->data);
	free(f->data);
}

cmp_t
cmp_int(int *a, int *b)
{
	if (*a < *b) {
		return LESS;
	} else if (*a > *b) {
		return GREATER;
	} else {
		return EQUAL;
	}
}

int
main()
{
	struct vector_t *v;
	int x;
	int i;

	//
	// Test copy elements (NULL elem_free_fn)
	//
	assert_not_null(v = vector_new(sizeof(int), 0, NULL));
	// Test push
	for (i = 0; i < 10; i++) {
		x = i;
		assert_equal(vector_push(v, &x), OK);
	}
	assert_equal((int) v->length, 10);
	// Test get
	for (i = 0; i < 10; i++) {
		x = * (int *) vector_get(v, i);
		assert_equal(x, i);
	}
	// Test swap
	vector_swap(v, 2, 3);
	x = * (int *) vector_get(v, 2);
	assert_equal(x, 3);
	x = * (int *) vector_get(v, 3);
	assert_equal(x, 2);
	// Test delete
	vector_delete(v);

	//
	// Test sort
	//
	assert_not_null(v = vector_new(sizeof(int), 0, NULL));
	x = 8; assert_equal(vector_push(v, &x), OK);
	x = 1; assert_equal(vector_push(v, &x), OK);
	x = 4; assert_equal(vector_push(v, &x), OK);
	x = 4; assert_equal(vector_push(v, &x), OK);
	x = 7; assert_equal(vector_push(v, &x), OK);
	x = 2; assert_equal(vector_push(v, &x), OK);
	x = 3; assert_equal(vector_push(v, &x), OK);

	cmp_t (*cmp_sort)(void *, void *);
	cmp_sort = (cmp_t (*)(void *, void *)) cmp_int;

	vector_sort(v, cmp_sort);
	x = * (int *) vector_get(v, 0); assert_equal(x, 1);
	x = * (int *) vector_get(v, 1); assert_equal(x, 2);
	x = * (int *) vector_get(v, 2); assert_equal(x, 3);
	x = * (int *) vector_get(v, 3); assert_equal(x, 4);
	x = * (int *) vector_get(v, 4); assert_equal(x, 4);
	x = * (int *) vector_get(v, 5); assert_equal(x, 7);
	x = * (int *) vector_get(v, 6); assert_equal(x, 8);

	cmp_t (*cmp_search)(void *, void *);
	cmp_search = (cmp_t (*)(void *, void *)) cmp_int;

	// Test bin_search
	int key;
	int *y;
	key = 1; y = (int *) vector_bin_search(v, cmp_search, &key); assert_not_null(y); assert_equal(*y, key);
	key = 4; y = (int *) vector_bin_search(v, cmp_search, &key); assert_not_null(y); assert_equal(*y, key);
	key = 8; y = (int *) vector_bin_search(v, cmp_search, &key); assert_not_null(y); assert_equal(*y, key);
	key = 0; y = (int *) vector_bin_search(v, cmp_search, &key); assert_null(y);;
	key = 6; y = (int *) vector_bin_search(v, cmp_search, &key); assert_null(y);;
	key = 9; y = (int *) vector_bin_search(v, cmp_search, &key); assert_null(y);;

	vector_delete(v);

	//
	// Test reference elements (NOT NULL elem_free_fn)
	//
	foo_t f;
	assert_not_null(v = vector_new(sizeof(foo_t), 0, (void (*)(void *)) foo_free));
	// Test push
	for (i = 0; i < 10; i++) {
		assert_equal(foo_init(&f), OK);
		assert_equal(vector_push(v, &f), OK);
	}
	// Test delete
	vector_delete(v);
}
#endif
