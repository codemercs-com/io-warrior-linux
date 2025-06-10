CXX = g++
CSTD = -std=c++0x
CFLAGS  += -Wall
LDFLAGS = -liowkit -pthread

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET_DIR = bin
TARGET = bmp280

all: $(TARGET)
	@rm -f $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) $(CSTD) $(CFLAGS) $(OBJS) -o $(TARGET_DIR)/$(TARGET) $(LDFLAGS)
	
%.o: %.cpp
	$(CXX) $(CSTD) $(LDFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET_DIR)/$(TARGET)
