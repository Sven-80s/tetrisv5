# Tetris CLI Makefile
# Supports: all, clean, test

# Compiler settings
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L
CFLAGS_DEBUG = -g -O0
CFLAGS_RELEASE = -O2

# Directories
SRCDIR = src
TESTDIR = tests
BUILDDIR = build
TESTBUILDDIR = $(BUILDDIR)/tests

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

# Test files
TEST_SRCS = $(wildcard $(TESTDIR)/test_*.c)
TEST_BINS = $(patsubst $(TESTDIR)/%.c,%,$(TEST_SRCS))

# Targets
.PHONY: all clean test debug

# Default target: build all executables
all: tetris

# Create build directories
$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(TESTBUILDDIR):
	@mkdir -p $(TESTBUILDDIR)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) -c $< -o $@

# Main executable (placeholder - will be built in WP-004)
tetris: $(OBJS)
	@echo "Main executable will be built in WP-004 (Integration)"
	@echo "Current objects: $(OBJS)"

# Debug build
debug: CFLAGS_RELEASE = $(CFLAGS_DEBUG)
debug: all

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)
	rm -f tetris test_tetromino test_game

# Run tests
test: test_tetromino
	@./test_tetromino

test_tetromino: $(TESTBUILDDIR)/test_tetromino.o $(BUILDDIR)/tetromino.o | $(TESTBUILDDIR)
	$(CC) $^ -o $@

# Compile test files
$(TESTBUILDDIR)/test_tetromino.o: $(TESTDIR)/test_tetromino.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTBUILDDIR)/test_game.o: $(TESTDIR)/test_game.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Print available targets
help:
	@echo "Available targets:"
	@echo "  all      - Build all source files (objects only for WP-001)"
	@echo "  test     - Run tetromino tests"
	@echo "  debug    - Build with debug symbols"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help"
