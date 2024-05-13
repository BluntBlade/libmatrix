#include <exotic/cester.h>

#ifndef MATRIX_C
#include "matrix.c"
#define MATRIX_C
#endif

/* ==== Tests for mtx_int32_t matrix. ==== */

CESTER_TEST(
    MATRIX_i32_allocate_for_1x1_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(1, 1);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1);
    cester_assert_uint_eq(m->col_cnt, 1);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_1x2_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(1, 2);
    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1);
    cester_assert_uint_eq(m->col_cnt, 2);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_1x5_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(1, 5);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1);
    cester_assert_uint_eq(m->col_cnt, 5);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 2);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_2x1_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(2, 1);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 2);
    cester_assert_uint_eq(m->col_cnt, 1);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_6x1_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(6, 1);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 6);
    cester_assert_uint_eq(m->col_cnt, 1);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_4x4_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(4, 4);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 4);
    cester_assert_uint_eq(m->col_cnt, 4);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_7x7_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(7, 7);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 7);
    cester_assert_uint_eq(m->col_cnt, 7);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 2);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_8x8_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(8, 8);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 8);
    cester_assert_uint_eq(m->col_cnt, 8);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 2);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_1024x1024_matrix,
    _,

    ptr_matrix_t m = mtx_i32_allocate(1024, 1024);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1024);
    cester_assert_uint_eq(m->col_cnt, 1024);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 256);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 256);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 256);
    cester_assert_not_null(m->data);
    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_set_and_get_one_value_in_5x5_matrix,
    _,
    mtx_int32_t val1 = 0;
    mtx_int32_t val2 = 0;
    ptr_matrix_t m = mtx_i32_allocate(5, 5);

    mtx_i32_set(m, 0, 0, 1);
    mtx_i32_set(m, 4, 4, 5);

    val1 = mtx_i32_get(m, 0, 0);
    val2 = mtx_i32_get(m, 4, 4);

    cester_assert_uint_eq(m->i32_vals[0][0], 1);
    cester_assert_uint_eq(m->i32_vals[4][4], 5);
    cester_assert_uint_eq(val1, 1);
    cester_assert_uint_eq(val2, 5);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_set_each_value_to_5_in_2x2_matrix,
    _,
    mtx_int32_t val = 0;
    ptr_matrix_t m = mtx_i32_allocate(2, 2);

    mtx_i32_set_each(m, 5);

    val = mtx_i32_get(m, 0, 0);
    cester_assert_uint_eq(val, 5);
    val = mtx_i32_get(m, 0, 1);
    cester_assert_uint_eq(val, 5);
    val = mtx_i32_get(m, 1, 0);
    cester_assert_uint_eq(val, 5);
    val = mtx_i32_get(m, 1, 1);
    cester_assert_uint_eq(val, 5);

    mtx_destroy(m);
)

/* ==== Tests for common functions. ==== */

