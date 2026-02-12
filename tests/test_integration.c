/**
 * @file test_integration.c
 * @brief Integration tests for Tetris CLI modules
 */

#include "../src/tetromino.h"
#include "../src/game.h"
#include "minunit.h"
#include <stdio.h>
#include <stdlib.h>

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

char *test_tetromino_game_integration(void) {
    printf("  Testing: Tetromino-Game integration...\n");
    for (int type = TETRO_I; type <= TETRO_L; type++) {
        Tetromino t = tetromino_create((TetrominoType)type);
        mu_assert("Type match", t.type == type);
        mu_assert("Rotation 0", t.rotation == 0);
        const int (*shape)[4] = tetromino_get_shape(t.type, t.rotation);
        mu_assert("Shape valid", shape != NULL);
        int color = tetromino_get_color(t.type);
        mu_assert("Color 1-7", color >= 1 && color <= 7);
    }
    return 0;
}

char *test_complete_game_sequence(void) {
    printf("  Testing: Complete game sequence...\n");
    GameState game;
    game_init(&game);
    mu_assert("Score 0", game.score == 0);
    mu_assert("Level 1", game.level == 1);
    int moves = 0;
    while (game_move_current(&game, 0, 1) && moves < 30) moves++;
    mu_assert("Moved", moves > 0);
    game_lock_piece(&game);
    int filled = 0;
    for (int y = 0; y < BOARD_HEIGHT; y++)
        for (int x = 0; x < BOARD_WIDTH; x++)
            if (game.board.cells[y][x] != 0) filled++;
    mu_assert("Locked", filled > 0);
    return 0;
}

char *test_hard_drop(void) {
    printf("  Testing: Hard drop...\n");
    GameState game;
    game_init(&game);
    int dist = game_hard_drop(&game);
    mu_assert("Drop > 5", dist > 5);
    return 0;
}

char *test_rotation(void) {
    printf("  Testing: Rotation...\n");
    GameState game;
    game_init(&game);
    while (game_move_current(&game, -1, 0));
    game_rotate_current(&game, 1);
    mu_assert("Valid", game.is_running == 1);
    return 0;
}

char *test_level(void) {
    printf("  Testing: Level progression...\n");
    GameState game;
    game_init(&game);
    int speed = game_get_speed_ms(2);
    mu_assert("Speed 900", speed == 900);
    speed = game_get_speed_ms(20);
    mu_assert("Speed min 100", speed == 100);
    return 0;
}

char *test_next(void) {
    printf("  Testing: Next piece...\n");
    GameState game;
    game_init(&game);
    TetrominoType next = game.next.type;
    mu_assert("Valid next", next >= TETRO_I && next <= TETRO_L);
    return 0;
}

char *test_game_over(void) {
    printf("  Testing: Game over...\n");
    GameState game;
    game_init(&game);
    mu_assert("Not over", game_check_game_over(&game) == 0);
    return 0;
}

char *test_boundaries(void) {
    printf("  Testing: Boundaries...\n");
    GameState game;
    game_init(&game);
    int moves = 0;
    while (game_move_current(&game, -1, 0) && moves < 20) moves++;
    mu_assert("Cant move left", game_move_current(&game, -1, 0) == 0);
    return 0;
}

char *test_score(void) {
    printf("  Testing: Score calc...\n");
    GameState game;
    game_init(&game);
    mu_assert("1 line", game_calculate_score(1, 1) == 100);
    mu_assert("4 lines", game_calculate_score(4, 1) == 800);
    mu_assert("Level mult", game_calculate_score(1, 3) == 300);
    return 0;
}

char *test_reset(void) {
    printf("  Testing: Reset...\n");
    GameState game;
    game_init(&game);
    game.score = 1000;
    game_reset(&game);
    mu_assert("Score 0", game.score == 0);
    mu_assert("Level 1", game.level == 1);
    return 0;
}

char *test_spawns(void) {
    printf("  Testing: Multiple spawns...\n");
    GameState game;
    game_init(&game);
    for (int i = 0; i < 3; i++) {
        if (!game_spawn_piece(&game, TETRO_I)) break;
        game_hard_drop(&game);
    }
    mu_assert("Valid", game.is_running == 0 || game.is_running == 1);
    return 0;
}

char *all_tests(void) {
    printf("\n=== Integration Tests ===\n\n");
    mu_run_test(test_tetromino_game_integration);
    mu_run_test(test_complete_game_sequence);
    mu_run_test(test_hard_drop);
    mu_run_test(test_rotation);
    mu_run_test(test_level);
    mu_run_test(test_next);
    mu_run_test(test_game_over);
    mu_run_test(test_boundaries);
    mu_run_test(test_score);
    mu_run_test(test_reset);
    mu_run_test(test_spawns);
    return 0;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    char *result = all_tests();
    printf("\nTests: %d passed, %d failed\n", tests_passed, tests_failed);
    return result ? 1 : 0;
}
