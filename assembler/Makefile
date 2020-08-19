CC=gcc
CFLAGS=-Wall -g -std=c99
TEST_CFLAGS=-DTEST=1 -g
OBJ=error.o string.o vector.o number.o

test: test_vector test_string test_number
	./test_vector
	./test_string
	./test_number

test_valgrind: test_vector test_string
	valgrind -q ./test_vector
	valgrind -q ./test_string
	valgrind -q ./test_number

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
	valgrind -q ./main

clean:
	rm *.o test_* main
