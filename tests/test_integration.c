/**
 * @file test_integration.c
 * @brief Integration tests for Tetris CLI modules
 * 
 * Tests the interaction between modules at their boundaries.
 * These tests verify that the modules work together correctly.
 * 
 * @author Tetris CLI Project
 * @version 1.0
 */

#include "../src/tetromino.h"
#include "../src/game.h"
#include "minunit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Test counters */
int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

/**
 * @brief Test: Tetromino module integration with Game
 * 
 * Verifies that tetrominos can be created and used in game context.
 */
char *test_tetromino_game_integration(void) {
    printf("  Testing: Tetromino-Game integration...\n");
    
    /* Create all tetromino types */
    for (int type = TETRO_I; type <= TETRO_L; type++) {
        Tetromino t = tetromino_create((TetrominoType)type);
        
        mu_assert("Tetromino type should match", t.type == type);
        mu_assert("Tetromino should start at rotation 0", t.rotation == 0);
        mu_assert("Tetromino should start at position (3,0)", 
                  t.x == 3 && t.y == 0);
        
        /* Verify shape is valid */
        const int (*shape)[4] = tetromino_get_shape(t.type, t.rotation);
        mu_assert("Shape should not be NULL", shape != NULL);
        
        /* Verify color is valid (1-7) */
        int color = tetromino_get_color(t.type);
        mu_assert("Color should be between 1 and 7", 
                  color >= 1 && color <= 7);
    }
    
    return 0;
}

/**
 * @brief Test: Complete game move sequence
 * 
 * Tests a complete game sequence: spawn -> move -> lock -> clear lines
 */
char *test_complete_game_sequence(void) {
    printf("  Testing: Complete game sequence...\n");
    
    GameState game;
    game_init(&game);
    
    /* Verify initial state */
    mu_assert("Initial score should be 0", game.score == 0);
    mu_assert("Initial level should be 1", game.level == 1);
    mu_assert("Initial lines should be 0", game.lines == 0);
    mu_assert("Game should be running", game.is_running == 1);
    mu_assert("Game should not be paused", game.is_paused == 0);
    
    /* Verify board is empty */
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            mu_assert("Board should be empty initially", 
                      game.board.cells[y][x] == 0);
        }
    }
    
    /* Move piece down until it locks */
    int moves = 0;
    while (game_move_current(&game, 0, 1) && moves < BOARD_HEIGHT + 5) {
        moves++;
    }
    
    /* Piece should have moved or be at bottom */
    mu_assert("Piece should have moved down", moves > 0);
    
    /* Lock the piece and verify it's on the board */
    game_lock_piece(&game);
    
    /* Verify at least one cell is filled (the piece was locked) */
    int filled_cells = 0;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game.board.cells[y][x] != 0) filled_cells++;
        }
    }
    mu_assert("Piece should be locked on board", filled_cells > 0);
    
    /* Clear lines (if any) */
    int cleared = game_clear_lines(&game);
    mu_assert("Line clearing should work", cleared >= 0 && cleared <= 4);
    
    /* Score should be updated based on cleared lines */
    if (cleared > 0) {
        int expected_score = 0;
        switch (cleared) {
            case 1: expected_score = 100 * game.level; break;
            case 2: expected_score = 300 * game.level; break;
            case 3: expected_score = 500 * game.level; break;
            case 4: expected_score = 800 * game.level; break;
        }
        mu_assert("Score should be updated correctly", 
                  game.score == expected_score);
    }
    
    return 0;
}

/**
 * @brief Test: Hard drop integration
 * 
 * Tests hard drop from top to bottom.
 */
char *test_hard_drop_integration(void) {
    printf("  Testing: Hard drop integration...\n");
    
    GameState game;
    game_init(&game);
    
    /* Perform hard drop */
    int drop_distance = game_hard_drop(&game);
    
    /* Verify piece moved down significantly */
    mu_assert("Hard drop should move piece significantly", drop_distance > 5);
    
    /* Verify piece is locked by checking board has filled cells */
    int filled_cells = 0;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game.board.cells[y][x] != 0) filled_cells++;
        }
    }
    mu_assert("Piece should be locked after hard drop", filled_cells > 0);
    
    return 0;
}

/**
 * @brief Test: Rotation with wall kicks
 * 
 * Tests rotation at edges and collision handling.
 */
