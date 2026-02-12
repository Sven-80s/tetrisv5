/**
 * @file test_tetromino.c
 * @brief Unit tests for the tetromino module
 */

#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "../src/tetromino.h"

/* Test: tetromino_create creates valid tetromino */
mu_test(test_create_valid)
{
    Tetromino t = tetromino_create(TETRO_I);
    mu_assert_eq_int(TETRO_I, t.type);
    mu_assert_eq_int(TETRO_START_X, t.x);
    mu_assert_eq_int(TETRO_START_Y, t.y);
    mu_assert_eq_int(0, t.rotation);
}

/* Test: tetromino_create for all types */
mu_test(test_create_all_types)
{
    for (TetrominoType type = TETRO_I; type < TETRO_COUNT; type++) {
        Tetromino t = tetromino_create(type);
        mu_assert_eq_int(type, t.type);
        mu_assert_eq_int(TETRO_START_X, t.x);
        mu_assert_eq_int(TETRO_START_Y, t.y);
        mu_assert_eq_int(0, t.rotation);
    }
}

/* Test: rotation clockwise wraparound */
mu_test(test_rotate_clockwise)
{
    mu_assert_eq_int(1, tetromino_rotate_clockwise(0));
    mu_assert_eq_int(2, tetromino_rotate_clockwise(1));
    mu_assert_eq_int(3, tetromino_rotate_clockwise(2));
    mu_assert_eq_int(0, tetromino_rotate_clockwise(3));
}

/* Test: rotation counter-clockwise wraparound */
mu_test(test_rotate_counter_clockwise)
{
    mu_assert_eq_int(3, tetromino_rotate_counter_clockwise(0));
    mu_assert_eq_int(0, tetromino_rotate_counter_clockwise(1));
    mu_assert_eq_int(1, tetromino_rotate_counter_clockwise(2));
    mu_assert_eq_int(2, tetromino_rotate_counter_clockwise(3));
}

/* Test: get_shape returns valid pointer for all types and rotations */
mu_test(test_get_shape_all)
{
    for (TetrominoType type = TETRO_I; type < TETRO_COUNT; type++) {
        for (int rot = 0; rot < ROTATION_COUNT; rot++) {
            const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(type, rot);
            mu_assert_not_null(shape);
        }
    }
}

/* Test: get_shape returns NULL for invalid type */
mu_test(test_get_shape_invalid_type)
{
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_COUNT, 0);
    mu_assert_null(shape);
}

/* Test: get_shape returns NULL for invalid rotation */
mu_test(test_get_shape_invalid_rotation)
{
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_I, -1);
    mu_assert_null(shape);
    
    shape = tetromino_get_shape(TETRO_I, 4);
    mu_assert_null(shape);
}

/* Test: I-tetromino shape at rotation 0 (horizontal) */
mu_test(test_i_shape_rotation_0)
{
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_I, 0);
    mu_assert_not_null(shape);
    
    /* Row 1 should have 4 blocks */
    mu_assert_eq_int(0, shape[0][0]);
    mu_assert_eq_int(0, shape[0][1]);
    mu_assert_eq_int(0, shape[0][2]);
    mu_assert_eq_int(0, shape[0][3]);
    
    mu_assert_eq_int(1, shape[1][0]);
    mu_assert_eq_int(1, shape[1][1]);
    mu_assert_eq_int(1, shape[1][2]);
    mu_assert_eq_int(1, shape[1][3]);
}

