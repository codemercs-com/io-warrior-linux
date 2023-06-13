CXX = g++
CSTD = -std=c++0x
CFLAGS  += -Wall
LDFLAGS = -liowkit

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET = iow28_led

all: $(TARGET)
	@rm -f $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) $(CSTD) $(LDFLAGS) $(CFLAGS) $(OBJS) -o $(TARGET)
	
%.o: %.cpp
	$(CXX) $(CSTD) $(LDFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
