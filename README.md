# Tetris CLI

Ein klassisches Tetris-Spiel für das Linux-Terminal, implementiert in C11 mit ncurses.

## Features

- **Klassisches Gameplay**: 10x20 Spielfeld mit allen 7 Standard-Tetrominos (I, O, T, S, Z, J, L)
- **Farbige Darstellung**: Jedes Tetromino hat seine eigene Farbe
- **Vorschau**: Zeigt das nächste Tetromino an
- **Progressives Level-System**: Die Geschwindigkeit erhöht sich mit jedem Level
- **Punktesystem**: Belohnt Mehrfach-Linienlöschungen
- **Pause-Funktion**: Pausiere das Spiel jederzeit
- **Wall-Kick**: Verbesserte Rotation mit automatischer Positionsanpassung

## Abhängigkeiten

### Debian/Ubuntu
```bash
sudo apt-get install build-essential libncurses5-dev
```

### Fedora/RHEL
```bash
sudo dnf install gcc ncurses-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel ncurses
```

## Build

### Standard-Build
```bash
make
```

### Debug-Build
```bash
make debug
```

### Tests ausführen
```bash
make test
```

### Spiel starten
```bash
make run
```

### Aufräumen
```bash
make clean
```

## Bedienung

| Taste | Aktion |
|-------|--------|
| ← (Pfeil links) | Nach links bewegen |
| → (Pfeil rechts) | Nach rechts bewegen |
| ↓ (Pfeil runter) | Schneller fallen (Soft Drop) |
| ↑ (Pfeil hoch) | Rotieren (im Uhrzeigersinn) |
| Leertaste | Rotieren (im Uhrzeigersinn) |
| Z | Rotieren (gegen Uhrzeigersinn) |
| X / Enter | Hard Drop (sofort fallen lassen) |
| P | Pause / Fortsetzen |
| Q | Spiel beenden |

## Punktesystem

| Aktion | Punkte |
|--------|--------|
| 1 Zeile (Single) | 100 |
| 2 Zeilen (Double) | 300 |
| 3 Zeilen (Triple) | 500 |
| 4 Zeilen (Tetris) | 800 |
| Soft Drop (pro Zelle) | 1 |
| Hard Drop (pro Zelle) | 2 |

## Projektstruktur

```
tetrisv5/
├── main.c           # Einstiegspunkt, Hauptloop
├── game.c/h         # Spielzustand, Logik, Scoring
├── tetromino.c/h    # Tetromino-Definitionen, Rotation, Kollision
├── renderer.c/h     # ncurses-Zeichenroutinen
├── input.c/h        # Tastatureingabe
├── Makefile         # Build-Konfiguration
├── README.md        # Diese Datei
└── tests/
    ├── minunit.h    # Single-Header Test-Framework
    ├── test_tetromino.c  # Tests für Tetromino-Funktionalität
    └── test_game.c       # Tests für Game-Logik
```

## Technische Details

### Compiler-Flags
- `-std=c11`: C11-Standard
- `-Wall -Wextra -Wpedantic`: Strenge Compiler-Warnungen
- `-D_POSIX_C_SOURCE=200809L`: POSIX.1-2008 Kompatibilität

### Test-Framework
Das Projekt verwendet `minunit.h`, ein minimalistisches Single-Header-Test-Framework.

## Code-Qualität

- **Error Handling**: Alle Systemaufrufe prüfen und bei Fehler sauber beenden
- **Dokumentation**: JSDoc-style Kommentare in Header-Dateien
- **Namenskonventionen**: `snake_case` für Funktionen/Variablen, `UPPER_CASE` für Konstanten
- **Modulare Struktur**: Klare Trennung der Verantwortlichkeiten

## Lizenz

MIT License - Dies ist ein Lernprojekt.

## Bekannte Einschränkungen

- Keine T-Spin-Erkennung
- Keine Hold-Funktion
- Keine Ghost-Piece-Anzeige
- Keine Highscore-Speicherung zwischen Sitzungen

## Mitwirken

Beiträge sind willkommen! Bitte beachte:
- Code muss mit `make test` bestanden werden
- Neue Funktionen erfordern Tests
- Dokumentation in Header-Dateien aktualisieren
