/**
 * @file input.c
 * @brief Input handling module implementation
 */

#include "input.h"

#include <ncurses.h>

/**
 * @brief Internal flag to track initialization state
 */
static int input_initialized = 0;

void input_init(void)
{
    if (input_initialized) {
        return;
    }

    /* Enable cbreak mode - disable line buffering but allow Ctrl-C, etc. */
    cbreak();
    
    /* Don't echo typed characters */
    noecho();
    
    /* Non-blocking input - getch() returns ERR if no key pressed */
    nodelay(stdscr, TRUE);
    
    /* Enable special keys (arrow keys, function keys, etc.) */
    keypad(stdscr, TRUE);
    
    /* Hide cursor */
    curs_set(0);
    
    input_initialized = 1;
}

void input_cleanup(void)
{
    if (!input_initialized) {
        return;
    }

    /* Show cursor again */
    curs_set(1);
    
    input_initialized = 0;
}

InputAction input_get_action(void)
{
    if (!input_initialized) {
        return INPUT_INVALID;
    }

    int ch = getch();
    
    /* No key pressed (non-blocking mode) */
    if (ch == ERR) {
        return INPUT_NONE;
    }

    /* Handle special keys (arrow keys) */
    switch (ch) {
        case KEY_LEFT:
            return INPUT_LEFT;
        case KEY_RIGHT:
            return INPUT_RIGHT;
        case KEY_DOWN:
            return INPUT_DOWN;
        case KEY_UP:
            return INPUT_ROTATE_CW;
        default:
            break;
    }

    /* Handle regular ASCII keys */
    switch (ch) {
        /* Space bar - hard drop */
        case ' ':
            return INPUT_HARD_DROP;
            
        /* z or Z - rotate counter-clockwise */
        case 'z':
        case 'Z':
            return INPUT_ROTATE_CCW;
            
        /* p or P - pause */
        case 'p':
        case 'P':
            return INPUT_PAUSE;
            
        /* q or Q - quit */
        case 'q':
        case 'Q':
            return INPUT_QUIT;
            
        /* Unknown key */
        default:
            return INPUT_INVALID;
    }
}

int input_has_input(void)
{
    if (!input_initialized) {
        return 0;
    }

    int ch = getch();
    
    if (ch == ERR) {
        return 0;
    }
    
    /* Put the character back so input_get_action can read it */
    ungetch(ch);
    return 1;
}
