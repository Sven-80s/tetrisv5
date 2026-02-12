/**
 * @file tetromino.c
 * @brief Implementation of tetromino operations
 */

#include "tetromino.h"
#include <assert.h>

/*
 * Tetromino shape definitions
 * Each tetromino has 4 rotation states as 4x4 matrices
 * 1 = occupied cell, 0 = empty
 */

/* I-Tetromino (Cyan) - Line piece */
static const int shape_I[ROTATION_COUNT][TETRO_MATRIX_SIZE][TETRO_MATRIX_SIZE] = {
    {   /* Rotation 0 (horizontal) */
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 1 (vertical) */
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    },
    {   /* Rotation 2 (horizontal, same as 0) */
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 3 (vertical, same as 1) */
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    }
};

/* O-Tetromino (Yellow) - Square piece */
static const int shape_O[ROTATION_COUNT][TETRO_MATRIX_SIZE][TETRO_MATRIX_SIZE] = {
    {   /* All rotations are identical */
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 1 (same) */
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 2 (same) */
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 3 (same) */
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

/* T-Tetromino (Magenta) */
static const int shape_T[ROTATION_COUNT][TETRO_MATRIX_SIZE][TETRO_MATRIX_SIZE] = {
    {   /* Rotation 0: T pointing up */
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 1: T pointing right */
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 2: T pointing down */
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 3: T pointing left */
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    }
};

/* S-Tetromino (Green) */
static const int shape_S[ROTATION_COUNT][TETRO_MATRIX_SIZE][TETRO_MATRIX_SIZE] = {
    {   /* Rotation 0 */
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 1 */
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 2 (same as 0) */
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 3 (same as 1) */
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    }
};

/* Z-Tetromino (Red) */
static const int shape_Z[ROTATION_COUNT][TETRO_MATRIX_SIZE][TETRO_MATRIX_SIZE] = {
    {   /* Rotation 0 */
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 1 */
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 2 (same as 0) */
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 3 (same as 1) */
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    }
};

/* J-Tetromino (Blue) */
static const int shape_J[ROTATION_COUNT][TETRO_MATRIX_SIZE][TETRO_MATRIX_SIZE] = {
    {   /* Rotation 0 */
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 1 */
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 2 */
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 3 */
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    }
};

/* L-Tetromino (White) */
static const int shape_L[ROTATION_COUNT][TETRO_MATRIX_SIZE][TETRO_MATRIX_SIZE] = {
    {   /* Rotation 0 */
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 1 */
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 2 */
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {   /* Rotation 3 */
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    }
};

/* Pointer table for all shapes */
static const int (*const shape_table[TETRO_COUNT][ROTATION_COUNT])[TETRO_MATRIX_SIZE] = {
    [TETRO_I] = { shape_I[0], shape_I[1], shape_I[2], shape_I[3] },
    [TETRO_O] = { shape_O[0], shape_O[1], shape_O[2], shape_O[3] },
    [TETRO_T] = { shape_T[0], shape_T[1], shape_T[2], shape_T[3] },
    [TETRO_S] = { shape_S[0], shape_S[1], shape_S[2], shape_S[3] },
    [TETRO_Z] = { shape_Z[0], shape_Z[1], shape_Z[2], shape_Z[3] },
    [TETRO_J] = { shape_J[0], shape_J[1], shape_J[2], shape_J[3] },
    [TETRO_L] = { shape_L[0], shape_L[1], shape_L[2], shape_L[3] }
};

/* Color table */
static const int color_table[TETRO_COUNT] = {
    [TETRO_I] = COLOR_I,
    [TETRO_O] = COLOR_O,
    [TETRO_T] = COLOR_T,
    [TETRO_S] = COLOR_S,
    [TETRO_Z] = COLOR_Z,
    [TETRO_J] = COLOR_J,
    [TETRO_L] = COLOR_L
};

Tetromino tetromino_create(TetrominoType type)
{
    assert(tetromino_type_is_valid(type));
    
    Tetromino t = {
        .type = type,
        .x = TETRO_START_X,
        .y = TETRO_START_Y,
        .rotation = 0
    };
    return t;
}

int tetromino_rotate_clockwise(int rotation)
{
    assert(tetromino_rotation_is_valid(rotation));
    return (rotation + 1) % ROTATION_COUNT;
}

int tetromino_rotate_counter_clockwise(int rotation)
{
    assert(tetromino_rotation_is_valid(rotation));
    return (rotation + ROTATION_COUNT - 1) % ROTATION_COUNT;
}

const int (*tetromino_get_shape(TetrominoType type, int rotation))[TETRO_MATRIX_SIZE]
{
    if (!tetromino_type_is_valid(type) || !tetromino_rotation_is_valid(rotation)) {
        return NULL;
    }
    return shape_table[type][rotation];
}

int tetromino_get_color(TetrominoType type)
{
    if (!tetromino_type_is_valid(type)) {
        return -1;
    }
    return color_table[type];
}

void tetromino_move(Tetromino *t, int dx, int dy)
{
    assert(t != NULL);
    t->x += dx;
    t->y += dy;
}

int tetromino_type_is_valid(TetrominoType type)
{
    return type >= 0 && type < TETRO_COUNT;
}

int tetromino_rotation_is_valid(int rotation)
{
    return rotation >= 0 && rotation < ROTATION_COUNT;
}
