#include "input.h"

#include <ncurses.h>
#include <stdlib.h>

void input_init(void) {
    /* ncurses ist bereits in renderer_init initialisiert */
    /* Hier könnten weitere Eingabe-spezifische Einstellungen folgen */
}

bool input_process(Game *game) {
    if (game == NULL) return false;
    
    if (game->state == STATE_GAME_OVER) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            game->running = false;
            return false;
        }
        return true;
    }
    
    int ch = getch();
    
    if (ch == ERR) {
        return true;  /* Keine Taste gedrückt */
    }
    
    switch (ch) {
        case 'q':
        case 'Q':
            input_handle_quit(game);
            return false;
            
        case 'p':
        case 'P':
            input_handle_pause(game);
            break;
            
        case KEY_LEFT:
            input_handle_left(game);
            break;
            
        case KEY_RIGHT:
            input_handle_right(game);
            break;
            
        case KEY_DOWN:
            input_handle_down(game);
            break;
            
        case KEY_UP:
        case ' ':
            input_handle_rotate(game, true);
            break;
            
        case 'z':
        case 'Z':
            input_handle_rotate(game, false);
            break;
            
        case 'x':
        case 'X':
        case '\n':
        case KEY_ENTER:
            input_handle_hard_drop(game);
            break;
    }
    
    return true;
}

bool input_has_key(void) {
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return true;
    }
    return false;
}

int input_get_key(void) {
    return getch();
}

void input_handle_pause(Game *game) {
    if (game == NULL) return;
    game_toggle_pause(game);
}

void input_handle_left(Game *game) {
    if (game == NULL) return;
    if (game->state == STATE_PLAYING) {
        game_move_piece(game, -1, 0);
    }
}

void input_handle_right(Game *game) {
    if (game == NULL) return;
    if (game->state == STATE_PLAYING) {
        game_move_piece(game, 1, 0);
    }
}

void input_handle_down(Game *game) {
    if (game == NULL) return;
    if (game->state == STATE_PLAYING) {
        game_move_piece(game, 0, 1);
        /* Zusatzpunkte für Soft Drop */
        game->score += 1;
    }
}

void input_handle_rotate(Game *game, bool clockwise) {
    if (game == NULL) return;
    if (game->state == STATE_PLAYING) {
        game_rotate_piece(game, clockwise);
    }
}

void input_handle_hard_drop(Game *game) {
    if (game == NULL) return;
    if (game->state == STATE_PLAYING) {
        game_hard_drop(game);
        /* Piece sofort locken */
        game_lock_piece(game);
        
        /* Zeilen löschen */
        int cleared = game_clear_lines(game);
        if (cleared > 0) {
            game->score += game_calculate_score(cleared);
            game->lines_cleared += cleared;
            game_update_level(game);
        }
        
        /* Neues Piece spawnen */
        if (!game_spawn_piece(game)) {
            game->state = STATE_GAME_OVER;
        }
    }
}

void input_handle_quit(Game *game) {
    if (game == NULL) return;
    game->running = false;
}
