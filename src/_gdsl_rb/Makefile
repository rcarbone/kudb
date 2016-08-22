CC=			gcc
CFLAGS=		-g -Wall -O2 -fno-diagnostics-color
OBJS=		../benchmark/benchmark.o ../runit/runlib.o test.o
DFLAGS=
# INCLUDES=	-I../benchmark -I../runit -I. -I$(HOME)/opt/include
INCLUDES=	-I../benchmark -I../runit -I. -I/home/rocco/git/containers/gdsl-1.8 -I/home/rocco/git/containers/gdsl-1.8/src
# LIBS=		-L$(HOME)/opt/lib -lgdsl
LIBS=		/home/rocco/git/containers/gdsl-1.8/src/.libs/libgdsl.a

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

