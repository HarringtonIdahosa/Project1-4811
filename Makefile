CC=/usr/bin/gcc
CFLAGS=-I. -pthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: main

main: main.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)