/* Test: I-tetromino shape at rotation 1 (vertical) */
mu_test(test_i_shape_rotation_1)
{
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_I, 1);
    mu_assert_not_null(shape);
    
    /* Column 2 should have 4 blocks */
    mu_assert_eq_int(0, shape[0][0]);
    mu_assert_eq_int(0, shape[0][1]);
    mu_assert_eq_int(1, shape[0][2]);
    mu_assert_eq_int(0, shape[0][3]);
    
    mu_assert_eq_int(0, shape[1][0]);
    mu_assert_eq_int(0, shape[1][1]);
    mu_assert_eq_int(1, shape[1][2]);
    mu_assert_eq_int(0, shape[1][3]);
    
    mu_assert_eq_int(0, shape[2][0]);
    mu_assert_eq_int(0, shape[2][1]);
    mu_assert_eq_int(1, shape[2][2]);
    mu_assert_eq_int(0, shape[2][3]);
    
    mu_assert_eq_int(0, shape[3][0]);
    mu_assert_eq_int(0, shape[3][1]);
    mu_assert_eq_int(1, shape[3][2]);
    mu_assert_eq_int(0, shape[3][3]);
}

/* Test: O-tetromino has same shape for all rotations */
mu_test(test_o_shape_all_rotations)
{
    const int (*shape0)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_O, 0);
    const int (*shape1)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_O, 1);
    const int (*shape2)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_O, 2);
    const int (*shape3)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_O, 3);
    
    mu_assert_not_null(shape0);
    mu_assert_not_null(shape1);
    mu_assert_not_null(shape2);
    mu_assert_not_null(shape3);
    
    /* All should be identical */
    for (int i = 0; i < TETRO_MATRIX_SIZE; i++) {
        for (int j = 0; j < TETRO_MATRIX_SIZE; j++) {
            mu_assert_eq_int(shape0[i][j], shape1[i][j]);
            mu_assert_eq_int(shape0[i][j], shape2[i][j]);
            mu_assert_eq_int(shape0[i][j], shape3[i][j]);
        }
    }
}

/* Test: T-tetromino shape at rotation 0 */
mu_test(test_t_shape_rotation_0)
{
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_T, 0);
    mu_assert_not_null(shape);
    
    /* T pointing up: row 0 has center block, row 1 has 3 blocks */
    mu_assert_eq_int(0, shape[0][0]);
    mu_assert_eq_int(1, shape[0][1]);
    mu_assert_eq_int(0, shape[0][2]);
    
    mu_assert_eq_int(1, shape[1][0]);
    mu_assert_eq_int(1, shape[1][1]);
    mu_assert_eq_int(1, shape[1][2]);
}

/* Test: get_color returns valid values for all types */
mu_test(test_get_color_all_types)
{
    mu_assert_eq_int(COLOR_I, tetromino_get_color(TETRO_I));
    mu_assert_eq_int(COLOR_O, tetromino_get_color(TETRO_O));
    mu_assert_eq_int(COLOR_T, tetromino_get_color(TETRO_T));
    mu_assert_eq_int(COLOR_S, tetromino_get_color(TETRO_S));
    mu_assert_eq_int(COLOR_Z, tetromino_get_color(TETRO_Z));
    mu_assert_eq_int(COLOR_J, tetromino_get_color(TETRO_J));
    mu_assert_eq_int(COLOR_L, tetromino_get_color(TETRO_L));
}

/* Test: get_color returns -1 for invalid type */
mu_test(test_get_color_invalid)
{
    mu_assert_eq_int(-1, tetromino_get_color(TETRO_COUNT));
    mu_assert_eq_int(-1, tetromino_get_color(-1));
}

/* Test: tetromino_move updates position correctly */
mu_test(test_move)
{
    Tetromino t = tetromino_create(TETRO_I);
    
    tetromino_move(&t, 1, 0);
    mu_assert_eq_int(TETRO_START_X + 1, t.x);
    mu_assert_eq_int(TETRO_START_Y, t.y);
    
    tetromino_move(&t, 0, 1);
    mu_assert_eq_int(TETRO_START_X + 1, t.x);
    mu_assert_eq_int(TETRO_START_Y + 1, t.y);
    
    tetromino_move(&t, -2, 3);
    mu_assert_eq_int(TETRO_START_X - 1, t.x);
    mu_assert_eq_int(TETRO_START_Y + 4, t.y);
}

