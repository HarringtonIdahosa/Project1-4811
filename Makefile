CC = gcc
CFLAGS = -Wall -Werror

all: output test1 test2 test3 test4 test5

output: project1.c
	$(CC) $(CFLAGS) -o $@ $^

test1: test1.c
	$(CC) $(CFLAGS) -o $@ $^

test2: test2.c
	$(CC) $(CFLAGS) -o $@ $^

test3: test3.c
	$(CC) $(CFLAGS) -o $@ $^

test4: test4.c
	$(CC) $(CFLAGS) -o $@ $^

test5: test5.c
	$(CC) $(CFLAGS) -o $@ $^

run_tests: all
	./output
	./test1
	./test2
	./test3
	./test4
	./test5

clean:
	rm -f output test1 test2 test3 test4 test5
