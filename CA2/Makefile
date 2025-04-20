# Compiler settings
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Ishared -Isrc

# Directories
BINDIR := bin
SRCDIR := src
SHAREDDIR := shared
MAINDIR := main

# Source files
SRC := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SHAREDDIR)/*.cpp)
OBJ := $(SRC:.cpp=.o)

# Apps with main() (each will become a binary)
APPS := $(basename $(notdir $(wildcard $(MAINDIR)/*.cpp)))
BINAPPS := $(addprefix $(BINDIR)/,$(APPS))

# Default target
.PHONY: all
all: $(BINDIR) $(BINAPPS)

# Ensure bin/ exists
$(BINDIR):
	mkdir -p $(BINDIR)

# Build each binary from main/
$(BINDIR)/%: $(MAINDIR)/%.cpp $(OBJ)
	@echo "[+] Building $@"
	$(CXX) $(CXXFLAGS) -o $@ $^

# Get the CLI args after `run`
WORKER_NUM := $(word 2, $(MAKECMDGOALS))
FILE_NUM := $(word 3, $(MAKECMDGOALS))

# Run controller
.PHONY: run
run: all
	@echo "[+] Running controller with worker_num=$(WORKER_NUM), file_num=$(FILE_NUM)"
	./$(BINDIR)/controller $(WORKER_NUM) $(FILE_NUM)

# Clean up
.PHONY: clean
clean:
	rm -f *.o $(SRCDIR)/*.o $(SHAREDDIR)/*.o
	rm -rf $(BINDIR)
	rm -f pipes/*
	rm -f controller

# Prevent errors when running `make run 3 5` due to unknown goals
%:
	@:
