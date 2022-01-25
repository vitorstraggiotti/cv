# Makefile to build test program for cv library

CC = gcc

RELEASEFLAGS = -Wall -pedantic -c -O2
DEBUGFLAGS = -Wall -pedantic -c -g
RUNLIB = -lpthread

.PHONY: all clean cleanimg cleanall

all:
	@echo "You can choose one of the options:"
	@echo "  make test      --> Optimised version"
	@echo "  make testDEBUG --> Debug version"
	@echo "  make clean     --> Clear all generated files"


# Building optimized version
test: test.o bitmap.o cv.o
	$(CC) -o $@ $^ $(RUNLIB)

test.o: test.c
	$(CC) $(RELEASEFLAGS) -o $@ $^

bitmap.o: bitmap.c
	$(CC) $(RELEASEFLAGS) -o $@ $^

cv.o: cv.c
	$(CC) $(RELEASEFLAGS) -o $@ $^

# Building debug version
testDEBUG: test_d.o bitmap_d.o cv_d.o
	$(CC) -o $@ $^ $(RUNLIB)

test_d.o: test.c
	$(CC) $(DEBUGFLAGS) -o $@ $^

bitmap_d.o: bitmap.c
	$(CC) $(DEBUGFLAGS) -o $@ $^

cv_d.o: cv.c
	$(CC) $(DEBUGFLAGS) -o $@ $^

# Cleaning generated files
clean:
	rm test testDEBUG *.o saida*

