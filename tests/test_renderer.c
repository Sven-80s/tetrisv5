/**
 * @file test_renderer.c
 * @brief Unit tests for the renderer module
 */

#include "../tests/minunit.h"
#include "../src/renderer.h"
#include "../src/game.h"

#include <ncurses.h>

/* Test: Renderer initialization and cleanup */
mu_test(test_renderer_init_cleanup)
{
    /* Test initialization */
    renderer_init();
    
    /* Cleanup */
    renderer_cleanup();
    
    /* If we get here without crash, test passes */
    mu_assert("renderer_init/cleanup should complete without crash", 1);
}

/* Test: Multiple init calls should not crash */
mu_test(test_renderer_multiple_init)
{
    renderer_init();
    renderer_init();  /* Second init should be safe */
    renderer_init();  /* Third init should be safe */
    
    renderer_cleanup();
    
    mu_assert("multiple init calls should be safe", 1);
}

/* Test: Multiple cleanup calls should not crash */
mu_test(test_renderer_multiple_cleanup)
{
    renderer_init();
    renderer_cleanup();
    renderer_cleanup();  /* Second cleanup should be safe */
    renderer_cleanup();  /* Third cleanup should be safe */
    
    mu_assert("multiple cleanup calls should be safe", 1);
}

/* Test: Draw board with NULL board (should not crash) */
mu_test(test_renderer_draw_board_null)
{
    renderer_init();
    
    /* Should handle NULL gracefully */
    renderer_draw_board(NULL, NULL);
    
    renderer_cleanup();
    
    mu_assert("draw_board with NULL should not crash", 1);
}

/* Test: Draw game with NULL game (should not crash) */
mu_test(test_renderer_draw_game_null)
{
    renderer_init();
    
    /* Should handle NULL gracefully */
    renderer_draw_game(NULL);
    
    renderer_cleanup();
    
    mu_assert("draw_game with NULL should not crash", 1);
}

/* Test: Draw next piece for each tetromino type */
mu_test(test_renderer_draw_next_piece_i)
{
    renderer_init();
    renderer_draw_next_piece(TETRO_I);
    renderer_cleanup();
    mu_assert("draw_next_piece I should not crash", 1);
}

mu_test(test_renderer_draw_next_piece_o)
{
    renderer_init();
    renderer_draw_next_piece(TETRO_O);
    renderer_cleanup();
    mu_assert("draw_next_piece O should not crash", 1);
}

mu_test(test_renderer_draw_next_piece_t)
{
    renderer_init();
    renderer_draw_next_piece(TETRO_T);
    renderer_cleanup();
    mu_assert("draw_next_piece T should not crash", 1);
}

mu_test(test_renderer_draw_next_piece_s)
{
    renderer_init();
    renderer_draw_next_piece(TETRO_S);
    renderer_cleanup();
    mu_assert("draw_next_piece S should not crash", 1);
}

mu_test(test_renderer_draw_next_piece_z)
{
    renderer_init();
    renderer_draw_next_piece(TETRO_Z);
    renderer_cleanup();
    mu_assert("draw_next_piece Z should not crash", 1);
}

mu_test(test_renderer_draw_next_piece_j)
{
    renderer_init();
    renderer_draw_next_piece(TETRO_J);
    renderer_cleanup();
    mu_assert("draw_next_piece J should not crash", 1);
}

mu_test(test_renderer_draw_next_piece_l)
{
    renderer_init();
    renderer_draw_next_piece(TETRO_L);
    renderer_cleanup();
    mu_assert("draw_next_piece L should not crash", 1);
}

/* Test: Draw score with various values */
mu_test(test_renderer_draw_score_zero)
{
    renderer_init();
    renderer_draw_score(0, 1, 0);
    renderer_cleanup();
    mu_assert("draw_score with zeros should not crash", 1);
}

mu_test(test_renderer_draw_score_typical)
{
    renderer_init();
    renderer_draw_score(12345, 5, 27);
    renderer_cleanup();
    mu_assert("draw_score with typical values should not crash", 1);
}

mu_test(test_renderer_draw_score_high)
{
    renderer_init();
    renderer_draw_score(999999, 99, 999);
    renderer_cleanup();
    mu_assert("draw_score with high values should not crash", 1);
}

/* Test: Draw controls */
mu_test(test_renderer_draw_controls)
{
    renderer_init();
    renderer_draw_controls();
    renderer_cleanup();
    mu_assert("draw_controls should not crash", 1);
}

/* Test: Draw pause overlay */
mu_test(test_renderer_draw_pause)
{
    renderer_init();
    renderer_draw_pause();
    renderer_cleanup();
    mu_assert("draw_pause should not crash", 1);
}

/* Test: Draw game over overlay */
mu_test(test_renderer_draw_game_over)
{
    renderer_init();
    renderer_draw_game_over(12345);
    renderer_cleanup();
    mu_assert("draw_game_over should not crash", 1);
}

