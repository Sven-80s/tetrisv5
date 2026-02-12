# Tetris CLI

Ein klassisches Tetris-Spiel für das Linux-Terminal, implementiert in C11 mit ncurses.

## Status

**WP-001: Tetromino-Modul** ✅ Abgeschlossen

## Build

### Abhängigkeiten

```bash
# Debian/Ubuntu
sudo apt-get install build-essential libncurses5-dev

# Fedora/RHEL
sudo dnf install gcc ncurses-devel

# Arch Linux
sudo pacman -S base-devel ncurses
```

### Kompilieren

```bash
make
```

### Tests ausführen

```bash
make test
```

## Architektur

### Module

| Modul | Status | Beschreibung |
|-------|--------|--------------|
| `tetromino` | ✅ | Tetromino-Definitionen, Rotation, Farben |
| `game` | ⏳ | Spiellogik, Board, Scoring |
| `input` | ⏳ | Tastatureingabe |
| `renderer` | ⏳ | ncurses-Ausgabe |
| `main` | ⏳ | Hauptprogramm, Game-Loop |

### Tetromino-Modul API

```c
#include "src/tetromino.h"

// Tetromino erstellen
Tetromino t = tetromino_create(TETRO_T);

// Rotieren
int new_rot = tetromino_rotate_clockwise(t.rotation);

// Shape-Matrix abrufen
const int (*shape)[4] = tetromino_get_shape(TETRO_T, 0);

// Farbe für ncurses
int color = tetromino_get_color(TETRO_T);  // Returns COLOR_T (3)

// Bewegen
void tetromino_move(&t, 1, 0);  // 1 nach rechts
```

## Arbeitspakete

- [x] WP-001: Tetromino-Modul
- [ ] WP-002: Game Engine
- [ ] WP-003: UI-Layer (Input & Renderer)
- [ ] WP-004: Integration

## Coding Standards

- C11 Standard (`-std=c11`)
- Compiler-Warnungen: `-Wall -Wextra -Wpedantic`
- Naming: `snake_case` für Funktionen/Variablen, `UPPER_CASE` für Konstanten
- JSDoc-Kommentare in Header-Dateien
- Unit-Tests für jedes Modul
