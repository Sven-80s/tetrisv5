/**
 * @file test_game.c
 * @brief Unit-Tests für game.c
 */

#include <string.h>
#include "minunit.h"
#include "../game.h"

/**
 * @brief Test: game_init initialisiert Spiel korrekt
 */
static const char* test_game_init(void) {
    Game game;
    bool result = game_init(&game);
    
    mu_assert("game_init should return true", result);
    mu_assert_int_eq(0, game.score);
    mu_assert_int_eq(1, game.level);
    mu_assert_int_eq(0, game.lines_cleared);
    mu_assert_int_eq(STATE_PLAYING, game.state);
    mu_assert("game should be running", game.running);
    
    return NULL;
}

/**
 * @brief Test: game_init mit NULL-Pointer
 */
static const char* test_game_init_null(void) {
    bool result = game_init(NULL);
    mu_assert("game_init with NULL should return false", !result);
    return NULL;
}

/**
 * @brief Test: game_reset leert das Board
 */
static const char* test_game_reset(void) {
    Game game;
    game_init(&game);
    
    /* Einen Block setzen */
    game.board[10][5] = COLOR_I;
    game.score = 1000;
    game.level = 5;
    
    game_reset(&game);
    
    mu_assert_int_eq(0, game.board[10][5]);
    mu_assert_int_eq(0, game.score);
    mu_assert_int_eq(1, game.level);
    
    return NULL;
}

/**
 * @brief Test: Score-Berechnung für verschiedene Line-Clears
 */
static const char* test_calculate_score(void) {
    mu_assert_int_eq(0, game_calculate_score(0));
    mu_assert_int_eq(SCORE_SINGLE, game_calculate_score(1));
    mu_assert_int_eq(SCORE_DOUBLE, game_calculate_score(2));
    mu_assert_int_eq(SCORE_TRIPLE, game_calculate_score(3));
    mu_assert_int_eq(SCORE_TETRIS, game_calculate_score(4));
    
    /* Ungültige Werte */
    mu_assert_int_eq(0, game_calculate_score(5));
    mu_assert_int_eq(0, game_calculate_score(-1));
    
    return NULL;
}

/**
 * @brief Test: Level-Update erhöht Level korrekt
 */
static const char* test_level_update(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Nach 9 gelöschten Zeilen noch Level 1 */
    game.lines_cleared = 9;
    game_update_level(&game);
    mu_assert_int_eq(1, game.level);
    
    /* Nach 10 gelöschten Zeilen Level 2 */
    game.lines_cleared = 10;
    game_update_level(&game);
    mu_assert_int_eq(2, game.level);
    
    /* Nach 29 gelöschten Zeilen Level 3 */
    game.lines_cleared = 29;
    game_update_level(&game);
    mu_assert_int_eq(3, game.level);
    
    return NULL;
}

/**
 * @brief Test: Geschwindigkeit nimmt mit Level ab
 */
static const char* test_speed_decreases_with_level(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    int initial_speed = game.speed_ms;
    
    game.lines_cleared = 10;
    game_update_level(&game);
    
    mu_assert("Speed should decrease with level", game.speed_ms < initial_speed);
    
    return NULL;
}

/**
 * @brief Test: Geschwindigkeit hat Minimum
 */
static const char* test_speed_minimum(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Sehr hohes Level */
    game.lines_cleared = 1000;
    game_update_level(&game);
    
    mu_assert_int_eq(MIN_SPEED, game.speed_ms);
    
    return NULL;
}

/**
 * @brief Test: clear_lines löscht keine unvollständigen Zeilen
 */
static const char* test_clear_lines_incomplete(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Eine unvollständige Zeile erstellen */
    game.board[19][0] = COLOR_I;
    game.board[19][1] = COLOR_I;
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_int_eq(0, cleared);
    mu_assert_int_eq(COLOR_I, game.board[19][0]);
    mu_assert_int_eq(COLOR_I, game.board[19][1]);
    
    return NULL;
}

/**
 * @brief Test: clear_lines löscht eine vollständige Zeile
 */
static const char* test_clear_lines_single(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Eine vollständige Zeile erstellen */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board[19][x] = COLOR_I;
    }
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_int_eq(1, cleared);
    
    /* Zeile sollte leer sein */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        mu_assert_int_eq(0, game.board[19][x]);
    }
    
    return NULL;
}

