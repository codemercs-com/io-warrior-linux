CSTD = -std=c++0x
CFLAGS  += -Wall
LDFLAGS = -liowkit

all: iow28_io iow100_io

iow28_io:
	g++ $(CSTD) $(LDFLAGS) $(CFLAGS) iow28.cpp -o iow28_io

iow100_io:
	g++ $(CSTD) $(LDFLAGS) $(CFLAGS) iow100.cpp -o iow100_io

clean:
	rm -rf *.o
	rm iow100_io
	rm iow28_io
