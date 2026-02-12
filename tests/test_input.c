/**
 * @file test_input.c
 * @brief Unit tests for the input module
 */

#include "../tests/minunit.h"
#include "../src/input.h"

#include <ncurses.h>
#include <unistd.h>

/* Test: Input initialization and cleanup */
mu_test(test_input_init_cleanup)
{
    /* Initialize ncurses first (required for input module) */
    initscr();
    
    /* Test initialization */
    input_init();
    
    /* Cleanup */
    input_cleanup();
    
    /* Cleanup ncurses */
    endwin();
    
    /* If we get here, initialization and cleanup worked */
    mu_assert("input_init/cleanup should complete without crash", 1);
}

/* Test: Multiple init calls should not crash */
mu_test(test_input_multiple_init)
{
    initscr();
    
    input_init();
    input_init();  /* Second init should be safe */
    input_init();  /* Third init should be safe */
    
    input_cleanup();
    endwin();
    
    mu_assert("multiple init calls should be safe", 1);
}

/* Test: Multiple cleanup calls should not crash */
mu_test(test_input_multiple_cleanup)
{
    initscr();
    
    input_init();
    input_cleanup();
    input_cleanup();  /* Second cleanup should be safe */
    input_cleanup();  /* Third cleanup should be safe */
    
    endwin();
    
    mu_assert("multiple cleanup calls should be safe", 1);
}

/* Test: input_get_action returns INPUT_NONE when no input (non-blocking) */
mu_test(test_input_get_action_no_input)
{
    initscr();
    input_init();
    
    /* With no keys pressed, should return INPUT_NONE */
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_NONE, action);
    
    input_cleanup();
    endwin();
}

/* Test: input_has_input returns 0 when no input */
mu_test(test_input_has_input_no_input)
{
    initscr();
    input_init();
    
    /* With no keys pressed, should return 0 */
    int has_input = input_has_input();
    mu_assert_eq_int(0, has_input);
    
    input_cleanup();
    endwin();
}

/* Test: input_get_action returns INVALID without initialization */
mu_test(test_input_get_action_no_init)
{
    /* Cleanup any previous state */
    input_cleanup();
    
    /* Without initialization, should return INVALID */
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_INVALID, action);
}

/* Test: input_has_input returns 0 without initialization */
mu_test(test_input_has_input_no_init)
{
    input_cleanup();
    
    /* Without initialization, should return 0 */
    int has_input = input_has_input();
    mu_assert_eq_int(0, has_input);
}

/* Test: Simulated key mappings using ungetch */
mu_test(test_input_key_mapping_left)
{
    initscr();
    input_init();
    
    /* Simulate left arrow key */
    ungetch(KEY_LEFT);
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_LEFT, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_right)
{
    initscr();
    input_init();
    
    /* Simulate right arrow key */
    ungetch(KEY_RIGHT);
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_RIGHT, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_down)
{
    initscr();
    input_init();
    
    /* Simulate down arrow key */
    ungetch(KEY_DOWN);
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_DOWN, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_up)
{
    initscr();
    input_init();
    
    /* Simulate up arrow key */
    ungetch(KEY_UP);
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_ROTATE_CW, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_space)
{
    initscr();
    input_init();
    
    /* Simulate space bar */
    ungetch(' ');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_HARD_DROP, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_z_lowercase)
{
    initscr();
    input_init();
    
    /* Simulate lowercase z */
    ungetch('z');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_ROTATE_CCW, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_z_uppercase)
{
    initscr();
    input_init();
    
    /* Simulate uppercase Z */
    ungetch('Z');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_ROTATE_CCW, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_p_lowercase)
{
    initscr();
    input_init();
    
    /* Simulate lowercase p */
    ungetch('p');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_PAUSE, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_p_uppercase)
{
    initscr();
    input_init();
    
    /* Simulate uppercase P */
    ungetch('P');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_PAUSE, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_q_lowercase)
{
    initscr();
    input_init();
    
    /* Simulate lowercase q */
    ungetch('q');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_QUIT, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_q_uppercase)
{
    initscr();
    input_init();
    
    /* Simulate uppercase Q */
    ungetch('Q');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_QUIT, action);
    
    input_cleanup();
    endwin();
}

mu_test(test_input_key_mapping_invalid)
{
    initscr();
    input_init();
    
    /* Simulate an unknown key (e.g., 'x') */
    ungetch('x');
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_INVALID, action);
    
    input_cleanup();
    endwin();
}

/* Test: input_has_input correctly detects queued input */
mu_test(test_input_has_input_with_input)
{
    initscr();
    input_init();
    
    /* Queue a key */
    ungetch(' ');
    
    /* Should detect the queued input */
    int has_input = input_has_input();
    mu_assert_eq_int(1, has_input);
    
    /* The key should be returned by get_action */
    InputAction action = input_get_action();
    mu_assert_eq_int(INPUT_HARD_DROP, action);
    
    input_cleanup();
    endwin();
}

/* Test suite */
mu_suite(input_tests)
{
    printf("\n=== Input Module Tests ===\n");
    
    mu_run_test(test_input_init_cleanup);
    mu_run_test(test_input_multiple_init);
    mu_run_test(test_input_multiple_cleanup);
    mu_run_test(test_input_get_action_no_input);
    mu_run_test(test_input_has_input_no_input);
    mu_run_test(test_input_get_action_no_init);
    mu_run_test(test_input_has_input_no_init);
    mu_run_test(test_input_key_mapping_left);
    mu_run_test(test_input_key_mapping_right);
    mu_run_test(test_input_key_mapping_down);
    mu_run_test(test_input_key_mapping_up);
    mu_run_test(test_input_key_mapping_space);
    mu_run_test(test_input_key_mapping_z_lowercase);
    mu_run_test(test_input_key_mapping_z_uppercase);
    mu_run_test(test_input_key_mapping_p_lowercase);
    mu_run_test(test_input_key_mapping_p_uppercase);
    mu_run_test(test_input_key_mapping_q_lowercase);
    mu_run_test(test_input_key_mapping_q_uppercase);
    mu_run_test(test_input_key_mapping_invalid);
    mu_run_test(test_input_has_input_with_input);
}

int main(void)
{
    input_tests();
    mu_print_summary();
    return mu_return_status();
}
