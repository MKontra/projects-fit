#CC=g++
CC=mpiCC
CCFLAGS= -O3 -c 
LDFLAGS= -lm -O3 

OBJS= test_mpi.o

test_mpi: $(OBJS)
	$(CC) -o  $@ $(OBJS) $(LDFLAGS)

test_mpi.o : test_mpi.cc
	$(CC) $(CCFLAGS) test_mpi.cc
	