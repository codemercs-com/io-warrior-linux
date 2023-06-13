CSTD = -std=c++0x
CFLAGS  += -Wall
LDFLAGS = -liowkit

all: iow100_dac

iow100_dac:
	g++ $(CSTD) $(LDFLAGS) $(CFLAGS) iow100.cpp CIow100DacClass.cpp -o iow100_dac

clean:
	rm -rf *.o
	rm iow100_dac
