CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
TARGET := main

.PHONY: all clean run print

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

print:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"