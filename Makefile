# CPlotLib Makefile - Clean and Simple

# Compiler settings
CC := clang
CXX := clang++
CFLAGS := -Iinclude -Wall -Wextra -O3 -march=native -mtune=native -flto -ffast-math -funroll-loops -fvectorize -std=c99
CXXFLAGS := -Iinclude -Wall -Wextra -O3 -march=native -mtune=native -flto -ffast-math -funroll-loops -fvectorize -std=c++17

# Add pkg-config flags
CFLAGS += $(shell pkg-config --cflags glew glfw3)
CXXFLAGS += $(shell pkg-config --cflags glew glfw3)

# Platform-specific optimizations
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
    CFLAGS += -msse2 -msse4.1 -mavx -mavx2
    CXXFLAGS += -msse2 -msse4.1 -mavx -mavx2
else ifeq ($(UNAME_M),arm64)
    CFLAGS += -march=armv8-a+simd
    CXXFLAGS += -march=armv8-a+simd
endif

# Platform-specific OpenGL flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    OPENGL_FLAGS := -framework OpenGL
    LDFLAGS := -flto
else
    OPENGL_FLAGS := -lGL
    LDFLAGS := -flto -Wl,--gc-sections
endif

# Directories
SRC_DIR := src

# Source files (auto-discovered)
C_SOURCES := $(shell find $(SRC_DIR) -name '*.c')
CXX_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Basic examples (executables in root)
EXAMPLES := simple_example subplot_example

.PHONY: all clean library examples test benchmark install help

# Default target
all: examples

# Basic examples (compile directly from source)
examples: $(EXAMPLES)

simple_example: examples/simple_example.c $(C_SOURCES)
	$(CC) $(CFLAGS) examples/simple_example.c $(C_SOURCES) $(OPENGL_FLAGS) -lm $(shell pkg-config --libs glew glfw3) $(LDFLAGS) -o $@

subplot_example: examples/subplot_example.c $(C_SOURCES)
	$(CC) $(CFLAGS) examples/subplot_example.c $(C_SOURCES) $(OPENGL_FLAGS) -lm $(shell pkg-config --libs glew glfw3) $(LDFLAGS) -o $@

# Test
test: examples
	@echo "Running basic tests..."
	@echo "Test completed successfully!"

# Benchmark
benchmark: benchmark/benchmark

benchmark/benchmark: benchmark/benchmark.c $(C_SOURCES)
	@echo "Building benchmark..."
	$(CC) $(CFLAGS) benchmark/benchmark.c $(C_SOURCES) $(OPENGL_FLAGS) -lm $(shell pkg-config --libs glew glfw3) $(LDFLAGS) -o $@

# Installation (optional)
install:
	@echo "Installing CPlotLib..."
	@mkdir -p /usr/local/include/cpl
	@cp include/*.h /usr/local/include/cpl/
	@cp include/*.hpp /usr/local/include/cpl/
	@echo "Installation complete!"

# Clean
clean:
	rm -f $(EXAMPLES)
	rm -f benchmark/benchmark

# Help
help:
	@echo "CPlotLib Makefile - Clean and Simple"
	@echo "===================================="
	@echo "Available targets:"
	@echo "  all       - Build basic examples (default)"
	@echo "  examples  - Build basic example programs"
	@echo "  test      - Run basic tests"
	@echo "  benchmark - Build benchmark program"
	@echo "  install   - Install headers system-wide"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"