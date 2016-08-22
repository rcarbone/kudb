CC=			gcc
CXX=		g++
CFLAGS=		-g -Wall -O2 -fomit-frame-pointer
DFLAGS=		
CXXFLAGS=	$(CFLAGS)
OBJS=		../benchmark/benchmark.o ../runit/runlib.o test.o
INCLUDES=	-I../benchmark -I../runit -I.
LIBS=		

.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(INCLUDES) $(CFLAGS) $(DFLAGS) $< -o $@

.cc.o:
		$(CXX) -c $(INCLUDES) $(CXXFLAGS) $(DFLAGS) $< -o $@

all:test

test:$(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

test.o:

cleanlocal:
		rm -f *.o a.out *~ test

clean:cleanlocal

