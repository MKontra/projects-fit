CC=g++
CFLAGS=-pedantic -Wall -g -c

LD=g++ 
LDFLAGS=-lpthread -pthread -lpcap

SRCS=scan.cpp
HEADS=

OBJS=$(SRCS:.cpp=.o)

all: scan

scan: $(OBJS)
		$(LD) $(LDFLAGS) -o scan $(OBJS)
		
$(OBJS):%.o:%.cpp
		$(CC) $(CFLAGS) -o $@ $<
		
$(SRCS): $(HEADS)

clean:
		rm -f scan $(OBJS)