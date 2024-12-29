CC := gcc
CFLAGS := -Iinclude -Wall -Wextra -O2 -framework OpenGL -lm

OBJ_DIR := obj
SRC_DIR := src
LIB_DIR := lib

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
LIB  := $(LIB_DIR)/cpl.a

# look for glew and glfw3 libraries
CFLAGS += $(shell pkg-config --cflags glew glfw3)
LDFLAGS += $(shell pkg-config --libs glew glfw3)


all: $(LIB)

# Ensure lib/ directory exists before creating the library
$(LIB): $(OBJS) | $(LIB_DIR)
	ar rcs $@ $^

# Rule to create lib/ directory
$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create obj/ directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJS) $(LIB)