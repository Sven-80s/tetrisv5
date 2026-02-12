/**
 * @file game.h
 * @brief Game Engine module for CLI Tetris
 * 
 * This module manages the complete game state including the board,
 * current and next tetrominos, scoring, levels, and line clearing.
 * 
 * @author Tetris CLI Project
 * @version 1.0
 */

#ifndef GAME_H
#define GAME_H

#include "tetromino.h"

/**
 * @brief Board width in cells (standard Tetris width)
 */
#define BOARD_WIDTH 10

/**
 * @brief Board height in cells (standard Tetris height)
 */
#define BOARD_HEIGHT 20

/**
 * @brief Cell state type: 0=empty, 1-7=filled with tetromino color
 */
typedef int Cell;

/**
 * @brief Game board structure
 * 
 * The board is a 2D array of cells. Each cell is either empty (0)
 * or contains a color value (1-7) representing a locked tetromino.
 */
typedef struct {
    Cell cells[BOARD_HEIGHT][BOARD_WIDTH];
} Board;

/**
 * @brief Complete game state structure
 * 
 * Contains all information about the current game state including
 * the board, active and next tetrominos, score, level, and game status.
 */
typedef struct {
    Board board;               /**< Spielbrett */
    Tetromino current;         /**< Aktuelles Tetromino */
    Tetromino next;            /**< Nächstes Tetromino */
    int score;                 /**< Aktueller Punktestand */
    int level;                 /**< Aktuelles Level (1+) */
    int lines;                 /**< Gesamt gelöschte Linien */
    int is_running;            /**< 1=läuft, 0=Game Over */
    int is_paused;             /**< 1=pausiert, 0=aktiv */
} GameState;

/**
 * @brief Initializes a new game state
 * 
 * Sets up a fresh game with empty board, random current and next pieces,
 * score=0, level=1, lines=0, and game running but not paused.
 * 
 * @param game Pointer to GameState to initialize
 */
void game_init(GameState *game);

/**
 * @brief Resets the game state to initial values
 * 
 * Similar to game_init but preserves the game structure.
 * Clears the board, resets score/level/lines, and generates new pieces.
 * 
 * @param game Pointer to GameState to reset
 */
void game_reset(GameState *game);

/**
 * @brief Spawns a new piece at the top of the board
 * 
 * Places the tetromino at the standard starting position (centered, top).
 * Updates game->current with the new piece.
 * 
 * @param game Pointer to GameState
 * @param type Type of tetromino to spawn
 * @return 1 if spawn successful, 0 if blocked (Game Over condition)
 */
int game_spawn_piece(GameState *game, TetrominoType type);

/**
 * @brief Moves the current piece by the specified delta
 * 
 * Attempts to move the current tetromino by dx, dy cells.
 * Validates the new position before moving.
 * 
 * @param game Pointer to GameState
 * @param dx Horizontal movement (positive=right, negative=left)
 * @param dy Vertical movement (positive=down, negative=up)
 * @return 1 if move successful, 0 if blocked by wall or other pieces
 */
int game_move_current(GameState *game, int dx, int dy);

/**
 * @brief Rotates the current piece
 * 
 * Attempts to rotate the current tetromino clockwise or counter-clockwise.
 * Validates the new position before applying rotation.
 * 
 * @param game Pointer to GameState
 * @param clockwise 1 for clockwise rotation, 0 for counter-clockwise
 * @return 1 if rotation successful, 0 if blocked (no wall-kick for MVP)
 */
int game_rotate_current(GameState *game, int clockwise);

/**
 * @brief Performs a hard drop of the current piece
 * 
 * Moves the tetromino down as far as possible and immediately locks it.
 * This is the "instant drop" feature common in Tetris games.
 * 
 * @param game Pointer to GameState
 * @return Number of cells the piece fell (0 if already at bottom)
 */
int game_hard_drop(GameState *game);

/**
 * @brief Locks the current piece into the board
 * 
 * Transfers the current tetromino's cells to the board with its color,
 * then spawns the next piece and generates a new "next" piece.
 * Automatically clears completed lines and updates score.
 * 
 * @param game Pointer to GameState
 * @return Number of lines cleared after locking (0-4)
 */
int game_lock_piece(GameState *game);

/**
 * @brief Clears completed lines from the board
 * 
 * Scans the board for full lines, removes them, shifts lines down,
 * and updates the score and line count accordingly.
 * 
 * @param game Pointer to GameState
 * @return Number of lines cleared (0-4)
 */
int game_clear_lines(GameState *game);

/**
 * @brief Calculates score for cleared lines
 * 
 * Implements standard Tetris scoring:
 * - 1 line:  100 × level
 * - 2 lines: 300 × level
 * - 3 lines: 500 × level
 * - 4 lines: 800 × level (Tetris)
 * 
 * @param lines_cleared Number of lines cleared (1-4)
 * @param level Current level (1+)
 * @return Points earned for this clear
 */
int game_calculate_score(int lines_cleared, int level);

/**
 * @brief Validates if a tetromino position is valid
 * 
 * Checks if the tetromino is within board boundaries and not
 * colliding with any filled cells on the board.
 * Only considers cells with value 1 in the tetromino's shape matrix.
 * 
 * @param game Pointer to GameState
 * @param t Pointer to Tetromino to validate
 * @return 1 if position is valid, 0 if collision or out of bounds
 */
int game_is_valid_position(const GameState *game, const Tetromino *t);

/**
 * @brief Calculates fall speed in milliseconds for a given level
 * 
 * Speed increases by 100ms per level, starting at 1000ms for level 1.
 * Minimum speed is capped at 100ms.
 * Formula: max(100, 1000 - (level-1) * 100)
 * 
 * @param level Current level (1+)
 * @return Fall speed in milliseconds
 */
int game_get_speed_ms(int level);

/**
 * @brief Checks if the game is over
 * 
 * Game Over occurs when a new piece cannot be spawned at the
 * starting position because it's blocked by existing pieces.
 * 
 * @param game Pointer to GameState
 * @return 1 if game over, 0 if game can continue
 */
int game_check_game_over(const GameState *game);

/**
 * @brief Gets the type of the next piece
 * 
 * @param game Pointer to GameState
 * @return Type of the next tetromino (0-6)
 */
TetrominoType game_get_next_type(const GameState *game);

/**
 * @brief Sets the next piece type (for testing or custom games)
 * 
 * @param game Pointer to GameState
 * @param type Type of tetromino to set as next
 */
void game_set_next_type(GameState *game, TetrominoType type);

#endif /* GAME_H */
