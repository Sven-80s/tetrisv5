/**
 * @file input.h
 * @brief Input handling module for CLI Tetris
 * 
 * This module provides non-blocking keyboard input using ncurses.
 * It maps physical keys to logical game actions and handles
 * special keys like arrow keys and function keys.
 * 
 * @author Tetris CLI Project
 * @version 1.0
 */

#ifndef INPUT_H
#define INPUT_H

/**
 * @brief Input action types
 * 
 * Maps physical key presses to logical game actions.
 * Used by the game loop to process player input.
 */
typedef enum {
    INPUT_NONE,           /**< No input available (non-blocking) */
    INPUT_LEFT,           /**< Move piece left (← arrow) */
    INPUT_RIGHT,          /**< Move piece right (→ arrow) */
    INPUT_DOWN,           /**< Soft drop (↓ arrow) */
    INPUT_ROTATE_CW,      /**< Rotate clockwise (↑ arrow) */
    INPUT_ROTATE_CCW,     /**< Rotate counter-clockwise (z/Z key) */
    INPUT_HARD_DROP,      /**< Hard drop (spacebar) */
    INPUT_PAUSE,          /**< Pause game (p/P key) */
    INPUT_QUIT,           /**< Quit game (q/Q key) */
    INPUT_INVALID         /**< Invalid/unknown key */
} InputAction;

/**
 * @brief Initialize the input system
 * 
 * Sets up ncurses for non-blocking input with no echo.
 * Must be called before any other input functions.
 * 
 * Configures:
 * - cbreak mode (disable line buffering)
 * - noecho (don't show typed characters)
 * - nodelay (non-blocking getch)
 * - keypad (enable special keys like arrows)
 * 
 * @warning Requires ncurses to be initialized first
 */
void input_init(void);

/**
 * @brief Cleanup the input system
 * 
 * Restores terminal to its original state.
 * Must be called before program exit to ensure
 * proper terminal restoration.
 */
void input_cleanup(void);

/**
 * @brief Get the next input action
 * 
 * Non-blocking function that checks for pending key presses
 * and returns the corresponding action.
 * 
 * @return InputAction representing the pressed key, or INPUT_NONE if no key pressed
 * 
 * @note This function does not block - returns INPUT_NONE immediately if no input
 * 
 * Key mappings:
 * - Arrow Left  → INPUT_LEFT
 * - Arrow Right → INPUT_RIGHT
 * - Arrow Down  → INPUT_DOWN
 * - Arrow Up    → INPUT_ROTATE_CW
 * - Space       → INPUT_HARD_DROP
 * - z, Z        → INPUT_ROTATE_CCW
 * - p, P        → INPUT_PAUSE
 * - q, Q        → INPUT_QUIT
 */
InputAction input_get_action(void);

/**
 * @brief Check if input is available
 * 
 * Non-blocking check to determine if a key has been pressed.
 * Useful for polling-based game loops.
 * 
 * @return 1 if a key is available to read, 0 otherwise
 * 
 * @note This is a convenience wrapper - input_get_action() also returns
 *       INPUT_NONE when no key is pressed
 */
int input_has_input(void);

#endif /* INPUT_H */