/* Test: Draw complete game state */
mu_test(test_renderer_draw_game_running)
{
    GameState game;
    game_init(&game);
    
    renderer_init();
    renderer_draw_game(&game);
    renderer_cleanup();
    
    mu_assert("draw_game for running game should not crash", 1);
}

mu_test(test_renderer_draw_game_paused)
{
    GameState game;
    game_init(&game);
    game.is_paused = 1;
    
    renderer_init();
    renderer_draw_game(&game);
    renderer_cleanup();
    
    mu_assert("draw_game for paused game should not crash", 1);
}

mu_test(test_renderer_draw_game_over_state)
{
    GameState game;
    game_init(&game);
    game.is_running = 0;
    game.score = 54321;
    
    renderer_init();
    renderer_draw_game(&game);
    renderer_cleanup();
    
    mu_assert("draw_game for game over should not crash", 1);
}

/* Test: Draw board with current piece */
mu_test(test_renderer_draw_board_with_piece)
{
    GameState game;
    game_init(&game);
    
    renderer_init();
    renderer_draw_board(&game.board, &game.current);
    renderer_cleanup();
    
    mu_assert("draw_board with current piece should not crash", 1);
}

/* Test: Draw sidebar with game state */
mu_test(test_renderer_draw_sidebar)
{
    GameState game;
    game_init(&game);
    
    renderer_init();
    renderer_draw_sidebar(&game);
    renderer_cleanup();
    
    mu_assert("draw_sidebar should not crash", 1);
}

/* Test: Full rendering sequence */
mu_test(test_renderer_full_sequence)
{
    GameState game;
    game_init(&game);
    
    renderer_init();
    
    /* Simulate several frames of gameplay */
    for (int i = 0; i < 5; i++) {
        renderer_draw_game(&game);
        game.score += 100;
        game.lines += 1;
    }
    
    /* Show pause */
    game.is_paused = 1;
    renderer_draw_game(&game);
    
    /* Resume */
    game.is_paused = 0;
    renderer_draw_game(&game);
    
    /* Game over */
    game.is_running = 0;
    renderer_draw_game(&game);
    
    renderer_cleanup();
    
    mu_assert("full rendering sequence should not crash", 1);
}

/* Test: Draw board with filled cells */
mu_test(test_renderer_draw_board_filled)
{
    GameState game;
    game_init(&game);
    
    /* Add some locked pieces to the board */
    for (int x = 0; x < BOARD_WIDTH; x++) {
        game.board.cells[BOARD_HEIGHT - 1][x] = COLOR_I;
        game.board.cells[BOARD_HEIGHT - 2][x] = COLOR_O;
    }
    
    renderer_init();
    renderer_draw_board(&game.board, NULL);
    renderer_cleanup();
    
    mu_assert("draw_board with filled cells should not crash", 1);
}

/* Test: Renderer functions without init (should not crash) */
mu_test(test_renderer_draw_without_init)
{
    /* Ensure renderer is not initialized */
    renderer_cleanup();
    
    /* These should all handle uninitialized state gracefully */
    renderer_draw_board(NULL, NULL);
    renderer_draw_game(NULL);
    renderer_draw_next_piece(TETRO_I);
    renderer_draw_score(0, 0, 0);
    renderer_draw_controls();
    renderer_draw_pause();
    renderer_draw_game_over(0);
    
    mu_assert("renderer functions without init should not crash", 1);
}

/* Test suite */
mu_suite(renderer_tests)
{
    printf("\n=== Renderer Module Tests ===\n");
    
    mu_run_test(test_renderer_init_cleanup);
    mu_run_test(test_renderer_multiple_init);
    mu_run_test(test_renderer_multiple_cleanup);
    mu_run_test(test_renderer_draw_board_null);
    mu_run_test(test_renderer_draw_game_null);
    mu_run_test(test_renderer_draw_next_piece_i);
    mu_run_test(test_renderer_draw_next_piece_o);
    mu_run_test(test_renderer_draw_next_piece_t);
    mu_run_test(test_renderer_draw_next_piece_s);
    mu_run_test(test_renderer_draw_next_piece_z);
    mu_run_test(test_renderer_draw_next_piece_j);
    mu_run_test(test_renderer_draw_next_piece_l);
    mu_run_test(test_renderer_draw_score_zero);
    mu_run_test(test_renderer_draw_score_typical);
    mu_run_test(test_renderer_draw_score_high);
    mu_run_test(test_renderer_draw_controls);
    mu_run_test(test_renderer_draw_pause);
    mu_run_test(test_renderer_draw_game_over);
    mu_run_test(test_renderer_draw_game_running);
    mu_run_test(test_renderer_draw_game_paused);
    mu_run_test(test_renderer_draw_game_over_state);
    mu_run_test(test_renderer_draw_board_with_piece);
    mu_run_test(test_renderer_draw_sidebar);
    mu_run_test(test_renderer_full_sequence);
    mu_run_test(test_renderer_draw_board_filled);
    mu_run_test(test_renderer_draw_without_init);
}

int main(void)
{
    renderer_tests();
    mu_print_summary();
    return mu_return_status();
}
