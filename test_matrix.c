#include <exotic/cester.h>

#ifndef MATRIX_C
#include "matrix.c"
#define MATRIX_C
#endif

/* ==== Tests for mtx_int32_t matrix. ==== */

CESTER_TEST(
    test_mtx_i32_allocate_for_1x1_matrix,
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
    test_mtx_i32_allocate_for_1x2_matrix,
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
    test_mtx_i32_allocate_for_1x5_matrix,
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
    test_mtx_i32_allocate_for_2x1_matrix,
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
    test_mtx_i32_allocate_for_6x1_matrix,
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
    test_mtx_i32_allocate_for_4x4_matrix,
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
    test_mtx_i32_allocate_for_7x7_matrix,
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
    test_mtx_i32_allocate_for_8x8_matrix,
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
    test_mtx_i32_allocate_for_1024x1024_matrix,
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

/* ==== Common functions. ==== */

CESTER_TEST(
    test_mtx_allocate_for_multiplying_5x1_and_1x5_matrix,
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
    test_mtx_allocate_for_multiplying_1x5_and_5x1_matrix,
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
    test_mtx_allocate_for_multiplying_8x8_and_8x8_matrix,
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
    test_mtx_allocate_for_transposing_5x1_matrix,
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
    test_mtx_allocate_for_transposing_1x5_matrix,
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
    test_mtx_allocate_for_transposing_13x13_matrix,
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
    test_mtx_allocate_in_shape_of_3x1_matrix,
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
    test_mtx_allocate_in_shape_of_1x3_matrix,
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
    test_mtx_allocate_in_shape_of_3x3_matrix,
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
    test_mtx_duplicate_10x10_matrix_of_random_values,
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
    test_mtx_initialize_15x15_identity_matrix_using_plain_code,
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
    test_mtx_initialize_9x5_zeros_matrix_using_plain_code,
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
    test_mtx_initialize_5x9_ones_matrix_using_plain_code,
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
