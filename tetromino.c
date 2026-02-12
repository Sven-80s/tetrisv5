#include "tetromino.h"

#include <stdlib.h>

/**
 * @brief Tetromino-Shape-Definitionen für alle Rotationen
 * 
 * Jedes Tetromino wird als 4x4 Matrix gespeichert.
 * Die Indizes sind: [TetrominoType][Rotation][y][x]
 * 1 = gefüllte Zelle, 0 = leer
 */
static const int SHAPES[NUM_TETROMINO_TYPES][NUM_ROTATIONS][TETROMINO_SIZE][TETROMINO_SIZE] = {
    /* I - Gerade */
    {
        {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}
    },
    /* O - Quadrat */
    {
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}
    },
    /* T - T-Form */
    {
        {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}
    },
    /* S - S-Form */
    {
        {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
        {{1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}
    },
    /* Z - Z-Form */
    {
        {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,1,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0}}
    },
    /* J - J-Form */
    {
        {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,1,0}, {0,0,1,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}}
    },
    /* L - L-Form */
    {
        {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,1,0}, {1,0,0,0}, {0,0,0,0}},
        {{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}
    }
};

const int* tetromino_get_shape(TetrominoType type, int rotation) {
    if (type < 0 || type >= NUM_TETROMINO_TYPES) {
        return NULL;
    }
    rotation = rotation % NUM_ROTATIONS;
    return &SHAPES[type][rotation][0][0];
}

TetrominoColor tetromino_get_color(TetrominoType type) {
    switch (type) {
        case TETROMINO_I: return COLOR_I;
        case TETROMINO_O: return COLOR_O;
        case TETROMINO_T: return COLOR_T;
        case TETROMINO_S: return COLOR_S;
        case TETROMINO_Z: return COLOR_Z;
        case TETROMINO_J: return COLOR_J;
        case TETROMINO_L: return COLOR_L;
        default: return COLOR_EMPTY;
    }
}

Tetromino tetromino_create(TetrominoType type, int start_x, int start_y) {
    Tetromino t;
    t.type = type;
    t.rotation = 0;
    t.x = start_x;
    t.y = start_y;
    return t;
}

void tetromino_rotate_clockwise(Tetromino *t) {
    if (t == NULL) return;
    t->rotation = (t->rotation + 1) % NUM_ROTATIONS;
}

void tetromino_rotate_counter_clockwise(Tetromino *t) {
    if (t == NULL) return;
    t->rotation = (t->rotation + NUM_ROTATIONS - 1) % NUM_ROTATIONS;
}

void tetromino_move_left(Tetromino *t) {
    if (t == NULL) return;
    t->x--;
}

void tetromino_move_right(Tetromino *t) {
    if (t == NULL) return;
    t->x++;
}

void tetromino_move_down(Tetromino *t) {
    if (t == NULL) return;
    t->y++;
}

bool tetromino_check_collision(const Tetromino *t, int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH]) {
    if (t == NULL || board == NULL) return true;
    
    const int *shape = tetromino_get_shape(t->type, t->rotation);
    if (shape == NULL) return true;
    
    for (int row = 0; row < TETROMINO_SIZE; row++) {
        for (int col = 0; col < TETROMINO_SIZE; col++) {
            int shape_value = shape[row * TETROMINO_SIZE + col];
            
            if (shape_value != 0) {
                int board_x = t->x + col;
                int board_y = t->y + row;
                
                /* Prüfe Board-Grenzen */
                if (board_x < 0 || board_x >= TETROMINO_BOARD_WIDTH || board_y >= TETROMINO_BOARD_HEIGHT) {
                    return true;
                }
                
                /* Nur prüfen wenn innerhalb des Boards (y >= 0) */
                if (board_y >= 0 && board[board_y][board_x] != 0) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool tetromino_check_position(TetrominoType type, int rotation, 
                               int board_x, int board_y, 
                               int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH]) {
    const int *shape = tetromino_get_shape(type, rotation);
    if (shape == NULL) return false;
    
    for (int row = 0; row < TETROMINO_SIZE; row++) {
        for (int col = 0; col < TETROMINO_SIZE; col++) {
            int shape_value = shape[row * TETROMINO_SIZE + col];
            
            if (shape_value != 0) {
                int cell_x = board_x + col;
                int cell_y = board_y + row;
                
                if (cell_x < 0 || cell_x >= TETROMINO_BOARD_WIDTH || cell_y >= TETROMINO_BOARD_HEIGHT) {
                    return false;
                }
                
                if (cell_y >= 0 && board[cell_y][cell_x] != 0) {
                    return false;
                }
            }
        }
    }
    
    return true;
}