CESTER_TEST(
    MATRIX_allocate_for_multiplying_5x1_and_1x5_matrix,
    _,

    ptr_matrix_t lhs = mtx_i32_allocate(5, 1);
    ptr_matrix_t rhs = mtx_i32_allocate(1, 5);
    ptr_matrix_t m = mtx_allocate_for_multiplying(lhs, rhs);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 5);
    cester_assert_uint_eq(m->col_cnt, 5);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 2);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, lhs);
    cester_assert_ptr_not_equal(m, rhs);

    mtx_destroy(m);
    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_allocate_for_multiplying_1x5_and_5x1_matrix,
    _,

    ptr_matrix_t lhs = mtx_i32_allocate(1, 5);
    ptr_matrix_t rhs = mtx_i32_allocate(5, 1);
    ptr_matrix_t m = mtx_allocate_for_multiplying(lhs, rhs);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1);
    cester_assert_uint_eq(m->col_cnt, 1);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, lhs);
    cester_assert_ptr_not_equal(m, rhs);

    mtx_destroy(m);
    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_allocate_for_multiplying_8x8_and_8x8_matrix,
    _,

    ptr_matrix_t lhs = mtx_i32_allocate(8, 8);
    ptr_matrix_t rhs = mtx_i32_allocate(8, 8);
    ptr_matrix_t m = mtx_allocate_for_multiplying(lhs, rhs);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 8);
    cester_assert_uint_eq(m->col_cnt, 8);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 2);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, lhs);
    cester_assert_ptr_not_equal(m, rhs);

    mtx_destroy(m);
    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_allocate_for_transposing_5x1_matrix,
    _,

    ptr_matrix_t src = mtx_i32_allocate(5, 1);
    ptr_matrix_t m = mtx_allocate_for_transposing(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1);
    cester_assert_uint_eq(m->col_cnt, 5);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 2);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, src);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_for_transposing_1x5_matrix,
    _,

    ptr_matrix_t src = mtx_i32_allocate(1, 5);
    ptr_matrix_t m = mtx_allocate_for_transposing(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 5);
    cester_assert_uint_eq(m->col_cnt, 1);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 2);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, src);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_for_transposing_13x13_matrix,
    _,

    ptr_matrix_t src = mtx_i32_allocate(13, 13);
    ptr_matrix_t m = mtx_allocate_for_transposing(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 13);
    cester_assert_uint_eq(m->col_cnt, 13);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 4);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 4);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 4);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, src);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_in_shape_of_3x1_matrix,
    _,

    ptr_matrix_t src = mtx_i32_allocate(3, 1);
    ptr_matrix_t m = mtx_allocate_in_shape_of(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 3);
    cester_assert_uint_eq(m->col_cnt, 1);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, src);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_in_shape_of_1x3_matrix,
    _,

    ptr_matrix_t src = mtx_i32_allocate(1, 3);
    ptr_matrix_t m = mtx_allocate_in_shape_of(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1);
    cester_assert_uint_eq(m->col_cnt, 3);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, src);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_in_shape_of_3x3_matrix,
    _,

    ptr_matrix_t src = mtx_i32_allocate(3, 3);
    ptr_matrix_t m = mtx_allocate_in_shape_of(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 3);
    cester_assert_uint_eq(m->col_cnt, 3);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 1);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 1);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, src);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_duplicate_10x10_matrix_of_random_values,
    _,

    ptr_matrix_t src = mtx_i32_allocate(10, 10);
    ptr_matrix_t m = mtx_duplicate(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 10);
    cester_assert_uint_eq(m->col_cnt, 10);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN * 3);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN * 3);
    cester_assert_uint_eq(m->padded_byte_cnt, m->value_size * m->padded_row_cnt * m->padded_col_cnt);
    cester_assert_uint_eq(m->pack_cnt_per_row, 3);
    cester_assert_not_null(m->data);

    cester_assert_ptr_not_equal(m, src);

    cester_assert_int_eq(m->i32_vals[0][0], src->i32_vals[0][0]);
    cester_assert_int_eq(m->i32_vals[0][1], src->i32_vals[0][1]);
    cester_assert_int_eq(m->i32_vals[1][0], src->i32_vals[1][0]);
    cester_assert_int_eq(m->i32_vals[1][1], src->i32_vals[1][1]);
    cester_assert_int_eq(m->i32_vals[8][9], src->i32_vals[8][9]);
    cester_assert_int_eq(m->i32_vals[9][8], src->i32_vals[9][8]);
    cester_assert_int_eq(m->i32_vals[9][9], src->i32_vals[9][9]);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_initialize_15x15_identity_matrix_using_plain_code,
    _,
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    ptr_matrix_t m = mtx_i32_allocate(15, 15);

    mtx_initialize_identity(m, MTX_PLAIN_CODE);

    for (i = 0; i < m->row_cnt; i += 1) {
        for (j = 0; j < m->col_cnt; j += 1) {
            if (i == j) {
                cester_assert_int_eq(m->i32_vals[i][j], 1);
            } else {
                cester_assert_int_eq(m->i32_vals[i][j], 0);
            } /* if */
        } /* for */
    } /* for */

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_initialize_9x5_zeros_matrix_using_plain_code,
    _,
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    ptr_matrix_t m = mtx_i32_allocate(9, 5);

    mtx_initialize_zeros(m, MTX_PLAIN_CODE);

    for (i = 0; i < m->row_cnt; i += 1) {
        for (j = 0; j < m->col_cnt; j += 1) {
            cester_assert_int_eq(m->i32_vals[i][j], 0);
        } /* for */
    } /* for */

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_initialize_5x9_ones_matrix_using_plain_code,
    _,
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    ptr_matrix_t m = mtx_i32_allocate(5, 9);

    mtx_initialize_ones(m, MTX_PLAIN_CODE);

    for (i = 0; i < m->row_cnt; i += 1) {
        for (j = 0; j < m->col_cnt; j += 1) {
            cester_assert_int_eq(m->i32_vals[i][j], 1);
        } /* for */
    } /* for */

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_can_do_add_btween_2x2_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(2, 2);
    ptr_matrix_t rhs = mtx_i32_allocate(2, 2);
    int ret = mtx_can_do_add(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_add_btween_2x5_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(2, 5);
    ptr_matrix_t rhs = mtx_i32_allocate(2, 5);
    int ret = mtx_can_do_add(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_cannot_do_add_btween_2x2_and_3x3_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(2, 2);
    ptr_matrix_t rhs = mtx_i32_allocate(3, 3);
    int ret = mtx_can_do_add(lhs, rhs);

    cester_assert_int_eq(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_sub_btween_3x3_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(3, 3);
    ptr_matrix_t rhs = mtx_i32_allocate(3, 3);
    int ret = mtx_can_do_sub(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_sub_btween_5x2_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(5, 2);
    ptr_matrix_t rhs = mtx_i32_allocate(5, 2);
    int ret = mtx_can_do_sub(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_cannot_do_sub_btween_1x7_and_7x1_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(1, 7);
    ptr_matrix_t rhs = mtx_i32_allocate(7, 1);
    int ret = mtx_can_do_sub(lhs, rhs);

    cester_assert_int_eq(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_mul_btween_13x13_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(13, 13);
    ptr_matrix_t rhs = mtx_i32_allocate(13, 13);
    int ret = mtx_can_do_multiply(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_mul_btween_2x4_and_4x5_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(2, 4);
    ptr_matrix_t rhs = mtx_i32_allocate(4, 5);
    int ret = mtx_can_do_multiply(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_mul_btween_5x3_and_3x2_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(5, 3);
    ptr_matrix_t rhs = mtx_i32_allocate(3, 2);
    int ret = mtx_can_do_multiply(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_cannot_do_mul_btween_5x7_and_8x2_matrices,
    _,
    ptr_matrix_t lhs = mtx_i32_allocate(5, 7);
    ptr_matrix_t rhs = mtx_i32_allocate(8, 2);
    int ret = mtx_can_do_multiply(lhs, rhs);

    cester_assert_int_eq(ret, 0);

    mtx_destroy(rhs);
    mtx_destroy(lhs);
)

CESTER_TEST(
    MATRIX_count_rows_columns_and_values_for_3x7_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(3, 7);

    cester_assert_int_eq(mtx_count_rows(m), 3);
    cester_assert_int_eq(mtx_count_columns(m), 7);
    cester_assert_int_eq(mtx_count_values(m), 21);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_on_left_top_corner_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 0, 0, 5, 5, &ref);

    cester_assert_int_eq(ref.row_off, 0);
    cester_assert_int_eq(ref.col_off, 0);
    cester_assert_int_eq(ref.row_cnt, 5);
    cester_assert_int_eq(ref.col_cnt, 5);
    cester_assert_ptr_equal(ref.val_ptrs[0], m->val_ptrs[0]);
    cester_assert_ptr_equal(ref.val_ptrs[1], m->val_ptrs[1]);
    cester_assert_ptr_equal(ref.val_ptrs[2], m->val_ptrs[2]);
    cester_assert_ptr_equal(ref.val_ptrs[3], m->val_ptrs[3]);
    cester_assert_ptr_equal(ref.val_ptrs[4], m->val_ptrs[4]);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_on_right_top_corner_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 11, 0, 5, 5, &ref);

    cester_assert_int_eq(ref.row_off, 11);
    cester_assert_int_eq(ref.col_off, 0);
    cester_assert_int_eq(ref.row_cnt, 5);
    cester_assert_int_eq(ref.col_cnt, 5);
    cester_assert_ptr_equal(ref.val_ptrs[0], m->val_ptrs[0]);
    cester_assert_ptr_equal(ref.val_ptrs[1], m->val_ptrs[1]);
    cester_assert_ptr_equal(ref.val_ptrs[2], m->val_ptrs[2]);
    cester_assert_ptr_equal(ref.val_ptrs[3], m->val_ptrs[3]);
    cester_assert_ptr_equal(ref.val_ptrs[4], m->val_ptrs[4]);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_on_left_bottom_corner_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 0, 11, 5, 5, &ref);

    cester_assert_int_eq(ref.row_off, 0);
    cester_assert_int_eq(ref.col_off, 11);
    cester_assert_int_eq(ref.row_cnt, 5);
    cester_assert_int_eq(ref.col_cnt, 5);
    cester_assert_ptr_equal(ref.val_ptrs[0], m->val_ptrs[0]);
    cester_assert_ptr_equal(ref.val_ptrs[1], m->val_ptrs[1]);
    cester_assert_ptr_equal(ref.val_ptrs[2], m->val_ptrs[2]);
    cester_assert_ptr_equal(ref.val_ptrs[3], m->val_ptrs[3]);
    cester_assert_ptr_equal(ref.val_ptrs[4], m->val_ptrs[4]);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_on_right_bottom_corner_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 11, 11, 5, 5, &ref);

    cester_assert_int_eq(ref.row_off, 11);
    cester_assert_int_eq(ref.col_off, 11);
    cester_assert_int_eq(ref.row_cnt, 5);
    cester_assert_int_eq(ref.col_cnt, 5);
    cester_assert_ptr_equal(ref.val_ptrs[0], m->val_ptrs[0]);
    cester_assert_ptr_equal(ref.val_ptrs[1], m->val_ptrs[1]);
    cester_assert_ptr_equal(ref.val_ptrs[2], m->val_ptrs[2]);
    cester_assert_ptr_equal(ref.val_ptrs[3], m->val_ptrs[3]);
    cester_assert_ptr_equal(ref.val_ptrs[4], m->val_ptrs[4]);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_on_center_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 6, 6, 4, 4, &ref);

    cester_assert_int_eq(ref.row_off, 6);
    cester_assert_int_eq(ref.col_off, 6);
    cester_assert_int_eq(ref.row_cnt, 4);
    cester_assert_int_eq(ref.col_cnt, 4);
    cester_assert_ptr_equal(ref.val_ptrs[0], m->val_ptrs[0]);
    cester_assert_ptr_equal(ref.val_ptrs[1], m->val_ptrs[1]);
    cester_assert_ptr_equal(ref.val_ptrs[2], m->val_ptrs[2]);
    cester_assert_ptr_equal(ref.val_ptrs[3], m->val_ptrs[3]);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_cross_right_boundary_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 0, 12, 10, 10, &ref);

    cester_assert_int_eq(ref.row_off, 0);
    cester_assert_int_eq(ref.col_off, 12);
    cester_assert_int_eq(ref.row_cnt, 10);
    cester_assert_int_eq(ref.col_cnt, 4);
    cester_assert_ptr_equal(ref.val_ptrs[0], m->val_ptrs[0]);
    cester_assert_ptr_equal(ref.val_ptrs[1], m->val_ptrs[1]);
    cester_assert_ptr_equal(ref.val_ptrs[2], m->val_ptrs[2]);
    cester_assert_ptr_equal(ref.val_ptrs[3], m->val_ptrs[3]);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_cross_bottom_boundary_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 15, 0, 10, 10, &ref);

    cester_assert_int_eq(ref.row_off, 15);
    cester_assert_int_eq(ref.col_off, 0);
    cester_assert_int_eq(ref.row_cnt, 1);
    cester_assert_int_eq(ref.col_cnt, 10);
    cester_assert_ptr_equal(ref.val_ptrs[0], m->val_ptrs[0]);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_beyond_right_boundary_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 0, 16, 10, 10, &ref);

    cester_assert_int_eq(ref.row_off, 0);
    cester_assert_int_eq(ref.col_off, 16);
    cester_assert_int_eq(ref.row_cnt, 0);
    cester_assert_int_eq(ref.col_cnt, 0);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_get_submatrix_beyond_bottom_boundary_of_16x16_matrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 16, 0, 10, 10, &ref);

    cester_assert_int_eq(ref.row_off, 16);
    cester_assert_int_eq(ref.col_off, 0);
    cester_assert_int_eq(ref.row_cnt, 0);
    cester_assert_int_eq(ref.col_cnt, 0);

    mtx_destroy(m);
)

