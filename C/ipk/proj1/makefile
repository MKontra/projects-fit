CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -c

LD=gcc
LDFLAGS=

SRCS=webclient.c
HEADS=

OBJS=$(SRCS:.c=.o)

all: webclientapp

webclientapp: $(OBJS)
		$(LD) $(LDFLAGS) -o webclient $(OBJS)
		
$(OBJS):%.o:%.c
		$(CC) $(CFLAGS) -o $@ $<
		
$(SRCS): $(HEADS)

clean:
		rm -f webclient $(OBJS)