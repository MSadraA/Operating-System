# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

# Executable names
BINARIES := extract_and_transform check_pipe

# Source files per binary
SRC_EXTRACT := extract_and_transform.cpp extractor.cpp transformer.cpp shared_func.cpp
SRC_CHECK := check_pipe.cpp

# Object files
OBJ_EXTRACT := $(SRC_EXTRACT:.cpp=.o)
OBJ_CHECK := $(SRC_CHECK:.cpp=.o)

# Default build
.PHONY: all
all: $(BINARIES)

# Build extract_and_transform
extract_and_transform: $(OBJ_EXTRACT)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build check_pipe
check_pipe: $(OBJ_CHECK)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run extract (producer)
.PHONY: run_extract
run_extract: extract_and_transform
	./extract_and_transform

# Run check (consumer)
.PHONY: run_check
run_check: check_pipe
	./check_pipe

# Run both in order (2 terminals)
.PHONY: test_pipe
test_pipe: extract_and_transform check_pipe
	@echo "[+] Running extract_and_transform in background..."
	./extract_and_transform &

	@echo "[+] Running check_pipe to read from pipe..."
	./check_pipe

# Clean everything
.PHONY: clean
clean:
	rm -f *.o $(BINARIES)
	rm -f pipes/extract_pipe pipes/temp_pipe_*
