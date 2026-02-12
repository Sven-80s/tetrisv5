/**
 * @file game.c
 * @brief Implementation of the Game Engine module
 */

#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

/**
 * @brief Generates a random tetromino type (0-6)
 * @return Random TetrominoType
 */
static TetrominoType random_type(void)
{
    return (TetrominoType)(rand() % TETRO_COUNT);
}

/**
 * @brief Clears the board (sets all cells to 0)
 * @param board Pointer to Board to clear
 */
static void clear_board(Board *board)
{
    memset(board->cells, 0, sizeof(board->cells));
}

void game_init(GameState *game)
{
    assert(game != NULL);
    
    /* Seed random number generator (only once per process ideally) */
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }
    
    /* Clear the board */
    clear_board(&game->board);
    
    /* Initialize game statistics */
    game->score = 0;
    game->level = 1;
    game->lines = 0;
    game->is_running = 1;
    game->is_paused = 0;
    
    /* Generate first pieces */
    game->next = tetromino_create(random_type());
    game_spawn_piece(game, random_type());
}

void game_reset(GameState *game)
{
    assert(game != NULL);
    game_init(game);
}

int game_spawn_piece(GameState *game, TetrominoType type)
{
    assert(game != NULL);
    assert(tetromino_type_is_valid(type));
    
    Tetromino new_piece = tetromino_create(type);
    
    /* Check if spawn position is valid */
    if (!game_is_valid_position(game, &new_piece)) {
        game->is_running = 0;
        return 0;
    }
    
    game->current = new_piece;
    return 1;
}

int game_move_current(GameState *game, int dx, int dy)
{
    assert(game != NULL);
    
    /* Create test piece at new position */
    Tetromino test = game->current;
    tetromino_move(&test, dx, dy);
    
    /* Check if new position is valid */
    if (!game_is_valid_position(game, &test)) {
        return 0;
    }
    
    /* Apply the move */
    game->current = test;
    return 1;
}

int game_rotate_current(GameState *game, int clockwise)
{
    assert(game != NULL);
    
    /* Create test piece with new rotation */
    Tetromino test = game->current;
    
    if (clockwise) {
        test.rotation = tetromino_rotate_clockwise(test.rotation);
    } else {
        test.rotation = tetromino_rotate_counter_clockwise(test.rotation);
    }
    
    /* Check if new rotation is valid */
    if (!game_is_valid_position(game, &test)) {
        return 0;
    }
    
    /* Apply the rotation */
    game->current = test;
    return 1;
}

int game_hard_drop(GameState *game)
{
    assert(game != NULL);
    
    int drop_distance = 0;
    
    /* Move down until blocked */
    while (game_move_current(game, 0, 1)) {
        drop_distance++;
    }
    
    /* Lock the piece */
    game_lock_piece(game);
    
    return drop_distance;
}

int game_lock_piece(GameState *game)
{
    assert(game != NULL);
    
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(
        game->current.type, game->current.rotation);
    
    if (shape == NULL) {
        return 0;
    }
    
    int color = tetromino_get_color(game->current.type);
    
    /* Copy tetromino cells to board */
    for (int row = 0; row < TETRO_MATRIX_SIZE; row++) {
        for (int col = 0; col < TETRO_MATRIX_SIZE; col++) {
            if (shape[row][col] == 1) {
                int board_x = game->current.x + col;
                int board_y = game->current.y + row;
                
                /* Ensure within bounds */
                if (board_x >= 0 && board_x < BOARD_WIDTH &&
                    board_y >= 0 && board_y < BOARD_HEIGHT) {
                    game->board.cells[board_y][board_x] = color;
                }
            }
        }
    }
    
    /* Move next to current and generate new next */
    game->current = game->next;
    game->next = tetromino_create(random_type());
    
    /* Check if new current piece can be placed */
    if (!game_is_valid_position(game, &game->current)) {
        game->is_running = 0;
        return 0;
    }
    
    /* Clear lines and return count */
    return game_clear_lines(game);
}

int game_clear_lines(GameState *game)
{
    assert(game != NULL);
    
    int lines_cleared = 0;
    int write_row = BOARD_HEIGHT - 1;
    
    /* Scan from bottom to top */
    for (int read_row = BOARD_HEIGHT - 1; read_row >= 0; read_row--) {
        int is_full = 1;
        
        /* Check if row is full */
        for (int col = 0; col < BOARD_WIDTH; col++) {
            if (game->board.cells[read_row][col] == 0) {
                is_full = 0;
                break;
            }
        }
        
        if (is_full) {
            /* Skip this row (don't copy it) - it's cleared */
            lines_cleared++;
        } else {
            /* Copy row to write position */
            if (write_row != read_row) {
                memcpy(game->board.cells[write_row],
                       game->board.cells[read_row],
                       sizeof(game->board.cells[write_row]));
            }
            write_row--;
        }
    }
    
    /* Clear remaining rows at top */
    for (int row = write_row; row >= 0; row--) {
        memset(game->board.cells[row], 0, sizeof(game->board.cells[row]));
    }
    
    /* Update statistics */
    if (lines_cleared > 0) {
        game->lines += lines_cleared;
        game->score += game_calculate_score(lines_cleared, game->level);
        
        /* Update level: level = (lines / 10) + 1 */
        game->level = (game->lines / 10) + 1;
    }
    
    return lines_cleared;
}

int game_calculate_score(int lines_cleared, int level)
{
    /* Standard Tetris scoring */
    switch (lines_cleared) {
        case 1:
            return 100 * level;
        case 2:
            return 300 * level;
        case 3:
            return 500 * level;
        case 4:
            return 800 * level;  /* Tetris! */
        default:
            return 0;
    }
}

int game_is_valid_position(const GameState *game, const Tetromino *t)
{
    assert(game != NULL);
    assert(t != NULL);
    
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(t->type, t->rotation);
    
    if (shape == NULL) {
        return 0;
    }
    
    /* Check each cell of the tetromino */
    for (int row = 0; row < TETRO_MATRIX_SIZE; row++) {
        for (int col = 0; col < TETRO_MATRIX_SIZE; col++) {
            if (shape[row][col] == 1) {
                int board_x = t->x + col;
                int board_y = t->y + row;
                
                /* Check board boundaries */
                if (board_x < 0 || board_x >= BOARD_WIDTH) {
                    return 0;
                }
                if (board_y < 0 || board_y >= BOARD_HEIGHT) {
                    return 0;
                }
                
                /* Check collision with locked pieces */
                if (game->board.cells[board_y][board_x] != 0) {
                    return 0;
                }
            }
        }
    }
    
    return 1;
}

int game_get_speed_ms(int level)
{
    if (level < 1) {
        level = 1;
    }
    
    int speed = 1000 - (level - 1) * 100;
    
    if (speed < 100) {
        speed = 100;
    }
    
    return speed;
}

int game_check_game_over(const GameState *game)
{
    assert(game != NULL);
    
    /* Game is over if is_running is 0 */
    if (!game->is_running) {
        return 1;
    }
    
    /* Or if current piece is not at valid position */
    Tetromino start = tetromino_create(game->current.type);
    if (!game_is_valid_position(game, &start)) {
        return 1;
    }
    
    return 0;
}

TetrominoType game_get_next_type(const GameState *game)
{
    assert(game != NULL);
    return game->next.type;
}

void game_set_next_type(GameState *game, TetrominoType type)
{
    assert(game != NULL);
    assert(tetromino_type_is_valid(type));
    game->next = tetromino_create(type);
}
