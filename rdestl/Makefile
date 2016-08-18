CC=			gcc
CXX=		g++
CFLAGS=		-g -Wall -O2 -fomit-frame-pointer -Wno-deprecated -Wno-unused-value -Wno-uninitialized
CXXFLAGS=	$(CFLAGS)
OBJS=		../benchmark/benchmark.o ../runit/runlib.o test.o \
			rdestl/allocator.o rdestl/intrusive_list.o \
			rdestl/list.o rdestl/slist.o
DFLAGS=
INCLUDES=	-I../benchmark -I../runit -I.
LIBS=		

.SUFFIXES:.c .o .cc .cpp

.c.o:
		$(CC) -c $(INCLUDES) $(CFLAGS) $(DFLAGS) $< -o $@

.cc.o:
		$(CXX) -c $(INCLUDES) $(CXXFLAGS) $(DFLAGS) $< -o $@

.cpp.o:
		$(CXX) -c $(INCLUDES) $(CXXFLAGS) $(DFLAGS) $< -o $@

all:test

test:$(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

cleanlocal:
		rm -f *.o a.out *~ test rdestl/*.o rdestl/*~

clean:cleanlocal

