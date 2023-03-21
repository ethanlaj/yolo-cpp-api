CXX = g++
CFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = $(shell pkg-config --libs opencv4) -lm
INCLUDES = $(shell pkg-config --cflags opencv4)

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = yolov7_example

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
