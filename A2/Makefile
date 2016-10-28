CC=gcc
CFLAGS=-lpthread
measure:
	$(CC) -o measure measure.c $(CFLAGS)
all: measure
clean:
	rm measure 
run: measure
	./measure