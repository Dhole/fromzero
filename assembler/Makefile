CC=gcc
CFLAGS=-Wall
TEST_CFLAGS=-DTEST=1 -g

test: test_vector test_string
	./test_vector
	./test_string

test_valgrind: test_vector test_string
	valgrind -q ./test_vector
	valgrind -q ./test_string

test_vector: vector.c
	$(CC) -o $@ $< $(CFLAGS) $(TEST_CFLAGS)

test_string: string.c
	$(CC) -o $@ $< $(CFLAGS) $(TEST_CFLAGS)
