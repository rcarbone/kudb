CC=			gcc
CXX=		g++
CFLAGS=		-g -Wall -O2 -fomit-frame-pointer
CXXFLAGS=	$(CFLAGS)
OBJS=		../benchmark/benchmark.o ../runit/runlib.o test.o
DFLAGS=
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

test2:../benchmark/benchmark.o ../runit/runlib.o test2.o
		$(CXX) $(CXXFLAGS) -o $@ ../benchmark/benchmark.o ../runit/runlib.o test2.o $(LIBS)

cleanlocal:
		rm -f *.o a.out *~ test test2

clean:cleanlocal

