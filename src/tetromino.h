/**
 * @file tetromino.h
 * @brief Tetromino definitions and operations for CLI Tetris
 * 
 * This module defines the 7 standard Tetromino types (I, O, T, S, Z, J, L),
 * their rotation states, and operations for manipulation.
 * 
 * @author Tetris CLI Project
 * @version 1.0
 */

#ifndef TETROMINO_H
#define TETROMINO_H

#include <stddef.h>

/**
 * @brief Color pair constants for ncurses
 */
#define COLOR_I 1   /**< Cyan */
#define COLOR_O 2   /**< Yellow */
#define COLOR_T 3   /**< Magenta */
#define COLOR_S 4   /**< Green */
#define COLOR_Z 5   /**< Red */
#define COLOR_J 6   /**< Blue */
#define COLOR_L 7   /**< White */

/**
 * @brief Tetromino type enumeration
 */
typedef enum {
    TETRO_I,     /**< I-Tetromino (4x1 line) */
    TETRO_O,     /**< O-Tetromino (2x2 square) */
    TETRO_T,     /**< T-Tetromino */
    TETRO_S,     /**< S-Tetromino */
    TETRO_Z,     /**< Z-Tetromino */
    TETRO_J,     /**< J-Tetromino */
    TETRO_L,     /**< L-Tetromino */
    TETRO_COUNT  /**< Number of tetromino types */
} TetrominoType;

/**
 * @brief Maximum rotation states (0-3 for 0°, 90°, 180°, 270°)
 */
#define ROTATION_COUNT 4

/**
 * @brief Matrix dimension for tetromino shapes
 */
#define TETRO_MATRIX_SIZE 4

/**
 * @brief Starting X position (centered on 10-wide board)
 */
#define TETRO_START_X 3

/**
 * @brief Starting Y position (top of board)
 */
#define TETRO_START_Y 0

/**
 * @brief Tetromino structure representing current state
 */
typedef struct {
    TetrominoType type;     /**< Type of tetromino */
    int x;                  /**< X position on board (leftmost) */
    int y;                  /**< Y position on board (topmost) */
    int rotation;           /**< Current rotation (0-3) */
} Tetromino;

/**
 * @brief Creates a new tetromino at starting position
 * 
 * The tetromino is placed at x=3, y=0 (centered on 10-wide board)
 * with rotation=0.
 * 
 * @param type The tetromino type
 * @return Tetromino initialized with given type and default position
 */
Tetromino tetromino_create(TetrominoType type);

/**
 * @brief Rotates a rotation value clockwise
 * 
 * @param rotation Current rotation (0-3)
 * @return New rotation value (0-3) with wraparound
 */
int tetromino_rotate_clockwise(int rotation);

/**
 * @brief Rotates a rotation value counter-clockwise
 * 
 * @param rotation Current rotation (0-3)
 * @return New rotation value (0-3) with wraparound
 */
int tetromino_rotate_counter_clockwise(int rotation);

/**
 * @brief Gets the shape matrix for a tetromino type and rotation
 * 
 * Returns a pointer to a 4x4 integer matrix where 1 indicates
 * an occupied cell and 0 indicates empty.
 * 
 * @param type Tetromino type
 * @param rotation Rotation state (0-3)
 * @return Pointer to 4x4 matrix, or NULL if invalid parameters
 */
const int (*tetromino_get_shape(TetrominoType type, int rotation))[TETRO_MATRIX_SIZE];

/**
 * @brief Gets the ncurses color pair for a tetromino type
 * 
 * @param type Tetromino type
 * @return Color pair number (1-7), or -1 if invalid type
 */
int tetromino_get_color(TetrominoType type);

/**
 * @brief Moves a tetromino by delta values
 * 
 * Updates the x and y position of the tetromino.
 * Does not check for collisions - this must be done by caller.
 * 
 * @param t Pointer to tetromino to move
 * @param dx Delta X (positive=right, negative=left)
 * @param dy Delta Y (positive=down, negative=up)
 */
void tetromino_move(Tetromino *t, int dx, int dy);

/**
 * @brief Validates a tetromino type
 * 
 * @param type Value to validate
 * @return 1 if valid, 0 if invalid
 */
int tetromino_type_is_valid(TetrominoType type);

/**
 * @brief Validates a rotation value
 * 
 * @param rotation Value to validate
 * @return 1 if valid (0-3), 0 if invalid
 */
int tetromino_rotation_is_valid(int rotation);

#endif /* TETROMINO_H */
