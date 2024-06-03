#include <exotic/cester.h>

#ifndef MATRIX_C
#include "matrix.c"
#define MATRIX_C
#endif

/* ==== Tests for int32_t matrix. ==== */

CESTER_TEST(
    MATRIX_i32_allocate_for_1x1_matrix,
    _,
    p_mi32_t m = mi32_allocate(1, 1);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 1);
    cester_assert_uint_eq(m->mt.cols, 1);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);

    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_1x2_matrix,
    _,
    p_mi32_t m = mi32_allocate(1, 2);
    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 1);
    cester_assert_uint_eq(m->mt.cols, 2);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_1x5_matrix,
    _,
    p_mi32_t m = mi32_allocate(1, 5);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#endif

    cester_assert_uint_eq(m->mt.rows, 1);
    cester_assert_uint_eq(m->mt.cols, 5);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));

    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_2x1_matrix,
    _,
    p_mi32_t m = mi32_allocate(2, 1);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 2);
    cester_assert_uint_eq(m->mt.cols, 1);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_6x1_matrix,
    _,
    p_mi32_t m = mi32_allocate(6, 1);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 6);
    cester_assert_uint_eq(m->mt.cols, 1);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_4x4_matrix,
    _,
    p_mi32_t m = mi32_allocate(4, 4);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#endif

    cester_assert_uint_eq(m->mt.rows, 4);
    cester_assert_uint_eq(m->mt.cols, 4);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_7x7_matrix,
    _,
    p_mi32_t m = mi32_allocate(7, 7);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#endif

    cester_assert_uint_eq(m->mt.rows, 7);
    cester_assert_uint_eq(m->mt.cols, 7);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_8x8_matrix,
    _,
    p_mi32_t m = mi32_allocate(8, 8);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#endif

    cester_assert_uint_eq(m->mt.rows, 8);
    cester_assert_uint_eq(m->mt.cols, 8);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_allocate_for_1024x1024_matrix,
    _,
    p_mi32_t m = mi32_allocate(1024, 1024);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 128);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 256);
#endif

    cester_assert_uint_eq(m->mt.rows, 1024);
    cester_assert_uint_eq(m->mt.cols, 1024);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);
    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_set_and_get_one_value_in_5x5_matrix,
    _,
    int32_t val1 = 0;
    int32_t val2 = 0;
    p_mi32_t m = mi32_allocate(5, 5);

    mi32_set(m, 0, 0, 1);
    mi32_set(m, 4, 4, 5);

    val1 = mi32_get(m, 0, 0);
    val2 = mi32_get(m, 4, 4);

    cester_assert_uint_eq(m->i32_vals[0][0], 1);
    cester_assert_uint_eq(m->i32_vals[4][4], 5);
    cester_assert_uint_eq(val1, 1);
    cester_assert_uint_eq(val2, 5);

    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_i32_set_each_value_to_5_in_2x2_matrix,
    _,
    int32_t val = 0;
    p_mi32_t m = mi32_allocate(2, 2);

    mi32_set_each(m, 5);

    val = mi32_get(m, 0, 0);
    cester_assert_uint_eq(val, 5);
    val = mi32_get(m, 0, 1);
    cester_assert_uint_eq(val, 5);
    val = mi32_get(m, 1, 0);
    cester_assert_uint_eq(val, 5);
    val = mi32_get(m, 1, 1);
    cester_assert_uint_eq(val, 5);

    mi32_destroy(m);
)

/* ==== Tests for common functions. ==== */

CESTER_TEST(
    MATRIX_allocate_for_multiplying_5x1_and_1x5_matrix,
    _,
    p_mi32_t lhs = mi32_allocate(5, 1);
    p_mi32_t rhs = mi32_allocate(1, 5);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#endif

    cester_assert_uint_eq(m->mt.rows, 5);
    cester_assert_uint_eq(m->mt.cols, 5);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, lhs);
    cester_assert_ptr_not_equal(m, rhs);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_allocate_for_multiplying_1x5_and_5x1_matrix,
    _,
    p_mi32_t lhs = mi32_allocate(1, 5);
    p_mi32_t rhs = mi32_allocate(5, 1);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 1);
    cester_assert_uint_eq(m->mt.cols, 1);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, lhs);
    cester_assert_ptr_not_equal(m, rhs);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_allocate_for_multiplying_8x8_and_8x8_matrix,
    _,

    p_mi32_t lhs = mi32_allocate(8, 8);
    p_mi32_t rhs = mi32_allocate(8, 8);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#endif

    cester_assert_uint_eq(m->mt.rows, 8);
    cester_assert_uint_eq(m->mt.cols, 8);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, lhs);
    cester_assert_ptr_not_equal(m, rhs);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_allocate_for_transposing_5x1_matrix,
    _,

    p_mi32_t src = mi32_allocate(5, 1);
    p_mi32_t m = mi32_allocate_for_transposing(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#endif

    cester_assert_uint_eq(m->mt.rows, 1);
    cester_assert_uint_eq(m->mt.cols, 5);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, src);

    mi32_destroy(m);
    mi32_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_for_transposing_1x5_matrix,
    _,

    p_mi32_t src = mi32_allocate(1, 5);
    p_mi32_t m = mi32_allocate_for_transposing(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 5);
    cester_assert_uint_eq(m->mt.cols, 1);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, src);

    mi32_destroy(m);
    mi32_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_for_transposing_13x13_matrix,
    _,

    p_mi32_t src = mi32_allocate(13, 13);
    p_mi32_t m = mi32_allocate_for_transposing(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 4);
#endif

    cester_assert_uint_eq(m->mt.rows, 13);
    cester_assert_uint_eq(m->mt.cols, 13);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, src);

    mi32_destroy(m);
    mi32_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_in_shape_of_3x1_matrix,
    _,

    p_mi32_t src = mi32_allocate(3, 1);
    p_mi32_t m = mi32_allocate_in_shape_of(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 3);
    cester_assert_uint_eq(m->mt.cols, 1);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, src);

    mi32_destroy(m);
    mi32_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_in_shape_of_1x3_matrix,
    _,
    p_mi32_t src = mi32_allocate(1, 3);
    p_mi32_t m = mi32_allocate_in_shape_of(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 1);
    cester_assert_uint_eq(m->mt.cols, 3);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, src);

    mi32_destroy(m);
    mi32_destroy(src);
)

CESTER_TEST(
    MATRIX_allocate_in_shape_of_3x3_matrix,
    _,

    p_mi32_t src = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_in_shape_of(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
#endif

    cester_assert_uint_eq(m->mt.rows, 3);
    cester_assert_uint_eq(m->mt.cols, 3);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_uint_eq(m->mt.pcks_in_row, 1);
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, src);

    mi32_destroy(m);
    mi32_destroy(src);
)

CESTER_TEST(
    MATRIX_duplicate_10x10_matrix_of_random_values,
    _,

    p_mi32_t src = mi32_allocate(10, 10);
    p_mi32_t m = mi32_duplicate(src);

    cester_assert_not_null(m);
    cester_assert_uint_eq(m->mt.val_size, sizeof(int32_t));

#if defined(MX_AVX2)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V8SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 2);
#elif defined(MX_SSE41)
    cester_assert_uint_eq(m->mt.vals_in_pck, I32_VALS_IN_V4SI);
    cester_assert_uint_eq(m->mt.pcks_in_row, 3);
#endif

    cester_assert_uint_eq(m->mt.rows, 10);
    cester_assert_uint_eq(m->mt.cols, 10);
    cester_assert_uint_eq(m->mt.bytes, m->mt.val_size * m->mt.rows * round_to_multiples(m->mt.cols, m->mt.vals_in_pck));
    cester_assert_not_null(m->mt.buf);

    cester_assert_ptr_not_equal(m, src);

    cester_assert_int_eq(m->i32_vals[0][0], src->i32_vals[0][0]);
    cester_assert_int_eq(m->i32_vals[0][1], src->i32_vals[0][1]);
    cester_assert_int_eq(m->i32_vals[1][0], src->i32_vals[1][0]);
    cester_assert_int_eq(m->i32_vals[1][1], src->i32_vals[1][1]);
    cester_assert_int_eq(m->i32_vals[8][9], src->i32_vals[8][9]);
    cester_assert_int_eq(m->i32_vals[9][8], src->i32_vals[9][8]);
    cester_assert_int_eq(m->i32_vals[9][9], src->i32_vals[9][9]);

    mi32_destroy(m);
    mi32_destroy(src);
)

CESTER_TEST(
    MATRIX_initialize_15x15_identity_matrix_using_plain_code,
    _,
    size_t i = 0;
    size_t j = 0;
    p_mi32_t m = mi32_allocate(15, 15);

    mi32_initialize_identity(m, MX_PLAIN_CODE);

    for (i = 0; i < m->mt.rows; i += 1) {
        for (j = 0; j < m->mt.cols; j += 1) {
            if (i == j) {
                cester_assert_int_eq(m->i32_vals[i][j], 1);
            } else {
                cester_assert_int_eq(m->i32_vals[i][j], 0);
            } /* if */
        } /* for */
    } /* for */

    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_initialize_9x5_zeros_matrix_using_plain_code,
    _,
    size_t i = 0;
    size_t j = 0;
    p_mi32_t m = mi32_allocate(9, 5);

    mi32_initialize_zeros(m, MX_PLAIN_CODE);

    for (i = 0; i < m->mt.rows; i += 1) {
        for (j = 0; j < m->mt.cols; j += 1) {
            cester_assert_int_eq(m->i32_vals[i][j], 0);
        } /* for */
    } /* for */

    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_initialize_5x9_ones_matrix_using_plain_code,
    _,
    size_t i = 0;
    size_t j = 0;
    p_mi32_t m = mi32_allocate(5, 9);

    mi32_initialize_ones(m, MX_PLAIN_CODE);

    for (i = 0; i < m->mt.rows; i += 1) {
        for (j = 0; j < m->mt.cols; j += 1) {
            cester_assert_int_eq(m->i32_vals[i][j], 1);
        } /* for */
    } /* for */

    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_can_do_add_btween_2x2_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(2, 2);
    p_mi32_t rhs = mi32_allocate(2, 2);
    int ret = mi32_can_do_add(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_add_btween_2x5_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(2, 5);
    p_mi32_t rhs = mi32_allocate(2, 5);
    int ret = mi32_can_do_add(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_cannot_do_add_btween_2x2_and_3x3_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(2, 2);
    p_mi32_t rhs = mi32_allocate(3, 3);
    int ret = mi32_can_do_add(lhs, rhs);

    cester_assert_int_eq(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_sub_btween_3x3_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(3, 3);
    p_mi32_t rhs = mi32_allocate(3, 3);
    int ret = mi32_can_do_sub(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_sub_btween_5x2_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(5, 2);
    p_mi32_t rhs = mi32_allocate(5, 2);
    int ret = mi32_can_do_sub(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_cannot_do_sub_btween_1x7_and_7x1_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(1, 7);
    p_mi32_t rhs = mi32_allocate(7, 1);
    int ret = mi32_can_do_sub(lhs, rhs);

    cester_assert_int_eq(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_mul_btween_13x13_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(13, 13);
    p_mi32_t rhs = mi32_allocate(13, 13);
    int ret = mi32_can_do_multiply(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_mul_btween_2x4_and_4x5_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(2, 4);
    p_mi32_t rhs = mi32_allocate(4, 5);
    int ret = mi32_can_do_multiply(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_can_do_mul_btween_5x3_and_3x2_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(5, 3);
    p_mi32_t rhs = mi32_allocate(3, 2);
    int ret = mi32_can_do_multiply(lhs, rhs);

    cester_assert_int_ne(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_cannot_do_mul_btween_5x7_and_8x2_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(5, 7);
    p_mi32_t rhs = mi32_allocate(8, 2);
    int ret = mi32_can_do_multiply(lhs, rhs);

    cester_assert_int_eq(ret, 0);

    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_count_rows_columns_and_values_for_3x7_matrix,
    _,
    p_mi32_t m = mi32_allocate(3, 7);

    cester_assert_int_eq(mi32_rows(m), 3);
    cester_assert_int_eq(mi32_columns(m), 7);
    cester_assert_int_eq(mi32_values(m), 21);

    mi32_destroy(m);
)

CESTER_TEST(
    MATRIX_add_and_store_3x3_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(3, 3);
    p_mi32_t rhs = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_in_shape_of(lhs);

    mi32_initialize_ones(lhs, MX_PLAIN_CODE);
    mi32_set_each(rhs, 5);

    mi32_add_and_store(m, lhs, rhs, MX_PLAIN_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 6);
    cester_assert_int_eq(m->i32_vals[0][1], 6);
    cester_assert_int_eq(m->i32_vals[0][2], 6);
    cester_assert_int_eq(m->i32_vals[1][0], 6);
    cester_assert_int_eq(m->i32_vals[1][1], 6);
    cester_assert_int_eq(m->i32_vals[1][2], 6);
    cester_assert_int_eq(m->i32_vals[2][0], 6);
    cester_assert_int_eq(m->i32_vals[2][1], 6);
    cester_assert_int_eq(m->i32_vals[2][2], 6);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_sub_and_store_3x3_matrices,
    _,
    p_mi32_t lhs = mi32_allocate(3, 3);
    p_mi32_t rhs = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_in_shape_of(lhs);

    mi32_set_each(lhs, 5);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);

    mi32_sub_and_store(m, lhs, rhs, MX_PLAIN_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 4);
    cester_assert_int_eq(m->i32_vals[0][1], 4);
    cester_assert_int_eq(m->i32_vals[0][2], 4);
    cester_assert_int_eq(m->i32_vals[1][0], 4);
    cester_assert_int_eq(m->i32_vals[1][1], 4);
    cester_assert_int_eq(m->i32_vals[1][2], 4);
    cester_assert_int_eq(m->i32_vals[2][0], 4);
    cester_assert_int_eq(m->i32_vals[2][1], 4);
    cester_assert_int_eq(m->i32_vals[2][2], 4);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_3x3_matrices_using_plain_code,
    _,
    p_mi32_t lhs = mi32_allocate(3, 3);
    p_mi32_t rhs = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    mi32_set_each(lhs, 5);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);

    mi32_multiply_and_store(m, lhs, rhs, MX_PLAIN_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 15);
    cester_assert_int_eq(m->i32_vals[0][1], 15);
    cester_assert_int_eq(m->i32_vals[0][2], 15);
    cester_assert_int_eq(m->i32_vals[1][0], 15);
    cester_assert_int_eq(m->i32_vals[1][1], 15);
    cester_assert_int_eq(m->i32_vals[1][2], 15);
    cester_assert_int_eq(m->i32_vals[2][0], 15);
    cester_assert_int_eq(m->i32_vals[2][1], 15);
    cester_assert_int_eq(m->i32_vals[2][2], 15);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_3x1_and_1x3_matrices_using_plain_code,
    _,
    p_mi32_t lhs = mi32_allocate(3, 1);
    p_mi32_t rhs = mi32_allocate(1, 3);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    mi32_set_each(lhs, 5);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);

    mi32_multiply_and_store(m, lhs, rhs, MX_PLAIN_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 5);
    cester_assert_int_eq(m->i32_vals[0][1], 5);
    cester_assert_int_eq(m->i32_vals[0][2], 5);
    cester_assert_int_eq(m->i32_vals[1][0], 5);
    cester_assert_int_eq(m->i32_vals[1][1], 5);
    cester_assert_int_eq(m->i32_vals[1][2], 5);
    cester_assert_int_eq(m->i32_vals[2][0], 5);
    cester_assert_int_eq(m->i32_vals[2][1], 5);
    cester_assert_int_eq(m->i32_vals[2][2], 5);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_1x3_and_3x1_matrices_using_plain_code,
    _,
    p_mi32_t lhs = mi32_allocate(1, 3);
    p_mi32_t rhs = mi32_allocate(3, 1);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    mi32_set_each(lhs, 5);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);

    mi32_multiply_and_store(m, lhs, rhs, MX_PLAIN_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 15);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_3x3_matrices_using_simd_code,
    _,
    p_mi32_t lhs = mi32_allocate(3, 3);
    p_mi32_t rhs = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    mi32_set_each(lhs, 5);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);

    mi32_multiply_and_store(m, lhs, rhs, MX_SIMD_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 15);
    cester_assert_int_eq(m->i32_vals[0][1], 15);
    cester_assert_int_eq(m->i32_vals[0][2], 15);
    cester_assert_int_eq(m->i32_vals[1][0], 15);
    cester_assert_int_eq(m->i32_vals[1][1], 15);
    cester_assert_int_eq(m->i32_vals[1][2], 15);
    cester_assert_int_eq(m->i32_vals[2][0], 15);
    cester_assert_int_eq(m->i32_vals[2][1], 15);
    cester_assert_int_eq(m->i32_vals[2][2], 15);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_3x1_and_1x3_matrices_using_simd_code,
    _,
    p_mi32_t lhs = mi32_allocate(3, 1);
    p_mi32_t rhs = mi32_allocate(1, 3);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    mi32_set_each(lhs, 5);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);

    mi32_multiply_and_store(m, lhs, rhs, MX_SIMD_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 5);
    cester_assert_int_eq(m->i32_vals[0][1], 5);
    cester_assert_int_eq(m->i32_vals[0][2], 5);
    cester_assert_int_eq(m->i32_vals[1][0], 5);
    cester_assert_int_eq(m->i32_vals[1][1], 5);
    cester_assert_int_eq(m->i32_vals[1][2], 5);
    cester_assert_int_eq(m->i32_vals[2][0], 5);
    cester_assert_int_eq(m->i32_vals[2][1], 5);
    cester_assert_int_eq(m->i32_vals[2][2], 5);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_1x3_and_3x1_matrices_using_simd_code,
    _,
    p_mi32_t lhs = mi32_allocate(1, 3);
    p_mi32_t rhs = mi32_allocate(3, 1);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    mi32_set_each(lhs, 5);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);

    mi32_multiply_and_store(m, lhs, rhs, MX_SIMD_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 15);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_1023x1023_matrices_using_simd_code,
    _,
    p_mi32_t lhs = mi32_allocate(1023, 1023);
    p_mi32_t rhs = mi32_allocate(1023, 1023);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    mi32_set_each(lhs, 10);
    mi32_initialize_ones(rhs, MX_PLAIN_CODE);
    rhs->i32_vals[0][0] = 2;
    rhs->i32_vals[1022][1022] = 2;

    mi32_multiply_and_store(m, lhs, rhs, MX_SIMD_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 10240);
    cester_assert_int_eq(m->i32_vals[0][1], 10230);
    cester_assert_int_eq(m->i32_vals[0][2], 10230);
    cester_assert_int_eq(m->i32_vals[0][1020], 10230);
    cester_assert_int_eq(m->i32_vals[0][1021], 10230);
    cester_assert_int_eq(m->i32_vals[0][1022], 10240);
    cester_assert_int_eq(m->i32_vals[1][0], 10240);
    cester_assert_int_eq(m->i32_vals[1][1], 10230);
    cester_assert_int_eq(m->i32_vals[1][2], 10230);
    cester_assert_int_eq(m->i32_vals[1][1020], 10230);
    cester_assert_int_eq(m->i32_vals[1][1021], 10230);
    cester_assert_int_eq(m->i32_vals[1][1022], 10240);
    cester_assert_int_eq(m->i32_vals[1022][1020], 10230);
    cester_assert_int_eq(m->i32_vals[1022][1021], 10230);
    cester_assert_int_eq(m->i32_vals[1022][1022], 10240);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_mul_and_store_3x3_matrices_of_different_numbers_using_simd_code,
    _,
    p_mi32_t lhs = mi32_allocate(3, 3);
    p_mi32_t rhs = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);

    /* lhs = [  1 2 3
                4 5 6
                7 8 9   ]
    */

    lhs->i32_vals[0][0] = 1;
    lhs->i32_vals[0][1] = 2;
    lhs->i32_vals[0][2] = 3;
    lhs->i32_vals[1][0] = 4;
    lhs->i32_vals[1][1] = 5;
    lhs->i32_vals[1][2] = 6;
    lhs->i32_vals[2][0] = 7;
    lhs->i32_vals[2][1] = 8;
    lhs->i32_vals[2][2] = 9;

    /* rhs = [  9 8 7
                6 5 4
                3 2 1   ]
    */
    rhs->i32_vals[0][0] = 9;
    rhs->i32_vals[0][1] = 8;
    rhs->i32_vals[0][2] = 7;
    rhs->i32_vals[1][0] = 6;
    rhs->i32_vals[1][1] = 5;
    rhs->i32_vals[1][2] = 4;
    rhs->i32_vals[2][0] = 3;
    rhs->i32_vals[2][1] = 2;
    rhs->i32_vals[2][2] = 1;

    /* ret = [   30  24  18
                 84  69  54
                138 114  90   ]
    */
    mi32_multiply_and_store(m, lhs, rhs, MX_SIMD_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 30);
    cester_assert_int_eq(m->i32_vals[0][1], 24);
    cester_assert_int_eq(m->i32_vals[0][2], 18);
    cester_assert_int_eq(m->i32_vals[1][0], 84);
    cester_assert_int_eq(m->i32_vals[1][1], 69);
    cester_assert_int_eq(m->i32_vals[1][2], 54);
    cester_assert_int_eq(m->i32_vals[2][0], 138);
    cester_assert_int_eq(m->i32_vals[2][1], 114);
    cester_assert_int_eq(m->i32_vals[2][2], 90);

    mi32_destroy(m);
    mi32_destroy(rhs);
    mi32_destroy(lhs);
)

CESTER_TEST(
    MATRIX_scalar_mul_and_store_3x3_matrix_using_plain_code,
    _,
    int32_t lhs = 5;
    p_mi32_t rhs = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_in_shape_of(rhs);

    mi32_set_each(rhs, 5);

    mi32_scalar_multiply_and_store(m, lhs, rhs, MX_PLAIN_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 25);
    cester_assert_int_eq(m->i32_vals[0][1], 25);
    cester_assert_int_eq(m->i32_vals[0][2], 25);
    cester_assert_int_eq(m->i32_vals[1][0], 25);
    cester_assert_int_eq(m->i32_vals[1][1], 25);
    cester_assert_int_eq(m->i32_vals[1][2], 25);
    cester_assert_int_eq(m->i32_vals[2][0], 25);
    cester_assert_int_eq(m->i32_vals[2][1], 25);
    cester_assert_int_eq(m->i32_vals[2][2], 25);

    mi32_destroy(m);
    mi32_destroy(rhs);
)

CESTER_TEST(
    MATRIX_scalar_mul_and_store_11x7_matrix_using_plain_code,
    _,
    int32_t lhs = 2;
    p_mi32_t rhs = mi32_allocate(11, 7);
    p_mi32_t m = mi32_allocate_in_shape_of(rhs);

    mi32_initialize_ones(rhs, MX_PLAIN_CODE);
    rhs->i32_vals[0][0] = 1;
    rhs->i32_vals[0][1] = 2;
    rhs->i32_vals[0][2] = 3;
    rhs->i32_vals[1][0] = 4;
    rhs->i32_vals[1][1] = 5;
    rhs->i32_vals[1][2] = 6;
    rhs->i32_vals[2][0] = 7;
    rhs->i32_vals[2][1] = 8;
    rhs->i32_vals[2][2] = 9;
    rhs->i32_vals[10][6] = 10;

    mi32_scalar_multiply_and_store(m, lhs, rhs, MX_PLAIN_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 2);
    cester_assert_int_eq(m->i32_vals[0][1], 4);
    cester_assert_int_eq(m->i32_vals[0][2], 6);
    cester_assert_int_eq(m->i32_vals[1][0], 8);
    cester_assert_int_eq(m->i32_vals[1][1], 10);
    cester_assert_int_eq(m->i32_vals[1][2], 12);
    cester_assert_int_eq(m->i32_vals[2][0], 14);
    cester_assert_int_eq(m->i32_vals[2][1], 16);
    cester_assert_int_eq(m->i32_vals[2][2], 18);
    cester_assert_int_eq(m->i32_vals[5][6], 2);
    cester_assert_int_eq(m->i32_vals[10][5], 2);
    cester_assert_int_eq(m->i32_vals[10][6], 20);

    mi32_destroy(m);
    mi32_destroy(rhs);
)

CESTER_TEST(
    MATRIX_scalar_mul_and_store_3x3_matrix_using_simd_code,
    _,
    int32_t lhs = 2;
    p_mi32_t rhs = mi32_allocate(3, 3);
    p_mi32_t m = mi32_allocate_in_shape_of(rhs);

    mi32_initialize_zeros(rhs, MX_PLAIN_CODE);
    rhs->i32_vals[0][0] = 1;
    rhs->i32_vals[0][1] = 2;
    rhs->i32_vals[0][2] = 3;
    rhs->i32_vals[1][0] = 4;
    rhs->i32_vals[1][1] = 5;
    rhs->i32_vals[1][2] = 6;
    rhs->i32_vals[2][0] = 7;
    rhs->i32_vals[2][1] = 8;
    rhs->i32_vals[2][2] = 9;

    mi32_scalar_multiply_and_store(m, lhs, rhs, MX_SIMD_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 2);
    cester_assert_int_eq(m->i32_vals[0][1], 4);
    cester_assert_int_eq(m->i32_vals[0][2], 6);
    cester_assert_int_eq(m->i32_vals[1][0], 8);
    cester_assert_int_eq(m->i32_vals[1][1], 10);
    cester_assert_int_eq(m->i32_vals[1][2], 12);
    cester_assert_int_eq(m->i32_vals[2][0], 14);
    cester_assert_int_eq(m->i32_vals[2][1], 16);
    cester_assert_int_eq(m->i32_vals[2][2], 18);

    mi32_destroy(m);
    mi32_destroy(rhs);
)

CESTER_TEST(
    MATRIX_scalar_mul_and_store_11x7_matrix_using_simd_code,
    _,
    int32_t lhs = 2;
    p_mi32_t rhs = mi32_allocate(11, 7);
    p_mi32_t m = mi32_allocate_in_shape_of(rhs);

    mi32_initialize_ones(rhs, MX_PLAIN_CODE);
    rhs->i32_vals[0][0] = 1;
    rhs->i32_vals[0][1] = 2;
    rhs->i32_vals[0][2] = 3;
    rhs->i32_vals[1][0] = 4;
    rhs->i32_vals[1][1] = 5;
    rhs->i32_vals[1][2] = 6;
    rhs->i32_vals[2][0] = 7;
    rhs->i32_vals[2][1] = 8;
    rhs->i32_vals[2][2] = 9;
    rhs->i32_vals[10][6] = 10;

    mi32_scalar_multiply_and_store(m, lhs, rhs, MX_SIMD_CODE);

    cester_assert_int_eq(m->i32_vals[0][0], 2);
    cester_assert_int_eq(m->i32_vals[0][1], 4);
    cester_assert_int_eq(m->i32_vals[0][2], 6);
    cester_assert_int_eq(m->i32_vals[1][0], 8);
    cester_assert_int_eq(m->i32_vals[1][1], 10);
    cester_assert_int_eq(m->i32_vals[1][2], 12);
    cester_assert_int_eq(m->i32_vals[2][0], 14);
    cester_assert_int_eq(m->i32_vals[2][1], 16);
    cester_assert_int_eq(m->i32_vals[2][2], 18);
    cester_assert_int_eq(m->i32_vals[5][6], 2);
    cester_assert_int_eq(m->i32_vals[10][5], 2);
    cester_assert_int_eq(m->i32_vals[10][6], 20);

    mi32_destroy(m);
    mi32_destroy(rhs);
)
