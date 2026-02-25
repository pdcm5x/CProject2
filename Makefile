CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

TARGETS = oss worker
OBJECTS = oss.o worker.o

all: $(TARGETS)

oss: oss.o
	$(CC) $(CFLAGS) -o oss oss.o

worker: worker.o
	$(CC) $(CFLAGS) -o worker worker.o

%.o: %.c clock.h shared.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean