#include <exotic/cester.h>

#ifndef MATRIX_C
#include "matrix.c"
#define MATRIX_C
#endif

/* ==== Tests for mtx_int32_t matrix. ==== */

CESTER_TEST(
    test_mtx_i32_allocate_for_1x1,
    _,
    ptr_matrix_t m = mtx_i32_allocate(1, 1);
    cester_assert_not_null(m);
    cester_assert_uint_eq(m->value_size, sizeof(mtx_int32_t));
    cester_assert_uint_eq(m->pack_len, I32_PACK_LEN);

    cester_assert_uint_eq(m->row_cnt, 1);
    cester_assert_uint_eq(m->col_cnt, 1);
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN);
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
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN);
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
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN);
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
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN);
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
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN);
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
    cester_assert_uint_eq(m->padded_row_cnt, I32_PACK_LEN);
    cester_assert_uint_eq(m->padded_col_cnt, I32_PACK_LEN);
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
