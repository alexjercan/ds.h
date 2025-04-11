CC = clang
CFLAGS = -Wall -Wextra -g
BUILD_DIR = build
SRC_DIR = examples
LIB_HEADER = ds.h

# Get a list of all C files in the examples folder
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding object file names in the build folder
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Generate corresponding executable names in the build folder
EXECS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%,$(SRCS))

# Targets
all: $(EXECS)

# Compile each source file into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(LIB_HEADER) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -c $< -o $@

# Link each object file into executables
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o
	$(CC) $(CFLAGS) $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
