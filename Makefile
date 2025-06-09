# Compiler settings
CXX = g++
CXXFLAGS_BASE = -Wall -Wextra -std=c++17 -Iinc -O3 -march=native -ffast-math
CXXFLAGS_SERIAL = $(CXXFLAGS_BASE)
CXXFLAGS_OMP = $(CXXFLAGS_BASE) -fopenmp
LDFLAGS_BASE = -lsfml-graphics -lsfml-window -lsfml-system
LDFLAGS_SERIAL = $(LDFLAGS_BASE)
LDFLAGS_OMP = $(LDFLAGS_BASE) -fopenmp

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# Common source files (exclude Simulation.cpp and SimulationOMP.cpp)
COMMON_SOURCES = $(filter-out $(SRC_DIR)/Simulation.cpp $(SRC_DIR)/SimulationOMP.cpp, $(wildcard $(SRC_DIR)/*.cpp))
COMMON_OBJECTS = $(COMMON_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Serial version objects
SERIAL_OBJECTS = $(COMMON_OBJECTS) $(OBJ_DIR)/Simulation.o
SERIAL_EXECUTABLE = $(BIN_DIR)/nbody_simulation_serial

# OMP version objects  
OMP_OBJECTS = $(COMMON_OBJECTS) $(OBJ_DIR)/SimulationOMP.o
OMP_EXECUTABLE = $(BIN_DIR)/nbody_simulation_omp

# Default target - build both executables
all: $(SERIAL_EXECUTABLE) $(OMP_EXECUTABLE)

# Serial executable
serial: $(SERIAL_EXECUTABLE)

# OMP executable
omp: $(OMP_EXECUTABLE)

# Link serial version
$(SERIAL_EXECUTABLE): $(SERIAL_OBJECTS) | $(BIN_DIR)
	$(CXX) $(SERIAL_OBJECTS) -o $@ $(LDFLAGS_SERIAL)

# Link OMP version
$(OMP_EXECUTABLE): $(OMP_OBJECTS) | $(BIN_DIR)
	$(CXX) $(OMP_OBJECTS) -o $@ $(LDFLAGS_OMP)

# Compile common source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS_SERIAL) -c $< -o $@

# Compile Simulation.cpp for serial version
$(OBJ_DIR)/Simulation.o: $(SRC_DIR)/Simulation.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS_SERIAL) -c $< -o $@

# Compile SimulationOMP.cpp for OMP version
$(OBJ_DIR)/SimulationOMP.o: $(SRC_DIR)/SimulationOMP.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS_OMP) -c $< -o $@

# Create directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run serial version
run-serial: $(SERIAL_EXECUTABLE)
	./$(SERIAL_EXECUTABLE)

# Run OMP version
run-omp: $(OMP_EXECUTABLE)
	./$(OMP_EXECUTABLE)

# Show help
help:
	@echo "Available targets:"
	@echo "  all         - Build both serial and OMP versions (default)"
	@echo "  serial      - Build only serial version"
	@echo "  omp         - Build only OMP version"
	@echo "  clean       - Remove all build artifacts"
	@echo "  run-serial  - Build and run serial version"
	@echo "  run-omp     - Build and run OMP version"
	@echo "  help        - Show this help message"

# Phony targets
.PHONY: all serial omp clean clean-obj clean-bin install run-serial run-omp help
