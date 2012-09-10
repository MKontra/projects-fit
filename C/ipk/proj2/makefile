CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -c

LD=gcc
LDFLAGS= -lpthread

SERVER_SRCS=sserver.c
SERVER_HEADS=

SERVER_OBJS=sserver.o

CLIENT_SRCS=sclient.c
CLIENT_HEADS=

CLIENT_OBJS=sclient.o

all: server client

server: $(SERVER_OBJS)
		$(LD) $(LDFLAGS) -o server $(SERVER_OBJS)
		
client: $(CLIENT_OBJS)
		$(LD) $(LDFLAGS) -o client $(CLIENT_OBJS)
		
sserver.o : sserver.c
		$(CC) $(CFLAGS) -o $@ $<
		
sclient.o : sclient.c
		$(CC) $(CFLAGS) -o $@ $<
		
$(SERVER_SRCS): $(SERVER_HEADS)

$(CLIENT_SRCS): $(CLIENT_HEADS)

clean:
		rm -f server client $(CLIENT_OBJS) $(SERVER_OBJS)