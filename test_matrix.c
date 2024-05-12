#include <exotic/cester.h>

#include "matrix.h"

CESTER_TEST(
    test_mtx_i32_allocate,
    _,
    ptr_matrix_t mtx = mtx_i32_allocate(1, 1);
    cester_assert_not_null(mtx);
    mtx_destroy(mtx);
)
