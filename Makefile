CC = gcc
CFLAGS = -pthread

SOURCES = main.c prod_cons_MT.c
EXECUTABLE = prog2

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(EXECUTABLE)
