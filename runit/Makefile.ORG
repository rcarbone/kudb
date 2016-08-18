CC=			gcc
CFLAGS=		-g -Wall -O2
DFLAGS=
OBJS=		runit.o runlib.o
PROG=		runit runtest
VERSION=	0.1.0
LIBS=		-lpthread

.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(CFLAGS) $(DFLAGS) $< -o $@

all:$(PROG)

runit:$(OBJS)
		$(CC) $(CFLAGS) $(DFLAGS) $(OBJS) -o $@ $(LIBS)

runtest:runtest.o
		$(CC) $(CFLAGS) runtest.o -o $@

cleanlocal:
		rm -f *.o a.out *~ *.a $(PROG) $(MANPAGE)

clean:cleanlocal