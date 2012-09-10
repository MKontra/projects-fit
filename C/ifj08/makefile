CFLAGS=-std=c99 -Wall -pedantic
CC=gcc

COMLIBS=str.c bvs.c ilist.c codes.c istacka.c
COMOBJS=$(COMLIBS:.c=.o)
PROJPARTS=main.c parser.c lexer.c exprparser.c interpret.c
PROJOBJS=$(PROJPARTS:.c=.o)

BIN=project
RM=del

ALL: $(COMOBJS) $(PROJOBJS)
	$(CC) $(CFLAGS) -o $(BIN) $(COMOBJS) $(PROJOBJS)

DEBUG: $(COMOBJS) $(PROJOBJS)
	$(CC) $(CFLAGS) -D DEBUG -o $(BIN) $(COMOBJS) $(PROJOBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

ptest: ptest.c $(COMOBJS) $(PROJOBJS)
	$(CC) $(CFLAGS) -o ptest.exe ptest.c $(COMOBJS) $(PROJOBJS)
	
lexertest: lexer.c lexer.h $(COMOBJS)
	$(CC) $(CFLAGS) -o lexertest.exe lexer.c lexer.h lexertest.c $(COMOBJS)
	
strtest: str.c str.h strtest.c
	$(CC) $(CFLAGS) -o strtest.exe strtest.c str.c str.h
	
symtest: symtest.c $(COMOBJS)
	$(CC) $(CFLAGS) -o symtest.exe symtest.c $(COMOBJS)
	
$(COMOBJS): str.h bvs.h ilist.h codes.h istacka.h

$(PROJOBJS): parser.h lexer.h exprparser.h interpret.h

clean:
	$(RM) *.o $(BIN)