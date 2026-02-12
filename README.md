# Tetris CLI

Ein klassisches Tetris-Spiel für das Linux-Terminal, implementiert in C11 mit ncurses.

## Status

**WP-001: Tetromino-Modul** ✅ Abgeschlossen  
**WP-002: Game Engine** ✅ Abgeschlossen  
**WP-003: UI-Layer** ✅ Abgeschlossen  
- Input-Modul ✅
- Renderer-Modul ✅

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

Für einzelne Test-Suiten:
```bash
make test_tetromino   # Nur Tetromino-Tests
make test_game        # Nur Game Engine-Tests
make test_input       # Nur Input-Modul-Tests
make test_renderer    # Nur Renderer-Modul-Tests
```

## Bedienung

| Taste | Aktion |
|-------|--------|
| ← (Pfeil links) | Nach links bewegen |
| → (Pfeil rechts) | Nach rechts bewegen |
| ↓ (Pfeil runter) | Schneller fallen (Soft Drop) |
| ↑ (Pfeil hoch) | Im Uhrzeigersinn rotieren |
| Leertaste | Sofort fallen (Hard Drop) |
| Z / z | Gegen Uhrzeigersinn rotieren |
| P / p | Pause |
| Q / q | Spiel beenden |

## Architektur

### Module

| Modul | Status | Beschreibung |
|-------|--------|--------------|
| `tetromino` | ✅ | Tetromino-Definitionen, Rotation, Farben |
| `game` | ✅ | Spiellogik, Board, Scoring, Level-System |
| `input` | ✅ | Tastatureingabe mit ncurses |
| `renderer` | ✅ | ncurses-Ausgabe, Farben, UI |
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

### Game Engine API

```c
#include "src/game.h"

// Spiel initialisieren
GameState game;
game_init(&game);

// Spielzüge
game_move_current(&game, 1, 0);    // Nach rechts
game_move_current(&game, -1, 0);   // Nach links
game_move_current(&game, 0, 1);    // Nach unten (Soft Drop)
game_rotate_current(&game, 1);     // Im Uhrzeigersinn rotieren
game_hard_drop(&game);             // Hard Drop (sofort fallen)

// Piece locken (wird automatisch bei Hard Drop gemacht)
int lines_cleared = game_lock_piece(&game);

// Linien löschen und Scoring
int cleared = game_clear_lines(&game);
int points = game_calculate_score(cleared, game.level);

// Spielgeschwindigkeit (für Game Loop)
int speed_ms = game_get_speed_ms(game.level);

// Game Over prüfen
if (game_check_game_over(&game)) {
    // Spiel beendet
}

// Next Piece abfragen
TetrominoType next = game_get_next_type(&game);
```

### Input-Modul API

```c
#include "src/input.h"

// Initialisieren (ncurses muss initialisiert sein)
input_init();

// Input verarbeiten
InputAction action = input_get_action();
switch (action) {
    case INPUT_LEFT:    /* Nach links */ break;
    case INPUT_RIGHT:   /* Nach rechts */ break;
    case INPUT_DOWN:    /* Runter */ break;
    case INPUT_ROTATE_CW:  /* Rotieren */ break;
    case INPUT_HARD_DROP:  /* Hard Drop */ break;
    case INPUT_PAUSE:      /* Pause */ break;
    case INPUT_QUIT:       /* Beenden */ break;
    case INPUT_NONE:       /* Keine Eingabe */ break;
}

// Cleanup
input_cleanup();
```

### Renderer-Modul API

```c
#include "src/renderer.h"

// Initialisieren
renderer_init();

// Komplettes Spiel zeichnen
renderer_draw_game(&game);

// Einzelne Komponenten
renderer_draw_board(&game.board, &game.current);
renderer_draw_sidebar(&game);
renderer_draw_next_piece(game.next.type);
renderer_draw_score(game.score, game.level, game.lines);
renderer_draw_controls();

// Overlays
renderer_draw_pause();
renderer_draw_game_over(game.score);

// Cleanup
renderer_cleanup();
```

### GameState Struktur

```c
typedef struct {
    Board board;               // 10×20 Spielfeld
    Tetromino current;         // Aktuelles Piece
    Tetromino next;            // Nächstes Piece
    int score;                 // Punktestand
    int level;                 // Aktuelles Level (1+)
    int lines;                 // Gelöschte Linien gesamt
    int is_running;            // 1=läuft, 0=Game Over
    int is_paused;             // 1=pausiert, 0=aktiv
} GameState;
```

### Scoring System

| Linien | Punkte (× Level) |
|--------|-----------------|
| 1      | 100             |
| 2      | 300             |
| 3      | 500             |
| 4 (Tetris) | 800         |

Level erhöht sich alle 10 gelöschte Linien.
Fallgeschwindigkeit: `max(100, 1000 - (level-1) * 100)` ms

## Arbeitspakete

- [x] WP-001: Tetromino-Modul
- [x] WP-002: Game Engine
- [x] WP-003: UI-Layer (Input & Renderer)
- [ ] WP-004: Integration

## Coding Standards

- C11 Standard (`-std=c11`)
- Compiler-Warnungen: `-Wall -Wextra -Wpedantic`
- Naming: `snake_case` für Funktionen/Variablen, `UPPER_CASE` für Konstanten
- JSDoc-Kommentare in Header-Dateien
- Unit-Tests für jedes Modul (mindestens 20 Tests pro Modul)
