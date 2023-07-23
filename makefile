CC=g++
CFLAGS=-O3
OBJS=bwtsearch.o
BINS=bwtsearch

all: $(BINS)

bwtsearch: $(OBJS)
	$(CC) $(CFLAGS) -o bwtsearch $(OBJS)

bwtsearch.o: bwtsearch.cpp
	$(CC) $(CFLAGS) -c bwtsearch.cpp

clean:
	rm -f $(BINS) *.o
