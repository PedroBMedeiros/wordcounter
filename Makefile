CC=gcc
CFLAGS= -Wall -pedantic -std=c89 -g

all: fw

fw: fw.o
	$(CC) $(CFLAGS) $^ -o $@

fw.o: fw.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	rm -f *.o
