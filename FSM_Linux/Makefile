# mimimal Makefile

all: fsm

CFLAGS = -g 

fsm: fsm.c util.o
util.o: util.h

.o : .c

clean: 
	rm -f fsm *.o
