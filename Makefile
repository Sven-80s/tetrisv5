# Makefile für Tetris CLI Spiel

# Compiler und Flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
LDFLAGS = -lncurses

# Verzeichnisse
SRC_DIR = .
TEST_DIR = tests
BUILD_DIR = build

# Quell-Dateien
SRCS = main.c game.c tetromino.c renderer.c input.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# Ziel-Dateien
TARGET = tetris
TEST_TARGET_TETROMINO = test_tetromino
TEST_TARGET_GAME = test_game

# Standard-Target
.PHONY: all clean test run

all: $(BUILD_DIR) $(TARGET)

# Build-Verzeichnis erstellen
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/tests

# Haupt-Programm bauen
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Objekt-Dateien kompilieren
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: tests/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Test-Programme bauen (ohne ncurses und main.c)
test: $(BUILD_DIR) test-tetromino test-game
	@echo ""
	@echo "Running all tests..."
	@./$(TEST_TARGET_TETROMINO) && ./$(TEST_TARGET_GAME)
	@echo ""

test-tetromino: $(TEST_TARGET_TETROMINO)
test-game: $(TEST_TARGET_GAME)

$(TEST_TARGET_TETROMINO): build/tetromino.o build/tests/test_tetromino.o
	$(CC) build/tetromino.o build/tests/test_tetromino.o -o $(TEST_TARGET_TETROMINO)

$(TEST_TARGET_GAME): build/game.o build/tetromino.o build/tests/test_game.o
	$(CC) build/game.o build/tetromino.o build/tests/test_game.o -o $(TEST_TARGET_GAME)

# Spiel ausführen
run: $(TARGET)
	./$(TARGET)

# Debug-Build
debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -O0 -DDEBUG
debug: clean all

# Sauber machen
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(TEST_TARGET_TETROMINO) $(TEST_TARGET_GAME)

# Hilfe
help:
	@echo "Verfügbare Targets:"
	@echo "  make        - Baut das Hauptprogramm"
	@echo "  make test   - Baut und führt Tests aus"
	@echo "  make run    - Baut und startet das Spiel"
	@echo "  make debug  - Baut Debug-Version"
	@echo "  make clean  - Löscht alle Build-Dateien"
	@echo "  make help   - Zeigt diese Hilfe"

# Abhängigkeiten
$(BUILD_DIR)/main.o: main.c game.h renderer.h input.h
$(BUILD_DIR)/game.o: game.c game.h tetromino.h
$(BUILD_DIR)/tetromino.o: tetromino.c tetromino.h
$(BUILD_DIR)/renderer.o: renderer.c renderer.h game.h tetromino.h
$(BUILD_DIR)/input.o: input.c input.h game.h
$(BUILD_DIR)/tests/test_tetromino.o: tests/test_tetromino.c tetromino.h tests/minunit.h
$(BUILD_DIR)/tests/test_game.o: tests/test_game.c game.h tetromino.h tests/minunit.h
