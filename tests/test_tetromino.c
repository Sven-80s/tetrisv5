/**
 * @file test_tetromino.c
 * @brief Unit-Tests für tetromino.c
 */

#include <stdbool.h>
#include "minunit.h"
#include "../tetromino.h"

/**
 * @brief Test: tetromino_get_shape gibt gültigen Pointer zurück
 */
static const char* test_get_shape_valid(void) {
    for (int type = 0; type < NUM_TETROMINO_TYPES; type++) {
        for (int rot = 0; rot < NUM_ROTATIONS; rot++) {
            const int* shape = tetromino_get_shape((TetrominoType)type, rot);
            mu_assert("get_shape should return non-NULL for valid type/rotation", 
                      shape != NULL);
        }
    }
    return NULL;
}

/**
 * @brief Test: tetromino_get_shape gibt NULL für ungültigen Typ zurück
 */
static const char* test_get_shape_invalid(void) {
    const int* shape = tetromino_get_shape((TetrominoType)99, 0);
    mu_assert("get_shape should return NULL for invalid type", shape == NULL);
    return NULL;
}

/**
 * @brief Test: Rotation wrap-around funktioniert korrekt
 */
static const char* test_rotation_wraparound(void) {
    const int* shape0 = tetromino_get_shape(TETROMINO_I, 0);
    const int* shape4 = tetromino_get_shape(TETROMINO_I, 4);
    const int* shape8 = tetromino_get_shape(TETROMINO_I, 8);
    
    mu_assert("Rotation 0 should equal rotation 4", shape0 == shape4);
    mu_assert("Rotation 0 should equal rotation 8", shape0 == shape8);
    return NULL;
}

/**
 * @brief Test: I-Tetromino hat korrekte Grundform
 */
static const char* test_i_tetromino_shape(void) {
    const int* shape = tetromino_get_shape(TETROMINO_I, 0);
    
    /* Zeile 1 sollte voll sein (1,1,1,1) */
    mu_assert_int_eq(1, shape[4]);
    mu_assert_int_eq(1, shape[5]);
    mu_assert_int_eq(1, shape[6]);
    mu_assert_int_eq(1, shape[7]);
    
    /* Zeile 0 sollte leer sein */
    mu_assert_int_eq(0, shape[0]);
    mu_assert_int_eq(0, shape[1]);
    mu_assert_int_eq(0, shape[2]);
    mu_assert_int_eq(0, shape[3]);
    
    return NULL;
}

/**
 * @brief Test: O-Tetromino ändert sich bei Rotation nicht
 */
static const char* test_o_tetromino_rotation(void) {
    const int* shape0 = tetromino_get_shape(TETROMINO_O, 0);
    const int* shape1 = tetromino_get_shape(TETROMINO_O, 1);
    const int* shape2 = tetromino_get_shape(TETROMINO_O, 2);
    const int* shape3 = tetromino_get_shape(TETROMINO_O, 3);
    
    /* O-Tetromino sollte in allen Rotationen identisch sein */
    for (int i = 0; i < TETROMINO_SIZE * TETROMINO_SIZE; i++) {
        mu_assert_int_eq(shape0[i], shape1[i]);
        mu_assert_int_eq(shape0[i], shape2[i]);
        mu_assert_int_eq(shape0[i], shape3[i]);
    }
    
    return NULL;
}

/**
 * @brief Test: tetromino_get_color gibt korrekte Farben zurück
 */
static const char* test_get_color(void) {
    mu_assert_int_eq(COLOR_I, tetromino_get_color(TETROMINO_I));
    mu_assert_int_eq(COLOR_O, tetromino_get_color(TETROMINO_O));
    mu_assert_int_eq(COLOR_T, tetromino_get_color(TETROMINO_T));
    mu_assert_int_eq(COLOR_S, tetromino_get_color(TETROMINO_S));
    mu_assert_int_eq(COLOR_Z, tetromino_get_color(TETROMINO_Z));
    mu_assert_int_eq(COLOR_J, tetromino_get_color(TETROMINO_J));
    mu_assert_int_eq(COLOR_L, tetromino_get_color(TETROMINO_L));
    return NULL;
}

