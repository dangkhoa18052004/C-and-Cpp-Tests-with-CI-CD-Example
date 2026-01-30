CC=gcc
CFLAGS=-Wall -Wextra -O0 -g --coverage
LDFLAGS=--coverage
LDLIBS=-lm -lncurses

all: test-library.out integration-test.out

test-library.out: lib.c lib.h test-library.c
	$(CC) $(CFLAGS) lib.c test-library.c -o $@ $(LDFLAGS) $(LDLIBS)

integration-test.out: lib.c lib.h integration-test.c
	$(CC) $(CFLAGS) lib.c integration-test.c -o $@ $(LDFLAGS) $(LDLIBS)

test: all
	./test-library.out
	./integration-test.out

clean:
	rm -f *.out *.o *.gcda *.gcno *.gcov coverage.info
	rm -rf coverage-html build coverage
