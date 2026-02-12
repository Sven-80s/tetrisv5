#include "game.h"

#include <stdlib.h>
#include <string.h>

bool game_init(Game *game) {
    if (game == NULL) return false;
    
    game_reset(game);
    return true;
}

void game_reset(Game *game) {
    if (game == NULL) return;
    
    /* Board leeren */
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game->board[y][x] = 0;
        }
    }
    
    /* Zufällige Tetrominos spawnen */
    TetrominoType first = (TetrominoType)(rand() % NUM_TETROMINO_TYPES);
    TetrominoType second = (TetrominoType)(rand() % NUM_TETROMINO_TYPES);
    
    game->current_piece = tetromino_create(first, BOARD_WIDTH / 2 - 2, 0);
    game->next_piece = tetromino_create(second, BOARD_WIDTH / 2 - 2, 0);
    
    game->score = 0;
    game->level = 1;
    game->lines_cleared = 0;
    game->speed_ms = INITIAL_SPEED;
    game->state = STATE_PLAYING;
    game->running = true;
}

void game_cleanup(Game *game) {
    if (game == NULL) return;
    game->running = false;
}

bool game_update(Game *game) {
    if (game == NULL || !game->running) return false;
    
    if (game->state != STATE_PLAYING) {
        return true;
    }
    
    /* Versuche nach unten zu bewegen */
    if (!game_move_piece(game, 0, 1)) {
        /* Kollision - Piece fixieren */
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
            return false;
        }
    }
    
    return true;
}

bool game_move_piece(Game *game, int dx, int dy) {
    if (game == NULL) return false;
    
    Tetromino temp = game->current_piece;
    temp.x += dx;
    temp.y += dy;
    
    if (!tetromino_check_collision(&temp, game->board)) {
        game->current_piece = temp;
        return true;
    }
    
    return false;
}

bool game_rotate_piece(Game *game, bool clockwise) {
    if (game == NULL) return false;
    
    Tetromino temp = game->current_piece;
    
    if (clockwise) {
        tetromino_rotate_clockwise(&temp);
    } else {
        tetromino_rotate_counter_clockwise(&temp);
    }
    
    /* Wall-Kick: Versuche verschiedene Positionen wenn Rotation kollidiert */
    int kicks[][2] = {{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {-2, 0}, {2, 0}, {0, -2}};
    
    for (int i = 0; i < 7; i++) {
        Tetromino test = temp;
        test.x += kicks[i][0];
        test.y += kicks[i][1];
        
        if (!tetromino_check_collision(&test, game->board)) {
            game->current_piece = test;
            return true;
        }
    }
    
    return false;
}

int game_hard_drop(Game *game) {
    if (game == NULL) return 0;
    
    int drop_distance = 0;
    
    while (game_move_piece(game, 0, 1)) {
        drop_distance++;
    }
    
    /* Zusatzpunkte für Hard Drop */
    game->score += drop_distance * 2;
    
    return drop_distance;
}

void game_lock_piece(Game *game) {
    if (game == NULL) return;
    
    const int *shape = tetromino_get_shape(game->current_piece.type, 
                                            game->current_piece.rotation);
    if (shape == NULL) return;
    
    int color = tetromino_get_color(game->current_piece.type);
    
    for (int row = 0; row < TETROMINO_SIZE; row++) {
        for (int col = 0; col < TETROMINO_SIZE; col++) {
            if (shape[row * TETROMINO_SIZE + col] != 0) {
                int board_x = game->current_piece.x + col;
                int board_y = game->current_piece.y + row;
                
                if (board_y >= 0 && board_y < BOARD_HEIGHT && 
                    board_x >= 0 && board_x < BOARD_WIDTH) {
                    game->board[board_y][board_x] = color;
                }
            }
        }
    }
}

int game_clear_lines(Game *game) {
    if (game == NULL) return 0;
    
    int lines_cleared = 0;
    int y = BOARD_HEIGHT - 1;
    
    while (y >= 0) {
        bool line_full = true;
        
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x] == 0) {
                line_full = false;
                break;
            }
        }
        
        if (line_full) {
            /* Zeile löschen - alle oberen Zeilen nach unten schieben */
            for (int row = y; row > 0; row--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    game->board[row][x] = game->board[row - 1][x];
                }
            }
            
            /* Obere Zeile leeren */
            for (int x = 0; x < BOARD_WIDTH; x++) {
                game->board[0][x] = 0;
            }
            
            lines_cleared++;
        } else {
            y--;
        }
    }
    
    return lines_cleared;
}

int game_calculate_score(int lines) {
    switch (lines) {
        case 1: return SCORE_SINGLE;
        case 2: return SCORE_DOUBLE;
        case 3: return SCORE_TRIPLE;
        case 4: return SCORE_TETRIS;
        default: return 0;
    }
}

bool game_spawn_piece(Game *game) {
    if (game == NULL) return false;
    
    game->current_piece = game->next_piece;
    game->current_piece.x = BOARD_WIDTH / 2 - 2;
    game->current_piece.y = 0;
    
    /* Neues Next-Piece */
    TetrominoType next_type = (TetrominoType)(rand() % NUM_TETROMINO_TYPES);
    game->next_piece = tetromino_create(next_type, BOARD_WIDTH / 2 - 2, 0);
    
    /* Prüfe ob Spawn-Position gültig */
    if (tetromino_check_collision(&game->current_piece, game->board)) {
        game->state = STATE_GAME_OVER;
        return false;
    }
    
    return true;
}

void game_update_level(Game *game) {
    if (game == NULL) return;
    
    game->level = (game->lines_cleared / LINES_PER_LEVEL) + 1;
    
    /* Geschwindigkeit anpassen */
    game->speed_ms = INITIAL_SPEED - (game->level - 1) * SPEED_DECREMENT;
    if (game->speed_ms < MIN_SPEED) {
        game->speed_ms = MIN_SPEED;
    }
}

void game_toggle_pause(Game *game) {
    if (game == NULL) return;
    
    if (game->state == STATE_PLAYING) {
        game->state = STATE_PAUSED;
    } else if (game->state == STATE_PAUSED) {
        game->state = STATE_PLAYING;
    }
}

bool game_is_game_over(const Game *game) {
    if (game == NULL) return true;
    return game->state == STATE_GAME_OVER;
}

int game_get_speed(const Game *game) {
    if (game == NULL) return INITIAL_SPEED;
    return game->speed_ms;
}

int game_get_board_cell(const Game *game, int x, int y) {
    if (game == NULL) return 0;
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return 0;
    return game->board[y][x];
}

void game_set_board_cell(Game *game, int x, int y, int value) {
    if (game == NULL) return;
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return;
    game->board[y][x] = value;
}
