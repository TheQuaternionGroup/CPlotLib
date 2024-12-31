CC := gcc
CFLAGS := -Iinclude -Wall -Wextra -O2 

OBJ_DIR := obj
SRC_DIR := src
LIB_DIR := lib
SHADER_DIR := shaders

# Find source files in subdirectories
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
LIB := $(LIB_DIR)/libcpl.a

all: $(LIB)

$(LIB): $(OBJS) | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Create obj/ subdirectory structure automatically
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJS) $(LIB)
	rm -f $(LIB_DIR)/*.glsl