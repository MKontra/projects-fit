CC=gcc
OMCC=$(CC) -fopenmp

# pro finalni vypocet nastavte N >= 1000
CFLAGS= -g -Wall -Wno-unknown-pragmas -DN=1000 -DNUM_OF_PARTICLES=1000
LIBS= -lm

PROGRAMS = seben proj02 gen-data

all: $(PROGRAMS)

# zde muzete experimentovat zejmena s poctem vlaken (>4)
run: $(PROGRAMS)
	./gen-data particles.dat
	./proj02 particles.dat 0.001 1000 4
# argumenty proj02: filename, time_step_size, num_of_time_steps, num_of_threads

seben: seben.c globaldata.h
	$(OMCC) $(CFLAGS) $(LIBS) -o $@ $<

proj02: proj02.c globaldata.h
	$(OMCC) $(CFLAGS) $(LIBS) -o $@ $<

gen-data: gen-data.c globaldata.h
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f *.o
	rm -f *~
	rm -f $(PROGRAMS)