/**
 * @brief Test: tetromino_create initialisiert korrekt
 */
static const char* test_create(void) {
    Tetromino t = tetromino_create(TETROMINO_T, 5, 3);
    
    mu_assert_int_eq(TETROMINO_T, t.type);
    mu_assert_int_eq(0, t.rotation);
    mu_assert_int_eq(5, t.x);
    mu_assert_int_eq(3, t.y);
    
    return NULL;
}

/**
 * @brief Test: Rotation im Uhrzeigersinn
 */
static const char* test_rotate_clockwise(void) {
    Tetromino t = tetromino_create(TETROMINO_I, 0, 0);
    
    mu_assert_int_eq(0, t.rotation);
    
    tetromino_rotate_clockwise(&t);
    mu_assert_int_eq(1, t.rotation);
    
    tetromino_rotate_clockwise(&t);
    mu_assert_int_eq(2, t.rotation);
    
    tetromino_rotate_clockwise(&t);
    mu_assert_int_eq(3, t.rotation);
    
    tetromino_rotate_clockwise(&t);
    mu_assert_int_eq(0, t.rotation);  /* Wrap-around */
    
    return NULL;
}

/**
 * @brief Test: Rotation gegen den Uhrzeigersinn
 */
static const char* test_rotate_counter_clockwise(void) {
    Tetromino t = tetromino_create(TETROMINO_I, 0, 0);
    
    mu_assert_int_eq(0, t.rotation);
    
    tetromino_rotate_counter_clockwise(&t);
    mu_assert_int_eq(3, t.rotation);  /* 0 -> 3 */
    
    tetromino_rotate_counter_clockwise(&t);
    mu_assert_int_eq(2, t.rotation);  /* 3 -> 2 */
    
    tetromino_rotate_counter_clockwise(&t);
    mu_assert_int_eq(1, t.rotation);  /* 2 -> 1 */
    
    tetromino_rotate_counter_clockwise(&t);
    mu_assert_int_eq(0, t.rotation);  /* 1 -> 0 */
    
    return NULL;
}

/**
 * @brief Test: Bewegung nach links
 */
static const char* test_move_left(void) {
    Tetromino t = tetromino_create(TETROMINO_I, 5, 5);
    
    tetromino_move_left(&t);
    mu_assert_int_eq(4, t.x);
    mu_assert_int_eq(5, t.y);
    
    return NULL;
}

/**
 * @brief Test: Bewegung nach rechts
 */
static const char* test_move_right(void) {
    Tetromino t = tetromino_create(TETROMINO_I, 5, 5);
    
    tetromino_move_right(&t);
    mu_assert_int_eq(6, t.x);
    mu_assert_int_eq(5, t.y);
    
    return NULL;
}

/**
 * @brief Test: Bewegung nach unten
 */
static const char* test_move_down(void) {
    Tetromino t = tetromino_create(TETROMINO_I, 5, 5);
    
    tetromino_move_down(&t);
    mu_assert_int_eq(5, t.x);
    mu_assert_int_eq(6, t.y);
    
    return NULL;
}

/**
 * @brief Test: Kollision mit Board-Grenze (rechts)
 */
static const char* test_collision_board_boundary_right(void) {
    int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH] = {{0}};
    Tetromino t = tetromino_create(TETROMINO_I, 8, 0);
    
    /* I-Tetromino bei x=8 kollidiert mit rechter Grenze */
    bool collision = tetromino_check_collision(&t, board);
    mu_assert("Should detect collision with right boundary", collision);
    
    return NULL;
}

/**
 * @brief Test: Kollision mit Board-Grenze (links)
 */
static const char* test_collision_board_boundary_left(void) {
    int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH] = {{0}};
    Tetromino t = tetromino_create(TETROMINO_I, -2, 0);
    
    /* I-Tetromino bei x=-2 kollidiert mit linker Grenze */
    bool collision = tetromino_check_collision(&t, board);
    mu_assert("Should detect collision with left boundary", collision);
    
    return NULL;
}

/**
 * @brief Test: Keine Kollision in gültiger Position
 */
