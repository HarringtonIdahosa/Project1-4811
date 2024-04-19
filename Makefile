CC=/usr/bin/gcc
CFLAGS=-I. -pthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: proj4

proj4: main.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)