CC=gcc
CFLAGS=-Wall
TEST_CFLAGS=-DTEST=1

test: test_vector test_string
	./test_vector
	./test_string

test_valgrind: test_vector test_string
	valgrind ./test_vector
	valgrind ./test_string

test_vector: vector.c
	$(CC) -o $@ $< $(CFLAGS) $(TEST_CFLAGS)

test_string: string.c
	$(CC) -o $@ $< $(CFLAGS) $(TEST_CFLAGS)
