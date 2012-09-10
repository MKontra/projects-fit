CC=gcc
CFLAGS=-c -std=c99 -pedantic -Wall
OBJECTS_DNY=error.o enumdny.o enumtest.o
OBJECTS_PRIMES=error.o prvocisla.o
OBJECTS_PRIMES_INLINE=error.o prvocisla-inline.o 

all: enumtest prvocisla prvocisla-inline

prvocisla: $(OBJECTS_PRIMES)
	$(CC) -lm -o $@ $(OBJECTS_PRIMES)

prvocisla-inline: $(OBJECTS_PRIMES_INLINE)
	$(CC) -lm -o $@ $(OBJECTS_PRIMES_INLINE)
 
enumtest: $(OBJECTS_DNY)
	$(CC) -o $@ $(OBJECTS_DNY)

prvocisla.o: prvocisla.c
	$(CC) $(CFLAGS) prvocisla.c

prvocisla-inline.o: prvocisla.c
	$(CC) $(CFLAGS) -DUSE_INLINE -o $@ prvocisla.c

enumtest.o: enumtest.c
	$(CC) $(CFLAGS) enumtest.c

error.o: error.c error.h
	$(CC) $(CFLAGS) error.c -o error.o

enumdny.o : enumdny.c enumdny.h
	$(CC) $(CFLAGS) enumdny.c -o enumdny.o  
clean:
	-rm *.o 