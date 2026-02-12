/**
 * @file test_game.c
 * @brief Unit tests for the Game Engine module
 */

#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "../src/game.h"

/* Helper: Create a game with deterministic next piece for testing */
static void setup_game_with_next(GameState *game, TetrominoType next_type)
{
    game_init(game);
    game_set_next_type(game, next_type);
}

/* Test: game_init initializes all fields correctly */
mu_test(test_init)
{
    GameState game;
    game_init(&game);
    
    mu_assert_eq_int(0, game.score);
    mu_assert_eq_int(1, game.level);
    mu_assert_eq_int(0, game.lines);
    mu_assert_eq_int(1, game.is_running);
    mu_assert_eq_int(0, game.is_paused);
}

/* Test: game_reset resets all fields */
mu_test(test_reset)
{
    GameState game;
    game_init(&game);
    
    /* Modify some values */
    game.score = 1000;
    game.level = 5;
    game.is_running = 0;
    
    game_reset(&game);
    
    mu_assert_eq_int(0, game.score);
    mu_assert_eq_int(1, game.level);
    mu_assert_eq_int(0, game.lines);
    mu_assert_eq_int(1, game.is_running);
}

/* Test: Board is cleared on init */
mu_test(test_board_cleared)
{
    GameState game;
    game_init(&game);
    
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            mu_assert_eq_int(0, game.board.cells[y][x]);
        }
    }
}

/* Test: game_is_valid_position allows valid position */
mu_test(test_valid_position_basic)
{
    GameState game;
    game_init(&game);
    
    Tetromino t = tetromino_create(TETRO_I);
    mu_assert("I-piece at start should be valid", 
              game_is_valid_position(&game, &t));
}

/* Test: game_is_valid_position detects wall collision (left) */
mu_test(test_invalid_position_left_wall)
{
    GameState game;
    game_init(&game);
    
    Tetromino t = tetromino_create(TETRO_I);
    t.x = -5;  /* Way off the left side */
    
    mu_assert("Piece off left should be invalid", 
              !game_is_valid_position(&game, &t));
}

/* Test: game_is_valid_position detects wall collision (right) */
mu_test(test_invalid_position_right_wall)
{
    GameState game;
    game_init(&game);
    
    Tetromino t = tetromino_create(TETRO_I);
    t.x = BOARD_WIDTH;  /* Off the right side */
    
    mu_assert("Piece off right should be invalid", 
              !game_is_valid_position(&game, &t));
}

/* Test: game_is_valid_position detects floor collision */
mu_test(test_invalid_position_floor)
{
    GameState game;
    game_init(&game);
    
    Tetromino t = tetromino_create(TETRO_I);
    t.y = BOARD_HEIGHT;  /* Below the board */
    
    mu_assert("Piece below board should be invalid", 
              !game_is_valid_position(&game, &t));
}

/* Test: game_is_valid_position detects collision with locked pieces */
mu_test(test_invalid_position_collision)
{
    GameState game;
    game_init(&game);
    
    /* Place a cell in the board */
    game.board.cells[5][5] = COLOR_O;
    
    /* Create piece that would overlap */
    Tetromino t = tetromino_create(TETRO_O);
    t.x = 4;
    t.y = 4;
    
    mu_assert("Piece colliding with board should be invalid", 
              !game_is_valid_position(&game, &t));
}

/* Test: game_move_current successful move */
mu_test(test_move_success)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    int start_x = game.current.x;
    int result = game_move_current(&game, 1, 0);
    
    mu_assert_eq_int(1, result);
    mu_assert_eq_int(start_x + 1, game.current.x);
}

/* Test: game_move_current blocked by left wall (I-piece at left edge) */
mu_test(test_move_blocked_wall)
{
    GameState game;
    setup_game_with_next(&game, TETRO_O);
    game_spawn_piece(&game, TETRO_I);
    
    /* I-piece at x=3, move to left edge (x=0 valid for horizontal I) */
    game.current.x = 0;
    /* Now try to move left - should fail */
    int result = game_move_current(&game, -1, 0);
    
    mu_assert_eq_int(0, result);
}

/* Test: game_move_current blocked by piece */
mu_test(test_move_blocked_piece)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    /* Place a blocking cell to the right of spawn position (x=3) */
    game.board.cells[0][5] = COLOR_Z;
    game.board.cells[1][5] = COLOR_Z;  /* O-piece is 2x2 */
    
    /* Position piece at spawn (x=3) and try to move right */
    int result = game_move_current(&game, 1, 0);
    mu_assert_eq_int(0, result);
}

