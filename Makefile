# Compiler and base flags
CC = gcc
BASE_CFLAGS = -Wall -Wextra
LIBS = -lm -lsqlite3

SRC_DIR = src
OUT_DIR = out
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)

# Default to production build
BUILD ?= release

ifeq ($(BUILD),debug)
    CFLAGS = $(BASE_CFLAGS) -g -DDBG
    TARGET := $(OUT_DIR)/sparse_calc_dbg
else ifeq ($(BUILD),test)
    CFLAGS = $(BASE_CFLAGS) -g -DTEST
    TARGET := $(OUT_DIR)/sparse_calc_test
else
    CFLAGS = $(BASE_CFLAGS) -O2
	TARGET := $(OUT_DIR)/sparse_calc
endif

# Default target
all: info $(TARGET)

info:
	@echo "Building mode: $(BUILD)"
	@echo "Target: $(TARGET)"

# Create output directory and link executable
$(TARGET): $(OBJ)
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile to object files
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean all builds
clean:
	rm -rf $(OUT_DIR)
