#CC=gcc 
MPICC=mpiCC
CFLAGS= -g -Wall -Wno-unknown-pragmas -DNUM_OF_PARTICLES=5000
LIBS= -lm

PROGRAMS = proj03 gen-data

all: $(PROGRAMS)

run: $(PROGRAMS)
	./gen-data particles.dat
	./mpi 4 proj03 particles.dat 1 15

proj03: proj03.c globaldata.h
	$(MPICC) $(CFLAGS) $(LIBS) -o $@ $<

gen-data: gen-data.c globaldata.h
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f *.o
	rm -f *~
	rm -f $(PROGRAMS)