/* Test: game_rotate_current successful rotation */
mu_test(test_rotate_success)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_I);
    
    int result = game_rotate_current(&game, 1);
    
    mu_assert_eq_int(1, result);
    mu_assert_eq_int(1, game.current.rotation);
}

/* Test: game_rotate_current blocked by locked pieces */
mu_test(test_rotate_blocked)
{
    GameState game;
    setup_game_with_next(&game, TETRO_O);
    game_spawn_piece(&game, TETRO_I);
    
    /* Place blocking pieces in column 2 (where vertical I-piece needs space) */
    game.board.cells[0][5] = COLOR_Z;
    game.board.cells[1][5] = COLOR_Z;
    game.board.cells[2][5] = COLOR_Z;
    game.board.cells[3][5] = COLOR_Z;
    
    int result = game_rotate_current(&game, 1);
    
    /* Should fail because vertical I-piece would collide */
    mu_assert_eq_int(0, result);
    mu_assert_eq_int(0, game.current.rotation);  /* Unchanged */
}

/* Test: game_hard_drop moves to bottom and locks */
mu_test(test_hard_drop)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    int drop = game_hard_drop(&game);
    
    mu_assert("Hard drop should move some distance", drop > 0);
    /* After hard drop, piece should be locked and new piece spawned */
    mu_assert_eq_int(TETRO_I, game.current.type);  /* Next became current */
}

/* Test: game_hard_drop with piece already at bottom */
mu_test(test_hard_drop_immediate)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    /* Place piece right at bottom (O-piece is 2 rows tall) */
    game.current.y = BOARD_HEIGHT - 2;
    
    int drop = game_hard_drop(&game);
    
    /* Should lock immediately with 0 drop distance */
    mu_assert_eq_int(0, drop);
}

/* Test: game_lock_piece places cells on board */
mu_test(test_lock_piece)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    /* Move piece to known position */
    game.current.x = 4;
    game.current.y = 0;
    
    game_lock_piece(&game);
    
    /* O-piece should have placed 4 cells */
    int count = 0;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game.board.cells[y][x] != 0) {
                count++;
            }
        }
    }
    mu_assert_eq_int(4, count);
}

/* Test: game_clear_lines clears full lines */
mu_test(test_clear_single_line)
{
    GameState game;
    game_init(&game);
    
    /* Fill a complete line */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[BOARD_HEIGHT - 1][x] = COLOR_I;
    }
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_eq_int(1, cleared);
    mu_assert_eq_int(1, game.lines);
    mu_assert_eq_int(100, game.score);  /* 100 × level 1 */
}

/* Test: game_clear_lines clears multiple lines */
mu_test(test_clear_double)
{
    GameState game;
    game_init(&game);
    
    /* Fill two complete lines */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[BOARD_HEIGHT - 1][x] = COLOR_I;
        game.board.cells[BOARD_HEIGHT - 2][x] = COLOR_O;
    }
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_eq_int(2, cleared);
    mu_assert_eq_int(2, game.lines);
    mu_assert_eq_int(300, game.score);  /* 300 × level 1 */
}

/* Test: Tetris (4 lines) */
mu_test(test_clear_tetris)
{
    GameState game;
    game_init(&game);
    
    /* Fill four complete lines */
    for (int row = 0; row < 4; row++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game.board.cells[BOARD_HEIGHT - 1 - row][x] = COLOR_I;
        }
    }
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_eq_int(4, cleared);
    mu_assert_eq_int(4, game.lines);
    mu_assert_eq_int(800, game.score);  /* 800 × level 1 */
}

/* Test: game_clear_lines shifts lines down */
mu_test(test_clear_shifts_down)
{
    GameState game;
    game_init(&game);
    
    /* Place a cell above a full line */
    game.board.cells[BOARD_HEIGHT - 3][5] = COLOR_J;
    
    /* Fill the line below it */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[BOARD_HEIGHT - 1][x] = COLOR_I;
    }
    
    game_clear_lines(&game);
    
    /* The J cell should have moved down */
    mu_assert_eq_int(COLOR_J, game.board.cells[BOARD_HEIGHT - 2][5]);
    /* Old position should be empty */
    mu_assert_eq_int(0, game.board.cells[BOARD_HEIGHT - 3][5]);
}