static const char* test_no_collision(void) {
    int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH] = {{0}};
    Tetromino t = tetromino_create(TETROMINO_O, 4, 4);
    
    bool collision = tetromino_check_collision(&t, board);
    mu_assert("Should not detect collision in valid position", !collision);
    
    return NULL;
}

/**
 * @brief Test: Kollision mit vorhandenem Block
 */
static const char* test_collision_with_block(void) {
    int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH] = {{0}};
    /* Setze Block direkt unter ein O-Tetromino bei y=3
     * O-Tetromino bei y=3 belegt Zeilen 3 und 4
     * Wenn wir einen Block bei y=5 setzen und das Tetromino bei y=4 bewegen,
     * sollte es kollidieren
     */
    board[5][5] = COLOR_I;
    
    Tetromino t = tetromino_create(TETROMINO_O, 4, 4);
    
    bool collision = tetromino_check_collision(&t, board);
    /* Bei y=4 belegt O-Tetromino Zeilen 4 und 5, also Kollision bei (5,5) */
    mu_assert("Should detect collision with existing block", collision);
    
    return NULL;
}

/**
 * @brief Test: Kollision am unteren Rand
 */
static const char* test_collision_bottom(void) {
    int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH] = {{0}};
    Tetromino t = tetromino_create(TETROMINO_O, 4, 18);
    
    /* O-Tetromino bei y=18 kollidiert mit unterer Grenze (belegt Zeilen 18,19) */
    bool collision = tetromino_check_collision(&t, board);
    mu_assert("O at y=18 should not collide yet", !collision);
    
    /* Nach unten bewegen */
    tetromino_move_down(&t);
    collision = tetromino_check_collision(&t, board);
    mu_assert("O at y=19 should collide (occupies row 20)", collision);
    
    return NULL;
}

/**
 * @brief Test: NULL-Pointer Handling bei Rotation
 */
static const char* test_null_pointer_rotation(void) {
    /* Sollte nicht abstürzen */
    tetromino_rotate_clockwise(NULL);
    tetromino_rotate_counter_clockwise(NULL);
    return NULL;
}

/**
 * @brief Test: NULL-Pointer Handling bei Bewegung
 */
static const char* test_null_pointer_move(void) {
    /* Sollte nicht abstürzen */
    tetromino_move_left(NULL);
    tetromino_move_right(NULL);
    tetromino_move_down(NULL);
    return NULL;
}

/**
 * @brief Test: NULL-Pointer Handling bei Kollision
 */
static const char* test_null_pointer_collision(void) {
    int board[TETROMINO_BOARD_HEIGHT][TETROMINO_BOARD_WIDTH] = {{0}};
    Tetromino t = tetromino_create(TETROMINO_I, 0, 0);
    
    /* Sollte nicht abstürzen und true zurückgeben */
    bool result = tetromino_check_collision(NULL, board);
    mu_assert("NULL tetromino should return collision", result);
    
    result = tetromino_check_collision(&t, NULL);
    mu_assert("NULL board should return collision", result);
    
    return NULL;
}

/**
 * @brief Hauptfunktion für Tetromino-Tests
 * @return Anzahl der fehlgeschlagenen Tests
 */
int main(void) {
    printf("\n========================================\n");
    printf("Running Tetromino Tests\n");
    printf("========================================\n\n");
    
    mu_suite_start();
    
    mu_run_test(test_get_shape_valid);
    mu_run_test(test_get_shape_invalid);
    mu_run_test(test_rotation_wraparound);
    mu_run_test(test_i_tetromino_shape);
    mu_run_test(test_o_tetromino_rotation);
    mu_run_test(test_get_color);
    mu_run_test(test_create);
    mu_run_test(test_rotate_clockwise);
    mu_run_test(test_rotate_counter_clockwise);
    mu_run_test(test_move_left);
    mu_run_test(test_move_right);
    mu_run_test(test_move_down);
    mu_run_test(test_collision_board_boundary_right);
    mu_run_test(test_collision_board_boundary_left);
    mu_run_test(test_no_collision);
    mu_run_test(test_collision_with_block);
    mu_run_test(test_collision_bottom);
    mu_run_test(test_null_pointer_rotation);
    mu_run_test(test_null_pointer_move);
    mu_run_test(test_null_pointer_collision);
    
    mu_suite_report();
}
