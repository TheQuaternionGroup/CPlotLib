CC := gcc
CFLAGS := -Iinclude -Wall -Wextra -O2 

OBJ_DIR := obj
SRC_DIR := src
LIB_DIR := lib
SHADER_DIR := shaders
TEST_DIR := test
TEST_BUILD_DIR := test/build

# Find source files in subdirectories
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
LIB := $(LIB_DIR)/libcpl.a

.PHONY: all clean

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

$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

test: $(LIB) | $(TEST_BUILD_DIR)
	clang $(TEST_DIR)/test2.c -L$(LIB_DIR) -Iinclude -lcpl -o $(TEST_BUILD_DIR)/test2 \
		-framework OpenGL -lm $(shell pkg-config --cflags --libs glew glfw3)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	find $(TEST_BUILD_DIR) -type f -delete