/* Test: game_clear_lines with no full lines */
mu_test(test_clear_no_lines)
{
    GameState game;
    game_init(&game);
    
    /* Place some incomplete lines */
    game.board.cells[BOARD_HEIGHT - 1][0] = COLOR_I;
    game.board.cells[BOARD_HEIGHT - 1][5] = COLOR_O;
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_eq_int(0, cleared);
    mu_assert_eq_int(0, game.lines);
    mu_assert_eq_int(0, game.score);
}

/* Test: Scoring at different levels */
mu_test(test_score_level_2)
{
    GameState game;
    game_init(&game);
    
    /* Set level 2 */
    game.level = 2;
    
    /* Fill a line */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[BOARD_HEIGHT - 1][x] = COLOR_I;
    }
    
    game_clear_lines(&game);
    
    mu_assert_eq_int(200, game.score);  /* 100 × level 2 */
}

/* Test: Level progression */
mu_test(test_level_progression)
{
    GameState game;
    game_init(&game);
    
    /* Simulate clearing 10 lines */
    for (int i = 0; i < 10; i++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game.board.cells[BOARD_HEIGHT - 1][x] = COLOR_I;
        }
        game_clear_lines(&game);
    }
    
    mu_assert_eq_int(10, game.lines);
    mu_assert_eq_int(2, game.level);  /* 10 lines = level 2 */
}

/* Test: game_calculate_score for all line counts */
mu_test(test_calculate_score)
{
    mu_assert_eq_int(100, game_calculate_score(1, 1));
    mu_assert_eq_int(200, game_calculate_score(1, 2));
    mu_assert_eq_int(300, game_calculate_score(2, 1));
    mu_assert_eq_int(600, game_calculate_score(2, 2));
    mu_assert_eq_int(500, game_calculate_score(3, 1));
    mu_assert_eq_int(800, game_calculate_score(4, 1));
    mu_assert_eq_int(1600, game_calculate_score(4, 2));
    mu_assert_eq_int(0, game_calculate_score(0, 1));
    mu_assert_eq_int(0, game_calculate_score(5, 1));
}

/* Test: game_get_speed_ms */
mu_test(test_get_speed)
{
    mu_assert_eq_int(1000, game_get_speed_ms(1));
    mu_assert_eq_int(900, game_get_speed_ms(2));
    mu_assert_eq_int(500, game_get_speed_ms(6));
    mu_assert_eq_int(100, game_get_speed_ms(10));  /* Capped at 100 */
    mu_assert_eq_int(100, game_get_speed_ms(20));  /* Still capped */
    mu_assert_eq_int(1000, game_get_speed_ms(0));  /* Invalid level */
}

/* Test: game_check_game_over when running */
mu_test(test_game_over_false)
{
    GameState game;
    game_init(&game);
    
    mu_assert_eq_int(0, game_check_game_over(&game));
}

/* Test: game_check_game_over when not running */
mu_test(test_game_over_true)
{
    GameState game;
    game_init(&game);
    
    game.is_running = 0;
    
    mu_assert_eq_int(1, game_check_game_over(&game));
}

/* Test: game_check_game_over when spawn blocked */
mu_test(test_game_over_blocked)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    /* Fill the spawn area */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < 2; y++) {
            game.board.cells[y][x] = COLOR_Z;
        }
    }
    
    mu_assert_eq_int(1, game_check_game_over(&game));
}

/* Test: game_spawn_piece */
mu_test(test_spawn_piece)
{
    GameState game;
    game_init(&game);
    
    int result = game_spawn_piece(&game, TETRO_T);
    
    mu_assert_eq_int(1, result);
    mu_assert_eq_int(TETRO_T, game.current.type);
}

/* Test: game_spawn_piece when blocked */
mu_test(test_spawn_blocked)
{
    GameState game;
    game_init(&game);
    
    /* Fill the spawn area */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < 2; y++) {
            game.board.cells[y][x] = COLOR_Z;
        }
    }
    
    int result = game_spawn_piece(&game, TETRO_O);
    
    mu_assert_eq_int(0, result);
    mu_assert_eq_int(0, game.is_running);
}

/* Test: game_get_next_type and game_set_next_type */
mu_test(test_next_type)
{
    GameState game;
    game_init(&game);
    
    game_set_next_type(&game, TETRO_L);
    
    mu_assert_eq_int(TETRO_L, game_get_next_type(&game));
}

