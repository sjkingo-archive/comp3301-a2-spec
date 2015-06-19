CC=gcc
CFLAGS=-Wall
OBJS=main.o test.o

.PHONY: all
all: test

.PHONY: clean
clean:
	rm -f *.o test

test: $(OBJS)
	$(CC) -o $@ $^
