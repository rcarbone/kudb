CC=			gcc
CFLAGS=		-g -Wall -O2
OBJS=		../benchmark/benchmark.o ../runit/runlib.o khtest.o
DFLAGS=
INCLUDES=	-I../benchmark -I../runit
LIBS=		

.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(INCLUDES) $(CFLAGS) $(DFLAGS) $< -o $@

run:$(OBJS)
		$(CC) -o $@ $(OBJS) $(LIBS)

cleanlocal:
		rm -f *.o a.out *~ run

clean:cleanlocal

