CC=			gcc
CFLAGS=		-g -Wall -O2
OBJS=		../benchmark/benchmark.o ../runit/runlib.o test.o bt_code.o
DFLAGS=
INCLUDES=	-I../benchmark -I../runit -I.
LIBS=		

.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(INCLUDES) $(CFLAGS) $(DFLAGS) $< -o $@

all:test

test:$(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

test.o:

cleanlocal:
		rm -f *.o a.out *~ test

clean:cleanlocal

