/**
 * @file main.c
 * @brief Main entry point and game loop for CLI Tetris
 *
 * This is the final integration module that combines all other modules
 * into a playable Tetris game. It handles initialization, the main game loop,
 * timing, and cleanup.
 *
 * @author Tetris CLI Project
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "tetromino.h"
#include "game.h"
#include "renderer.h"
#include "input.h"

/**
 * @brief Timing state for game loop
 *
 * Tracks timing information for piece dropping and other time-based events.
 */
typedef struct {
    struct timespec last_drop;     /**< Time of last automatic drop */
    struct timespec last_input;    /**< Time of last input processing */
} TimingState;

/**
 * @brief Process player input actions
 *
 * Handles all input actions and updates the game state accordingly.
 * Most actions are ignored when the game is paused.
 *
 * @param game Pointer to the game state
 * @param action The input action to process
 */
static void process_input(GameState *game, InputAction action) {
    switch (action) {
        case INPUT_LEFT:
            if (!game->is_paused) {
                game_move_current(game, -1, 0);
            }
            break;

        case INPUT_RIGHT:
            if (!game->is_paused) {
                game_move_current(game, 1, 0);
            }
            break;

        case INPUT_DOWN:
            if (!game->is_paused) {
                game_move_current(game, 0, 1);
            }
            break;

        case INPUT_ROTATE_CW:
            if (!game->is_paused) {
                game_rotate_current(game, 1);
            }
            break;

        case INPUT_ROTATE_CCW:
            if (!game->is_paused) {
                game_rotate_current(game, 0);
            }
            break;

        case INPUT_HARD_DROP:
            if (!game->is_paused) {
                game_hard_drop(game);
            }
            break;

        case INPUT_PAUSE:
            game->is_paused = !game->is_paused;
            break;

        case INPUT_QUIT:
            game->is_running = 0;
            break;

        case INPUT_NONE:
        case INPUT_INVALID:
        default:
            break;
    }
}

/**
 * @brief Check if it's time for the piece to drop automatically
 *
 * Compares current time with the last drop time and the current level's
 * speed to determine if the piece should drop.
 *
 * @param game Pointer to the game state
 * @param timing Pointer to timing state
 * @return 1 if piece should drop, 0 otherwise
 */
static int time_to_drop(const GameState *game, TimingState *timing) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    int speed_ms = game_get_speed_ms(game->level);
    struct timespec drop_interval = {
        .tv_sec = speed_ms / 1000,
        .tv_nsec = (speed_ms % 1000) * 1000000L
    };

    struct timespec diff = {
        .tv_sec = now.tv_sec - timing->last_drop.tv_sec,
        .tv_nsec = now.tv_nsec - timing->last_drop.tv_nsec
    };

    /* Normalize nanoseconds if negative */
    if (diff.tv_nsec < 0) {
        diff.tv_sec--;
        diff.tv_nsec += 1000000000L;
    }

    /* Check if enough time has passed */
    if (diff.tv_sec > drop_interval.tv_sec ||
        (diff.tv_sec == drop_interval.tv_sec &&
         diff.tv_nsec >= drop_interval.tv_nsec)) {
        timing->last_drop = now;
        return 1;
    }

    return 0;
}

/**
 * @brief Initialize timing state
 *
 * Sets up the initial timing values for the game loop.
 *
 * @param timing Pointer to timing state to initialize
 */
static void timing_init(TimingState *timing) {
    clock_gettime(CLOCK_MONOTONIC, &timing->last_drop);
    clock_gettime(CLOCK_MONOTONIC, &timing->last_input);
}

/**
 * @brief Main entry point
 *
 * Initializes all subsystems, runs the main game loop, and performs cleanup.
 *
 * @return 0 on successful exit
 */
int main(void) {
    /* Seed random number generator */
    srand((unsigned int)time(NULL));

    /* Initialize subsystems */
    renderer_init();
    input_init();

    /* Initialize game state */
    GameState game;
    game_init(&game);

    /* Set up initial pieces */
    game_spawn_piece(&game, rand() % TETRO_COUNT);
    game_set_next_type(&game, rand() % TETRO_COUNT);

    /* Initialize timing */
    TimingState timing;
    timing_init(&timing);

    /* Main game loop */
    while (game.is_running) {
        /* Process input (non-blocking) */
        InputAction action = input_get_action();
        process_input(&game, action);

        /* Handle game logic when not paused */
        if (!game.is_paused) {
            /* Time-based automatic drop */
            if (time_to_drop(&game, &timing)) {
                /* Try to move piece down */
                if (!game_move_current(&game, 0, 1)) {
                    /* Cannot move down - lock the piece */
                    game_lock_piece(&game);

                    /* Check for game over */
                    if (game_check_game_over(&game)) {
                        game.is_running = 0;
                    } else {
                        /* Spawn next piece */
                        game_spawn_piece(&game, game_get_next_type(&game));
                        game_set_next_type(&game, rand() % TETRO_COUNT);
                        /* Reset drop timer for new piece */
                        clock_gettime(CLOCK_MONOTONIC, &timing.last_drop);
                    }
                }
            }
        }

        /* Render game state */
        renderer_draw_game(&game);

        /* Small delay to reduce CPU usage (10ms) */
        nanosleep(&(struct timespec){0, 10000000L}, NULL);
    }

    /* Show game over screen */
    renderer_draw_game_over(game.score);
    getch();  /* Wait for key press */

    /* Cleanup */
    renderer_cleanup();
    input_cleanup();

    return 0;
}