/* Test: Move down (gravity) */
mu_test(test_move_down)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    int start_y = game.current.y;
    int result = game_move_current(&game, 0, 1);
    
    mu_assert_eq_int(1, result);
    mu_assert_eq_int(start_y + 1, game.current.y);
}

/* Test: Move down blocked by floor */
mu_test(test_move_down_blocked)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    /* Position at bottom (O-piece is 2 rows tall) */
    game.current.y = BOARD_HEIGHT - 2;
    
    int result = game_move_current(&game, 0, 1);
    
    mu_assert_eq_int(0, result);
}

/* Test: Multiple moves accumulate */
mu_test(test_multiple_moves)
{
    GameState game;
    setup_game_with_next(&game, TETRO_I);
    game_spawn_piece(&game, TETRO_O);
    
    int start_x = game.current.x;
    int start_y = game.current.y;
    
    game_move_current(&game, 1, 0);
    game_move_current(&game, 1, 0);
    game_move_current(&game, 0, 1);
    
    mu_assert_eq_int(start_x + 2, game.current.x);
    mu_assert_eq_int(start_y + 1, game.current.y);
}

/* Test: Complex line clear scenario */
mu_test(test_complex_line_clear)
{
    GameState game;
    game_init(&game);
    
    /* Create a pattern with gaps and full lines */
    /* Full line at bottom */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[BOARD_HEIGHT - 1][x] = COLOR_I;
    }
    /* Partial line above */
    game.board.cells[BOARD_HEIGHT - 2][0] = COLOR_O;
    game.board.cells[BOARD_HEIGHT - 2][9] = COLOR_O;
    /* Another full line */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[BOARD_HEIGHT - 3][x] = COLOR_T;
    }
    
    int cleared = game_clear_lines(&game);
    
    mu_assert_eq_int(2, cleared);
    mu_assert_eq_int(2, game.lines);
    /* Scoring: 1 line (100) + 1 line (100) = 200, but lines cleared in one go
     * The function detects 2 lines, applies 300 score for double line clear */
    mu_assert_eq_int(300, game.score);  /* 300 × level 1 for 2 lines */
    
    /* The partial line should now be at bottom */
    mu_assert_eq_int(COLOR_O, game.board.cells[BOARD_HEIGHT - 1][0]);
    mu_assert_eq_int(COLOR_O, game.board.cells[BOARD_HEIGHT - 1][9]);
}

/* Test suite runner */
static void run_all_tests(void)
{
    printf("\nRunning Game Engine Module Tests...\n");
    printf("===================================\n\n");
    
    mu_run_test(test_init);
    mu_run_test(test_reset);
    mu_run_test(test_board_cleared);
    mu_run_test(test_valid_position_basic);
    mu_run_test(test_invalid_position_left_wall);
    mu_run_test(test_invalid_position_right_wall);
    mu_run_test(test_invalid_position_floor);
    mu_run_test(test_invalid_position_collision);
    mu_run_test(test_move_success);
    mu_run_test(test_move_blocked_wall);
    mu_run_test(test_move_blocked_piece);
    mu_run_test(test_rotate_success);
    mu_run_test(test_rotate_blocked);
    mu_run_test(test_hard_drop);
    mu_run_test(test_hard_drop_immediate);
    mu_run_test(test_lock_piece);
    mu_run_test(test_clear_single_line);
    mu_run_test(test_clear_double);
    mu_run_test(test_clear_tetris);
    mu_run_test(test_clear_shifts_down);
    mu_run_test(test_clear_no_lines);
    mu_run_test(test_score_level_2);
    mu_run_test(test_level_progression);
    mu_run_test(test_calculate_score);
    mu_run_test(test_get_speed);
    mu_run_test(test_game_over_false);
    mu_run_test(test_game_over_true);
    mu_run_test(test_game_over_blocked);
    mu_run_test(test_spawn_piece);
    mu_run_test(test_spawn_blocked);
    mu_run_test(test_next_type);
    mu_run_test(test_move_down);
    mu_run_test(test_move_down_blocked);
    mu_run_test(test_multiple_moves);
    mu_run_test(test_complex_line_clear);
}

int main(void)
{
    run_all_tests();
    mu_print_summary();
    
    if (mu_tests_failed == 0) {
        printf("\nAll tests passed!\n");
    }
    
    return mu_return_status();
}
