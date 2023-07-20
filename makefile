CC=g++
CFLAGS=-std=c++14 -Wall -g
OBJS=main.o helper.o
BINS=main

all: $(BINS)

main: $(OBJS)
	$(CC) $(CFLAGS) -o main $(OBJS)

main.o: main.cpp helper.h
	$(CC) $(CFLAGS) -c main.cpp

helper.o: helper.cpp helper.h
	$(CC) $(CFLAGS) -c helper.cpp

clean:
	rm -f $(BINS) *.o
