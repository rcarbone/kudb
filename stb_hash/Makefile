CC=			gcc
CFLAGS=		-g -Wall -O2 -fomit-frame-pointer -Wno-unused-variable -Wno-unused-function -Wno-unused-value -Wno-parentheses -Wno-implicit-int -Wno-return-type
CFLAGS += -Wno-strict-aliasing -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-int-to-pointer-cast -Wno-incompatible-pointer-types
OBJS=		../benchmark/benchmark.o ../runit/runlib.o test.o
DFLAGS=
INCLUDES=	-I../benchmark -I../runit
LIBS= -lm

.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(INCLUDES) $(CFLAGS) $(DFLAGS) $< -o $@

all:test

test:$(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

cleanlocal:
		rm -f *.o a.out *~ test

clean:cleanlocal

