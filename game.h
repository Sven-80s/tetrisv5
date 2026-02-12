#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "tetromino.h"

/** @file game.h
 *  @brief Spielzustand, Logik und Scoring
 */

/** Standard-Breite des Spielfelds */
#define BOARD_WIDTH 10

/** Standard-Höhe des Spielfelds */
#define BOARD_HEIGHT 20

/** Startgeschwindigkeit in Millisekunden */
#define INITIAL_SPEED 1000

/** Geschwindigkeitsabnahme pro Level */
#define SPEED_DECREMENT 50

/** Minimale Geschwindigkeit */
#define MIN_SPEED 100

/** Punkte für eine einzelne Zeile */
#define SCORE_SINGLE 100

/** Punkte für zwei Zeilen */
#define SCORE_DOUBLE 300

/** Punkte für drei Zeilen */
#define SCORE_TRIPLE 500

/** Punkte für vier Zeilen (Tetris) */
#define SCORE_TETRIS 800

/** Zeilen bis zum nächsten Level */
#define LINES_PER_LEVEL 10

/** Spielzustände */
typedef enum {
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

/** Spielzustandsstruktur */
typedef struct {
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    Tetromino current_piece;
    Tetromino next_piece;
    int score;
    int level;
    int lines_cleared;
    int speed_ms;
    GameState state;
    bool running;
} Game;

/**
 * @brief Initialisiert ein neues Spiel
 * @param game Pointer auf die Game-Struktur
 * @return true bei Erfolg, false bei Fehler
 */
bool game_init(Game *game);

/**
 * @brief Setzt das Spiel zurück für einen Neustart
 * @param game Pointer auf die Game-Struktur
 */
void game_reset(Game *game);

/**
 * @brief Beendet das Spiel und gibt Ressourcen frei
 * @param game Pointer auf die Game-Struktur
 */
void game_cleanup(Game *game);

/**
 * @brief Aktualisiert den Spielzustand (Tick)
 * @param game Pointer auf die Game-Struktur
 * @return true wenn das Spiel läuft, false bei Game Over
 */
bool game_update(Game *game);

/**
 * @brief Versucht das aktuelle Tetromino zu bewegen
 * @param game Pointer auf die Game-Struktur
 * @param dx Delta-X (-1 links, +1 rechts)
 * @param dy Delta-Y (+1 runter)
 * @return true wenn Bewegung erfolgreich
 */
bool game_move_piece(Game *game, int dx, int dy);

/**
 * @brief Rotiert das aktuelle Tetromino
 * @param game Pointer auf die Game-Struktur
 * @param clockwise true für im Uhrzeigersinn
 * @return true wenn Rotation erfolgreich
 */
bool game_rotate_piece(Game *game, bool clockwise);

/**
 * @brief Lässt das aktuelle Tetromino sofort fallen
 * @param game Pointer auf die Game-Struktur
 * @return Anzahl der Zeilen die es fiel
 */
int game_hard_drop(Game *game);

/**
 * @brief Fixiert das aktuelle Tetromino auf dem Board
 * @param game Pointer auf die Game-Struktur
 */
void game_lock_piece(Game *game);

/**
 * @brief Überprüft und löscht vollständige Zeilen
 * @param game Pointer auf die Game-Struktur
 * @return Anzahl der gelöschten Zeilen
 */
int game_clear_lines(Game *game);

/**
 * @brief Berechnet Punkte basierend auf gelöschten Zeilen
 * @param lines Anzahl der gleichzeitig gelöschten Zeilen
 * @return Die zu vergebenden Punkte
 */
int game_calculate_score(int lines);

/**
 * @brief Spawnt ein neues Tetromino
 * @param game Pointer auf die Game-Struktur
 * @return true bei Erfolg, false wenn Spawn blockiert (Game Over)
 */
bool game_spawn_piece(Game *game);

/**
 * @brief Aktualisiert das Level basierend auf gelöschten Zeilen
 * @param game Pointer auf die Game-Struktur
 */
void game_update_level(Game *game);

/**
 * @brief Pausiert oder setzt das Spiel fort
 * @param game Pointer auf die Game-Struktur
 */
void game_toggle_pause(Game *game);

/**
 * @brief Prüft ob das Spiel vorbei ist
 * @param game Pointer auf die Game-Struktur
 * @return true bei Game Over
 */
bool game_is_game_over(const Game *game);

/**
 * @brief Gibt die aktuelle Geschwindigkeit zurück
 * @param game Pointer auf die Game-Struktur
 * @return Geschwindigkeit in Millisekunden
 */
int game_get_speed(const Game *game);

/**
 * @brief Gibt den Inhalt einer Board-Zelle zurück
 * @param game Pointer auf die Game-Struktur
 * @param x X-Koordinate
 * @param y Y-Koordinate
 * @return Farbwert (0 = leer)
 */
int game_get_board_cell(const Game *game, int x, int y);

/**
 * @brief Setzt den Wert einer Board-Zelle
 * @param game Pointer auf die Game-Struktur
 * @param x X-Koordinate
 * @param y Y-Koordinate
 * @param value Der zu setzende Wert
 */
void game_set_board_cell(Game *game, int x, int y, int value);

#endif /* GAME_H */