/* Test: type_is_valid */
mu_test(test_type_is_valid)
{
    mu_assert("TETRO_I should be valid", tetromino_type_is_valid(TETRO_I));
    mu_assert("TETRO_O should be valid", tetromino_type_is_valid(TETRO_O));
    mu_assert("TETRO_T should be valid", tetromino_type_is_valid(TETRO_T));
    mu_assert("TETRO_S should be valid", tetromino_type_is_valid(TETRO_S));
    mu_assert("TETRO_Z should be valid", tetromino_type_is_valid(TETRO_Z));
    mu_assert("TETRO_J should be valid", tetromino_type_is_valid(TETRO_J));
    mu_assert("TETRO_L should be valid", tetromino_type_is_valid(TETRO_L));
    
    mu_assert("TETRO_COUNT should be invalid", !tetromino_type_is_valid(TETRO_COUNT));
    mu_assert("-1 should be invalid", !tetromino_type_is_valid(-1));
}

/* Test: rotation_is_valid */
mu_test(test_rotation_is_valid)
{
    mu_assert("0 should be valid", tetromino_rotation_is_valid(0));
    mu_assert("1 should be valid", tetromino_rotation_is_valid(1));
    mu_assert("2 should be valid", tetromino_rotation_is_valid(2));
    mu_assert("3 should be valid", tetromino_rotation_is_valid(3));
    
    mu_assert("-1 should be invalid", !tetromino_rotation_is_valid(-1));
    mu_assert("4 should be invalid", !tetromino_rotation_is_valid(4));
}

/* Test: S and Z are mirror images (different shapes) */
mu_test(test_s_z_different)
{
    const int (*s_shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_S, 0);
    const int (*z_shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_Z, 0);
    
    mu_assert_not_null(s_shape);
    mu_assert_not_null(z_shape);
    
    /* S:  011   Z:  110
     *     110       011 */
    mu_assert_eq_int(0, s_shape[0][0]);
    mu_assert_eq_int(1, s_shape[0][1]);
    mu_assert_eq_int(1, s_shape[0][2]);
    
    mu_assert_eq_int(1, z_shape[0][0]);
    mu_assert_eq_int(1, z_shape[0][1]);
    mu_assert_eq_int(0, z_shape[0][2]);
}

/* Test: J and L are mirror images (different shapes) */
mu_test(test_j_l_different)
{
    const int (*j_shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_J, 0);
    const int (*l_shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(TETRO_L, 0);
    
    mu_assert_not_null(j_shape);
    mu_assert_not_null(l_shape);
    
    /* J block on left, L block on right */
    mu_assert_eq_int(1, j_shape[0][0]);
    mu_assert_eq_int(0, j_shape[0][2]);
    
    mu_assert_eq_int(0, l_shape[0][0]);
    mu_assert_eq_int(1, l_shape[0][2]);
}

/* Test suite runner */
static void run_all_tests(void)
{
    printf("\nRunning Tetromino Module Tests...\n");
    printf("================================\n\n");
    
    mu_run_test(test_create_valid);
    mu_run_test(test_create_all_types);
    mu_run_test(test_rotate_clockwise);
    mu_run_test(test_rotate_counter_clockwise);
    mu_run_test(test_get_shape_all);
    mu_run_test(test_get_shape_invalid_type);
    mu_run_test(test_get_shape_invalid_rotation);
    mu_run_test(test_i_shape_rotation_0);
    mu_run_test(test_i_shape_rotation_1);
    mu_run_test(test_o_shape_all_rotations);
    mu_run_test(test_t_shape_rotation_0);
    mu_run_test(test_get_color_all_types);
    mu_run_test(test_get_color_invalid);
    mu_run_test(test_move);
    mu_run_test(test_type_is_valid);
    mu_run_test(test_rotation_is_valid);
    mu_run_test(test_s_z_different);
    mu_run_test(test_j_l_different);
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
