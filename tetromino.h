#ifndef TETROMINO_H
#define TETROMINO_H

#include <stdbool.h>

/** @file tetromino.h
 *  @brief Tetromino-Definitionen, Rotation und Kollisionserkennung
 */

/** Anzahl der verschiedenen Tetromino-Typen */
#define NUM_TETROMINO_TYPES 7

/** Größe der Tetromino-Matrix (4x4) */
#define TETROMINO_SIZE 4

/** Anzahl der Rotationen pro Tetromino */
#define NUM_ROTATIONS 4

/** Board-Breite */
#define TETROMINO_BOARD_WIDTH 10

/** Board-Höhe */
#define TETROMINO_BOARD_HEIGHT 20

/** Farbpaare für ncurses - jedes Tetromino hat seine eigene Farbe */
typedef enum {
    COLOR_I = 1,  /**< Cyan */
    COLOR_O = 2,  /**< Gelb */
    COLOR_T = 3,  /**< Lila */
    COLOR_S = 4,  /**< Grün */
    COLOR_Z = 5,  /**< Rot */
    COLOR_J = 6,  /**< Blau */
    COLOR_L = 7,  /**< Orange */
    COLOR_EMPTY = 8
} TetrominoColor;

/** Tetromino-Typen */
typedef enum {
    TETROMINO_I,
    TETROMINO_O,
    TETROMINO_T,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_J,
    TETROMINO_L
} TetrominoType;

/** Struktur für ein Tetromino */
typedef struct {
    TetrominoType type;
    int rotation;
    int x;
    int y;
} Tetromino;

/** 
 * @brief Gibt das Tetromino-Array für einen bestimmten Typ und Rotation zurück
 * @param type Der Tetromino-Typ
 * @param rotation Die Rotation (0-3)
 * @return Pointer auf das 4x4 Array
 */
const int* tetromino_get_shape(TetrominoType type, int rotation);

/**
 * @brief Gibt die Farbe für einen Tetromino-Typ zurück
 * @param type Der Tetromino-Typ
 * @return Die Farbnummer für ncurses
 */
TetrominoColor tetromino_get_color(TetrominoType type);

/**
 * @brief Initialisiert ein neues Tetromino
 * @param type Der Tetromino-Typ
 * @param start_x Start-X-Position
 * @param start_y Start-Y-Position
 * @return Das initialisierte Tetromino
 */
Tetromino tetromino_create(TetrominoType type, int start_x, int start_y);

/**
 * @brief Rotiert ein Tetromino im Uhrzeigersinn
 * @param t Pointer auf das Tetromino
 */
void tetromino_rotate_clockwise(Tetromino *t);

/**
 * @brief Rotiert ein Tetromino gegen den Uhrzeigersinn
 * @param t Pointer auf das Tetromino
 */
void tetromino_rotate_counter_clockwise(Tetromino *t);

/**
 * @brief Bewegt ein Tetromino nach links
 * @param t Pointer auf das Tetromino
 */
void tetromino_move_left(Tetromino *t);

/**
 * @brief Bewegt ein Tetromino nach rechts
 * @param t Pointer auf das Tetromino
 */
void tetromino_move_right(Tetromino *t);

/**
 * @brief Bewegt ein Tetromino nach unten
 * @param t Pointer auf das Tetromino
 */
void tetromino_move_down(Tetromino *t);

/**
 * @brief Prüft ob ein Tetromino mit dem Spielfeld kollidiert
 * @param t Das Tetromino
 * @param board Das Spielfeld-Array (10x20) - nicht const wegen C-Kompatibilität
 * @return true bei Kollision, false sonst
 */
bool tetromino_check_collision(const Tetromino *t, int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH]);

/**
 * @brief Prüft ob eine spezifische Position gültig ist
 * @param type Der Tetromino-Typ
 * @param rotation Die Rotation
 * @param board_x X-Position im Board
 * @param board_y Y-Position im Board
 * @param board Das Spielfeld
 * @return true wenn Position gültig, false sonst
 */
bool tetromino_check_position(TetrominoType type, int rotation, 
                               int board_x, int board_y, 
                               int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH]);

#endif /* TETROMINO_H */
