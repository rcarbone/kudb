CC=			gcc
CFLAGS=		-g -Wall -O2 -fomit-frame-pointer
OBJS=		../benchmark/benchmark.o ../runit/runlib.o test.o
DFLAGS=
INCLUDES=	-I../benchmark -I../runit `pkg-config --cflags glib-2.0`
LIBS=		`pkg-config --libs glib-2.0`

.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(INCLUDES) $(CFLAGS) $(DFLAGS) $< -o $@

all:test

test:$(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

cleanlocal:
		rm -f *.o a.out *~ test

clean:cleanlocal

