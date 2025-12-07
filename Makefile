#Makefile

all: tests

tests: main.c logger.a logger.h
	$(CC) $(CFLAGS) -Wall -Wextra -pedantic -std=c11 $^ -o $@

logger.a: logger.o
	$(AR) rcs $@ $^

logger.o: logger.c
	$(CC) -c $(CFLAGS) -Wall -Wextra -pedantic -std=c11 $^ -o $@

clean:
	$(RM) tests logger.a *.o

.PHONY: all clean
