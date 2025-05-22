# ========== Paths ==========
SRC_PARALLEL := parallel
SRC_SERIAL := serial
SRC_SHARED := shared
BIN_DIR := bin
BUILD_DIR := build
DATA_DIR := data
PARAM_DIR := net_params

# ========== Compiler & Flags ==========
CXX := g++
CXXFLAGS := -std=c++17 -Wall -I$(SRC_PARALLEL) -I$(SRC_SHARED) -I$(DATA_DIR) -I$(PARAM_DIR)
LDFLAGS := -pthread

# ========== Sources & Objects ==========
PARALLEL_SRC := $(wildcard $(SRC_PARALLEL)/*.cpp)
PARALLEL_OBJ := $(patsubst $(SRC_PARALLEL)/%.cpp,$(BUILD_DIR)/%.o,$(PARALLEL_SRC))

SERIAL_SRC := $(wildcard $(SRC_SERIAL)/*.cpp)
SERIAL_OBJ := $(patsubst $(SRC_SERIAL)/%.cpp,$(BUILD_DIR)/%.o,$(SERIAL_SRC))

SHARED_SRC := $(wildcard $(SRC_SHARED)/*.cpp)
SHARED_OBJ := $(patsubst $(SRC_SHARED)/%.cpp,$(BUILD_DIR)/%.o,$(SHARED_SRC))

RUNNER_SRC := runner.cpp
RUNNER_OBJ := $(BUILD_DIR)/runner.o

# ========== Output Binaries ==========
PARALLEL_EXEC := $(BIN_DIR)/parallel_exec
SERIAL_EXEC := $(BIN_DIR)/serial_exec
RUNNER_EXEC := $(BIN_DIR)/runner_exec

# ========== Default Target ==========
all: $(PARALLEL_EXEC) $(SERIAL_EXEC) $(RUNNER_EXEC)

# ========== Compile Rules ==========
$(BUILD_DIR)/%.o: $(SRC_PARALLEL)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_SERIAL)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_SHARED)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/runner.o: runner.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ========== Linking ==========
$(PARALLEL_EXEC): $(PARALLEL_OBJ) $(SHARED_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[INFO] Setting cap_sys_nice on $@"
	@which setcap >/dev/null && sudo setcap 'cap_sys_nice=eip' $@ || echo "[WARN] setcap not available or failed."

$(SERIAL_EXEC): $(SERIAL_OBJ) $(SHARED_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(RUNNER_EXEC): $(RUNNER_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# ========== Run Program ==========
run: all
	@echo "[INFO] Running runner_exec..."
	$(RUNNER_EXEC) 2 128 10000

# ========== Clean ==========
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean run