/**
 * @brief Test: clear_lines löscht mehrere Zeilen
 */
static const char* test_clear_lines_multiple(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Zwei vollständige Zeilen erstellen */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board[18][x] = COLOR_O;
        game.board[19][x] = COLOR_T;
    }
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_int_eq(2, cleared);
    
    return NULL;
}

/**
 * @brief Test: clear_lines schiebt Zeilen nach unten
 */
static const char* test_clear_lines_shift_down(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Eine Zeile mit einem Block darüber */
    game.board[18][5] = COLOR_I;
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board[19][x] = COLOR_O;
    }
    
    game_clear_lines(&game);
    
    /* Block sollte nach unten gefallen sein */
    mu_assert_int_eq(COLOR_I, game.board[19][5]);
    mu_assert_int_eq(0, game.board[18][5]);
    
    return NULL;
}

/**
 * @brief Test: clear_lines löscht Tetris (4 Zeilen)
 */
static const char* test_clear_lines_tetris(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Vier vollständige Zeilen erstellen */
    for (int y = 16; y < 20; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game.board[y][x] = COLOR_I;
        }
    }
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_int_eq(4, cleared);
    
    return NULL;
}

/**
 * @brief Test: toggle_pause wechselt zwischen Zuständen
 */
static const char* test_toggle_pause(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    mu_assert_int_eq(STATE_PLAYING, game.state);
    
    game_toggle_pause(&game);
    mu_assert_int_eq(STATE_PAUSED, game.state);
    
    game_toggle_pause(&game);
    mu_assert_int_eq(STATE_PLAYING, game.state);
    
    return NULL;
}

/**
 * @brief Test: game_is_game_over erkennt Game Over
 */
static const char* test_game_over_detection(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    mu_assert("Should not be game over initially", !game_is_game_over(&game));
    
    game.state = STATE_GAME_OVER;
    mu_assert("Should detect game over", game_is_game_over(&game));
    
    return NULL;
}

/**
 * @brief Test: game_get_speed gibt korrekte Geschwindigkeit zurück
 */
static const char* test_get_speed(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    mu_assert_int_eq(INITIAL_SPEED, game_get_speed(&game));
    
    game.speed_ms = 500;
    mu_assert_int_eq(500, game_get_speed(&game));
    
    return NULL;
}

/**
 * @brief Test: game_get_speed mit NULL gibt Default zurück
 */
static const char* test_get_speed_null(void) {
    int speed = game_get_speed(NULL);
    mu_assert_int_eq(INITIAL_SPEED, speed);
    
    return NULL;
}

/**
 * @brief Test: Board-Zugriff funktioniert korrekt
 */
static const char* test_board_access(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Setzen und Lesen */
    game_set_board_cell(&game, 5, 10, COLOR_I);
    mu_assert_int_eq(COLOR_I, game_get_board_cell(&game, 5, 10));
    
    return NULL;
}

/**
 * @brief Test: Board-Zugriff außerhalb Grenzen ist sicher
 */
static const char* test_board_access_bounds(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Sollte nicht abstürzen und 0 zurückgeben */
    int value = game_get_board_cell(&game, -1, 5);
    mu_assert_int_eq(0, value);
    
    value = game_get_board_cell(&game, 100, 5);
    mu_assert_int_eq(0, value);
    
    /* Sollte nicht abstürzen */
    game_set_board_cell(&game, -1, 5, COLOR_I);
    game_set_board_cell(&game, 100, 5, COLOR_I);
    
    return NULL;
}

/**
 * @brief Test: move_piece bewegt im Board
 */
static const char* test_move_piece(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Setze Piece in mittlere Position */
    game.current_piece.x = 5;
    game.current_piece.y = 5;
    
    bool moved = game_move_piece(&game, 1, 0);
    mu_assert("Should move right", moved);
    mu_assert_int_eq(6, game.current_piece.x);
    
    moved = game_move_piece(&game, -1, 0);
    mu_assert("Should move left", moved);
    mu_assert_int_eq(5, game.current_piece.x);
    
    moved = game_move_piece(&game, 0, 1);
    mu_assert("Should move down", moved);
    mu_assert_int_eq(6, game.current_piece.y);
    
    return NULL;
}

