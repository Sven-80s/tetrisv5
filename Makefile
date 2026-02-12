# Tetris CLI Makefile
# Supports: all, clean, test

# Compiler settings
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L
CFLAGS_DEBUG = -g -O0
CFLAGS_RELEASE = -O2

# Linker flags
LDFLAGS = -lncurses

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
	rm -f tetris test_tetromino test_game test_input test_renderer

# Run all tests
test: test_tetromino test_game test_input test_renderer test_integration
	@./test_tetromino
	@./test_integration
	@./test_game
	@./test_input
	@./test_renderer

# Tetromino tests
test_tetromino: $(TESTBUILDDIR)/test_tetromino.o $(BUILDDIR)/tetromino.o | $(TESTBUILDDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# Game engine tests
test_game: $(TESTBUILDDIR)/test_game.o $(BUILDDIR)/game.o $(BUILDDIR)/tetromino.o | $(TESTBUILDDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# Input tests
test_input: $(TESTBUILDDIR)/test_input.o $(BUILDDIR)/input.o | $(TESTBUILDDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# Renderer tests
test_renderer: $(TESTBUILDDIR)/test_renderer.o $(BUILDDIR)/renderer.o $(BUILDDIR)/tetromino.o $(BUILDDIR)/game.o | $(TESTBUILDDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile test files
$(TESTBUILDDIR)/test_tetromino.o: $(TESTDIR)/test_tetromino.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTBUILDDIR)/test_game.o: $(TESTDIR)/test_game.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTBUILDDIR)/test_input.o: $(TESTDIR)/test_input.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTBUILDDIR)/test_renderer.o: $(TESTDIR)/test_renderer.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Print available targets
help:
	@echo "Available targets:"
	@echo "  all          - Build all source files"
	@echo "  test         - Run all unit tests"
	@echo "  test_tetromino - Run tetromino tests only"
	@echo "  test_game    - Run game engine tests only"
	@echo "  test_input   - Run input module tests only"
	@echo "  test_renderer- Run renderer module tests only"
	@echo "  debug        - Build with debug symbols"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this help"

# Integration tests
test_integration: $(TESTBUILDDIR)/test_integration.o $(BUILDDIR)/game.o $(BUILDDIR)/tetromino.o | $(TESTBUILDDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(TESTBUILDDIR)/test_integration.o: $(TESTDIR)/test_integration.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

