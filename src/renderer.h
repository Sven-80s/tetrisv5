/**
 * @file renderer.h
 * @brief Rendering module for CLI Tetris using ncurses
 * 
 * This module handles all visual output for the game including
 * the game board, sidebar with next piece and score, and overlays
 * for pause and game over states.
 * 
 * @author Tetris CLI Project
 * @version 1.0
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "tetromino.h"
#include "game.h"

/* Layout constants */
#define BOARD_DISPLAY_X     2       /**< Board start column on screen */
#define BOARD_DISPLAY_Y     1       /**< Board start row on screen */
#define BOARD_CELL_WIDTH    2       /**< Width of each cell in characters */
#define BOARD_WIDTH_CHARS   (BOARD_WIDTH * BOARD_CELL_WIDTH + 2)  /**< Board width + borders */
#define BOARD_HEIGHT_CHARS  (BOARD_HEIGHT + 2)  /**< Board height + borders */
#define SIDEBAR_X           (BOARD_WIDTH_CHARS + 4)  /**< Sidebar start column */
#define SIDEBAR_WIDTH       20      /**< Sidebar width in characters */

/**
 * @brief Initialize the renderer
 * 
 * Sets up ncurses and initializes color pairs for all tetromino types.
 * Must be called before any other renderer functions.
 * 
 * Initializes ncurses:
 * - initscr() - Initialize ncurses mode
 * - start_color() - Enable colors
 * - Color pairs 1-7 for each tetromino type
 * 
 * Color pairs:
 * - Pair 1: Cyan (COLOR_I)
 * - Pair 2: Yellow (COLOR_O)
 * - Pair 3: Magenta (COLOR_T)
 * - Pair 4: Green (COLOR_S)
 * - Pair 5: Red (COLOR_Z)
 * - Pair 6: Blue (COLOR_J)
 * - Pair 7: White (COLOR_L)
 */
void renderer_init(void);

/**
 * @brief Cleanup the renderer
 * 
 * Restores terminal to its original state.
 * Must be called before program exit.
 */
void renderer_cleanup(void);

/**
 * @brief Draw the complete game screen
 * 
 * Renders all game elements: board with current piece, sidebar
 * with next piece, score, level, lines, and controls.
 * 
 * @param game Pointer to the current game state
 * 
 * @note This is the main rendering function - call this every frame
 */
void renderer_draw_game(const GameState *game);

/**
 * @brief Draw the game board
 * 
 * Renders the board with all locked pieces and the current
 * tetromino overlaid on top. Draws a border around the board.
 * 
 * @param board Pointer to the board to draw
 * @param current Pointer to the current tetromino (can be NULL)
 * 
 * @note Each cell is rendered as 2 characters wide
 * @note Empty cells show as spaces with black background
 * @note Filled cells show as "██" with tetromino color
 */
void renderer_draw_board(const Board *board, const Tetromino *current);

/**
 * @brief Draw the sidebar
 * 
 * Renders the complete sidebar including next piece preview,
 * score, level, lines, and controls help.
 * 
 * @param game Pointer to the current game state
 */
void renderer_draw_sidebar(const GameState *game);

/**
 * @brief Draw the next piece preview
 * 
 * Renders a 4x4 box showing the next tetromino that will spawn.
 * The piece is centered within the preview box.
 * 
 * @param next_type The type of the next tetromino
 */
void renderer_draw_next_piece(TetrominoType next_type);

/**
 * @brief Draw the score display
 * 
 * Renders the current score, level, and lines cleared.
 * 
 * @param score Current score value
 * @param level Current level (1+)
 * @param lines Total lines cleared
 */
void renderer_draw_score(int score, int level, int lines);

/**
 * @brief Draw the controls help
 * 
 * Renders a list of available controls and their key bindings.
 */
void renderer_draw_controls(void);

/**
 * @brief Draw the pause overlay
 * 
 * Renders a centered "PAUSED" message on the board area.
 * Call this when the game is paused.
 */
void renderer_draw_pause(void);

/**
 * @brief Draw the game over overlay
 * 
 * Renders a centered "GAME OVER" message with final score.
 * Call this when the game ends.
 * 
 * @param score Final score to display
 */
void renderer_draw_game_over(int score);

#endif /* RENDERER_H */