CESTER_TEST(
    SUBMATRIX_i32_set_one_value_in_submatrix,
    _,
    ptr_matrix_t m = mtx_i32_allocate(16, 16);
    submatrix_t ref = {0};

    mtx_get_submatrix(m, 1, 1, 2, 2, &ref);
    smtx_i32_set(&ref, 0, 0, 5);
    smtx_i32_set(&ref, 0, 1, 6);
    smtx_i32_set(&ref, 1, 0, 7);
    smtx_i32_set(&ref, 1, 1, 8);

    cester_assert_int_eq(m->i32_vals[1][1], 5);
    cester_assert_int_eq(m->i32_vals[1][2], 6);
    cester_assert_int_eq(m->i32_vals[2][1], 7);
    cester_assert_int_eq(m->i32_vals[2][2], 8);

    mtx_destroy(m);
)

CESTER_TEST(
    MATRIX_transpose_and_store_from_5x1_to_1x5_matrix,
    _,
    submatrix_t ref = {0};
    ptr_matrix_t src = mtx_i32_allocate(5, 1);
    ptr_matrix_t m = mtx_allocate_for_transposing(src);

    mtx_initialize_zeros(src, MTX_PLAIN_CODE);
    mtx_get_submatrix(src, 2, 0, 2, 0, &ref);
    smtx_i32_set(&ref, 0, 0, 5);
    smtx_i32_set(&ref, 1, 0, 7);

    mtx_transpose_and_store(m, src);

    cester_assert_int_eq(m->row_cnt, 1);
    cester_assert_int_eq(m->col_cnt, 5);
    cester_assert_int_eq(m->i32_vals[0][0], 0);
    cester_assert_int_eq(m->i32_vals[0][1], 0);
    cester_assert_int_eq(m->i32_vals[0][2], 5);
    cester_assert_int_eq(m->i32_vals[0][3], 7);
    cester_assert_int_eq(m->i32_vals[0][4], 0);

    mtx_destroy(m);
    mtx_destroy(src);
)

CESTER_TEST(
    MATRIX_transpose_and_store_from_1x5_to_5x1_matrix,
    _,
    submatrix_t ref = {0};
    ptr_matrix_t src = mtx_i32_allocate(1, 5);
    ptr_matrix_t m = mtx_allocate_for_transposing(src);

    mtx_initialize_ones(src, MTX_PLAIN_CODE);
    mtx_get_submatrix(src, 0, 2, 0, 2, &ref);
    smtx_i32_set(&ref, 0, 0, 9);
    smtx_i32_set(&ref, 0, 1, 11);

    mtx_transpose_and_store(m, src);

    cester_assert_int_eq(m->row_cnt, 5);
    cester_assert_int_eq(m->col_cnt, 1);
    cester_assert_int_eq(m->i32_vals[0][0], 1);
    cester_assert_int_eq(m->i32_vals[1][0], 1);
    cester_assert_int_eq(m->i32_vals[2][0], 9);
    cester_assert_int_eq(m->i32_vals[3][0], 11);
    cester_assert_int_eq(m->i32_vals[4][0], 1);

    mtx_destroy(m);
    mtx_destroy(src);
)
