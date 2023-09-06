CXX = g++
CSTD = -std=c++0x
CFLAGS  += -Wall
LDFLAGS = -liowkit

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET = multiple_iows

all: $(TARGET)
	@rm -f $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) $(CSTD)  $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)
	
%.o: %.cpp
	$(CXX) $(CSTD) $(LDFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)