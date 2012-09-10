CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -c

LD=gcc
LDFLAGS=

SERVER_SRCS=server.c rdtsegment.c
SERVER_HEADS=rdtsegment.h

SERVER_OBJS=$(SERVER_SRCS:.c=.o)

CLIENT_SRCS=client.c rdtsegment.c
CLIENT_HEADS=rdtsegment.h

CLIENT_OBJS=$(CLIENT_SRCS:.c=.o)

all: server client

server: $(SERVER_OBJS)
		$(LD) $(LDFLAGS) -o rdtserver $(SERVER_OBJS)
		
client: $(CLIENT_OBJS)
		$(LD) $(LDFLAGS) -o rdtclient $(CLIENT_OBJS)
		
server.o: $(SERVER_SRCS)
		$(CC) $(CFLAGS) -o $@ $<
		
client.o: $(CLIENT_SRCS)
		$(CC) $(CFLAGS) -o $@ $<
		
rdtsegment.o: rdtsegment.c
		$(CC) $(CFLAGS) -o $@ $<
		
$(SERVER_SRCS): $(SERVER_HEADS)

$(CLIENT_SRCS): $(CLIENT_HEADS)

clean:
		rm -f rdtserver rdtclient $(CLIENT_OBJS) $(SERVER_OBJS)