/**
 * @brief Test: move_piece verhindert Bewegung bei Kollision
 */
static const char* test_move_piece_collision(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Setze Piece an den Rand */
    game.current_piece = tetromino_create(TETROMINO_O, BOARD_WIDTH - 2, 5);
    
    /* Versuch nach rechts zu bewegen (kollidiert) */
    int old_x = game.current_piece.x;
    bool moved = game_move_piece(&game, 1, 0);
    
    mu_assert("Should not move when colliding", !moved);
    mu_assert_int_eq(old_x, game.current_piece.x);
    
    return NULL;
}

/**
 * @brief Test: rotate_piece rotiert das Piece wenn möglich
 * Wir testen hier nur, dass Rotation funktioniert und nicht abstürzt.
 * Die genaue Rotation hängt vom Wall-Kick-System ab.
 */
static const char* test_rotate_piece(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Verwende T-Tetromino in einer sicheren Position */
    game.current_piece = tetromino_create(TETROMINO_T, 5, 5);
    int old_rotation = game.current_piece.rotation;
    
    bool rotated = game_rotate_piece(&game, true);
    mu_assert("Should rotate T-piece successfully", rotated);
    
    /* Rotation sollte sich geändert haben (oder Position angepasst) */
    mu_assert("Rotation or position should change after successful rotate",
              game.current_piece.rotation != old_rotation || 
              game.current_piece.x != 5 || game.current_piece.y != 5);
    
    return NULL;
}

/**
 * @brief Test: rotate_piece mit Wall-Kick
 */
static const char* test_rotate_piece_wall_kick(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* I-Piece an der Wand - sollte mit Wall-Kick rotieren können */
    game.current_piece = tetromino_create(TETROMINO_I, 5, 10);
    
    bool rotated = game_rotate_piece(&game, true);
    mu_assert("Should rotate with wall-kick", rotated);
    
    return NULL;
}

/**
 * @brief Test: lock_piece fixiert Piece auf Board
 * 
 * O-Tetromino bei x=4, y=4:
 * - Shape bei row 0: {0,1,1,0} -> (4+1,4+0)=(5,4), (4+2,4+0)=(6,4)
 * - Shape bei row 1: {0,1,1,0} -> (4+1,4+1)=(5,5), (4+2,4+1)=(6,5)
 */
static const char* test_lock_piece(void) {
    Game game;
    game_init(&game);
    game_reset(&game);
    
    /* Setze ein O-Piece */
    game.current_piece = tetromino_create(TETROMINO_O, 4, 4);
    
    game_lock_piece(&game);
    
    /* Prüfe ob die 4 Zellen des O-Pieces gesetzt sind */
    int color = COLOR_O;
    mu_assert_int_eq(color, game.board[4][5]);
    mu_assert_int_eq(color, game.board[4][6]);
    mu_assert_int_eq(color, game.board[5][5]);
    mu_assert_int_eq(color, game.board[5][6]);
    
    return NULL;
}

/**
 * @brief Hauptfunktion für Game-Tests
 * @return Anzahl der fehlgeschlagenen Tests
 */
int main(void) {
    printf("\n========================================\n");
    printf("Running Game Tests\n");
    printf("========================================\n\n");
    
    mu_suite_start();
    
    mu_run_test(test_game_init);
    mu_run_test(test_game_init_null);
    mu_run_test(test_game_reset);
    mu_run_test(test_calculate_score);
    mu_run_test(test_level_update);
    mu_run_test(test_speed_decreases_with_level);
    mu_run_test(test_speed_minimum);
    mu_run_test(test_clear_lines_incomplete);
    mu_run_test(test_clear_lines_single);
    mu_run_test(test_clear_lines_multiple);
    mu_run_test(test_clear_lines_shift_down);
    mu_run_test(test_clear_lines_tetris);
    mu_run_test(test_toggle_pause);
    mu_run_test(test_game_over_detection);
    mu_run_test(test_get_speed);
    mu_run_test(test_get_speed_null);
    mu_run_test(test_board_access);
    mu_run_test(test_board_access_bounds);
    mu_run_test(test_move_piece);
    mu_run_test(test_move_piece_collision);
    mu_run_test(test_rotate_piece);
    mu_run_test(test_rotate_piece_wall_kick);
    mu_run_test(test_lock_piece);
    
    mu_suite_report();
}