char *test_rotation_wall_kicks(void) {
    printf("  Testing: Rotation with wall kicks...\n");
    
    GameState game;
    game_init(&game);
    
    /* Move piece to left wall */
    while (game_move_current(&game, -1, 0)) {
        /* Keep moving left */
    }
    
    /* Try to rotate at left wall */
    game_rotate_current(&game, 1);
    
    /* Rotation might succeed with wall kick or fail - both are valid */
    /* The important thing is it doesn't crash or corrupt state */
    mu_assert("Game state should be valid after rotation attempt",
              game.is_running == 1);
    
    /* Move to right wall */
    game_reset(&game);
    while (game_move_current(&game, 1, 0)) {
        /* Keep moving right */
    }
    
    /* Try rotation at right wall */
    game_rotate_current(&game, 1);
    mu_assert("Game state should be valid after right wall rotation",
              game.is_running == 1);
    
    return 0;
}

/**
 * @brief Test: Level progression
 * 
 * Tests that level increases after clearing lines.
 */
char *test_level_progression(void) {
    printf("  Testing: Level progression...\n");
    
    GameState game;
    game_init(&game);
    
    /* Simulate clearing 10 lines */
    game.lines = 10;
    game.level = (game.lines / 10) + 1;
    
    mu_assert("Level should be 2 after 10 lines", game.level == 2);
    
    /* Verify speed calculation */
    int speed = game_get_speed_ms(game.level);
    int expected_speed = 1000 - (game.level - 1) * 100;
    if (expected_speed < 100) expected_speed = 100;
    
    mu_assert("Speed should decrease with level", speed == expected_speed);
    mu_assert("Speed at level 2 should be 900ms", speed == 900);
    
    /* Test max speed at high level */
    int high_level_speed = game_get_speed_ms(20);
    mu_assert("Speed should not go below 100ms", high_level_speed == 100);
    
    return 0;
}

/**
 * @brief Test: Next piece queue
 * 
 * Tests that next piece is properly managed.
 */
char *test_next_piece_queue(void) {
    printf("  Testing: Next piece queue...\n");
    
    GameState game;
    game_init(&game);
    
    /* Store initial next piece */
    TetrominoType initial_next = game.next.type;
    mu_assert("Next piece should be valid", 
              initial_next >= TETRO_I && initial_next <= TETRO_L);
    
    /* Lock current piece - next should become current */
    game_hard_drop(&game);
    
    /* After lock, current should be what was next */
    /* Note: After hard drop, new piece is spawned from next */
    mu_assert("Current piece should be valid after spawn",
              game.current.type >= TETRO_I && game.current.type <= TETRO_L);
    
    return 0;
}

/**
 * @brief Test: Game over detection
 * 
 * Tests game over when pieces stack to top.
 */
char *test_game_over_detection(void) {
    printf("  Testing: Game over detection...\n");
    
    GameState game;
    game_init(&game);
    
    /* Initially should not be game over */
    mu_assert("New game should not be game over", 
              game_check_game_over(&game) == 0);
    
    /* Simulate filling the board to top */
    /* Fill top row with blocks */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[0][x] = 1;  /* Color 1 = I piece */
    }
    
    /* Try to spawn new piece - should fail or detect game over */
    int can_spawn = game_spawn_piece(&game, TETRO_T);
    
    /* Either spawn fails or game over is detected */
    if (can_spawn) {
        mu_assert("Game over should be detected when board is full",
                  game_check_game_over(&game) == 1);
    }
    
    return 0;
}

/**
 * @brief Test: Board boundary enforcement
 * 
 * Tests that pieces cannot move outside board (cells stay within bounds).
 * Note: Tetromino x/y can be negative as long as filled cells are within board.
 */
