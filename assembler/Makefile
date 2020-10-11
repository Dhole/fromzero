CC=gcc
CFLAGS=-Wall -g -std=c99
TEST_CFLAGS=-DTEST=1 -g
OBJ=error.o string.o vector.o number.o instructions.o
VALGRIND_OPTS=--leak-check=full -q

test: test_vector test_string test_number
	./test_vector
	./test_string
	./test_number

test_valgrind: test_vector test_string
	valgrind $(VALGRIND_OPTS) ./test_vector
	valgrind $(VALGRIND_OPTS) ./test_string
	valgrind $(VALGRIND_OPTS) ./test_number

test_vector: vector.c
	$(CC) -o $@ $< $(CFLAGS) $(TEST_CFLAGS)

test_string: string.c
	$(CC) -o $@ $< $(CFLAGS) $(TEST_CFLAGS)

test_number: string.o number.c
	$(CC) -o $@ $^ $(CFLAGS) $(TEST_CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ) main.c
	$(CC) -o $@ $^ $(CFLAGS)

main_valgrind: main
	valgrind $(VALGRIND_OPTS) ./main

clean:
	rm *.o test_* main
