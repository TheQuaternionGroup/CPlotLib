CC := gcc
CFLAGS := -Iinclude -Wall -Wextra -O2 

OBJ_DIR := obj
SRC_DIR := src
LIB_DIR := lib
SHADER_DIR := shaders

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
LIB := $(LIB_DIR)/libcpl.a

# Look for glew and glfw3 libraries
# CFLAGS += $(shell pkg-config --cflags glew glfw3)
# LDFLAGS += $(shell pkg-config --libs glew glfw3)

all: $(LIB) install-shaders

$(LIB): $(OBJS) | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

install-shaders: | $(SHADER_DIR)
	cp $(SHADER_DIR)/*.glsl $(LIB_DIR)/

clean:
	rm -f $(OBJS) $(LIB)
	rm -f $(LIB_DIR)/*.glsl