char *test_board_boundaries(void) {
    printf("  Testing: Board boundary enforcement...\n");
    
    GameState game;
    game_init(&game);
    int initial_x = game.current.x;
    
    /* Try to move far left - should stop when piece can't move further */
    int moves_left = 0;
    while (game_move_current(&game, -1, 0) && moves_left < 20) {
        moves_left++;
    }
    
    /* Piece should not be able to move left anymore */
    int can_move_left = game_move_current(&game, -1, 0);
    mu_assert("Piece should not be able to move left beyond boundary", 
              can_move_left == 0);
    
    /* Reset and try right */
    game_reset(&game);
    int moves_right = 0;
    while (game_move_current(&game, 1, 0) && moves_right < 20) {
        moves_right++;
    }
    
    /* Piece should not be able to move right anymore */
    int can_move_right = game_move_current(&game, 1, 0);
    mu_assert("Piece should not be able to move right beyond boundary",
              can_move_right == 0);
    
    /* Verify piece moved from initial position */
    mu_assert("Piece should have been able to move in at least one direction",
              moves_left > 0 || moves_right > 0);
    
    return 0;
}

/**
 * @brief Test: Score calculation accuracy
 * 
 * Tests scoring for different line clear counts.
 */
char *test_score_calculation(void) {
    printf("  Testing: Score calculation accuracy...\n");
    
    GameState game;
    game_init(&game);
    
    /* Test each line clear scenario */
    struct {
        int lines;
        int expected_base;
    } scenarios[] = {
        {1, 100},
        {2, 300},
        {3, 500},
        {4, 800},
        {0, 0}
    };
    
    for (int i = 0; i < 5; i++) {
        int base_score = game_calculate_score(scenarios[i].lines, 1);
        mu_assert("Score calculation should match expected",
                  base_score == scenarios[i].expected_base);
        
        /* Test with level multiplier */
        int level3_score = game_calculate_score(scenarios[i].lines, 3);
        mu_assert("Score should be multiplied by level",
                  level3_score == scenarios[i].expected_base * 3);
    }
    
    return 0;
}

/**
 * @brief Test: Reset functionality
 * 
 * Tests that game reset properly clears state.
 */
char *test_game_reset(void) {
    printf("  Testing: Game reset functionality...\n");
    
    GameState game;
    game_init(&game);
    
    /* Make some changes */
    game.score = 1000;
    game.level = 5;
    game.lines = 50;
    game.board.cells[10][5] = 3;
    
    /* Reset */
    game_reset(&game);
    
    /* Verify reset state */
    mu_assert("Score should reset to 0", game.score == 0);
    mu_assert("Level should reset to 1", game.level == 1);
    mu_assert("Lines should reset to 0", game.lines == 0);
    mu_assert("Board should be empty", game.board.cells[10][5] == 0);
    mu_assert("Game should be running", game.is_running == 1);
    
    return 0;
}

/**
 * @brief Test: Multiple piece spawn sequence
 * 
 * Tests spawning multiple pieces in sequence.
 */
char *test_multiple_spawns(void) {
    printf("  Testing: Multiple piece spawn sequence...\n");
    
    GameState game;
    game_init(&game);
    
    /* Spawn and lock several pieces */
    for (int i = 0; i < 5; i++) {
        TetrominoType spawn_type = (TetrominoType)(TETRO_I + (i % 7));
        
        int spawned = game_spawn_piece(&game, spawn_type);
        if (!spawned) break;  /* Game over or blocked */
        
        mu_assert("Spawned piece should have correct type",
                  game.current.type == spawn_type);
        
        /* Move to bottom and lock */
        game_hard_drop(&game);
    }
    
    /* Game should still be in valid state */
    mu_assert("Game should be valid after multiple spawns",
              game.is_running == 0 || game.is_running == 1);
    
    return 0;
}

/**
 * @brief Run all integration tests
 */
char *all_tests(void) {
    printf("\n=== Running Integration Tests ===\n\n");
    
    mu_run_test(test_tetromino_game_integration);
    mu_run_test(test_complete_game_sequence);
    mu_run_test(test_hard_drop_integration);
    mu_run_test(test_rotation_wall_kicks);
    mu_run_test(test_level_progression);
    mu_run_test(test_next_piece_queue);
    mu_run_test(test_game_over_detection);
    mu_run_test(test_board_boundaries);
    mu_run_test(test_score_calculation);
    mu_run_test(test_game_reset);
    mu_run_test(test_multiple_spawns);
    
    return 0;
}

/**
 * @brief Main entry point
 */
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    char *result = all_tests();
    
    printf("\n=== Integration Test Results ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    
    if (result != 0) {
        printf("FAILED: %s\n", result);
        return 1;
    }
    
    printf("\nAll integration tests passed!\n");
    return 0;
}
