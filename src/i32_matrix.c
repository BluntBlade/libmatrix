#include <omp.h>
#include <memory.h>
#include <smmintrin.h>
#include <stdio.h>

#include "src/v8si_storage.h"
#include "src/v8si_operation.h"
#include "src/i32_matrix.h"

typedef struct I32_MATRIX
{
    mx_stor_t stor;
} i32_matrix_t;

i32_matrix_ptr i32m_allocate(uint32_t rows, uint32_t cols)
{
    i32_matrix_ptr mx = calloc(sizeof(i32_matrix_t), 1);
    if (! mx) {
        return NULL;
    } // if
    mstr_v8si_init(&mx->stor, rows, cols);
    return mx;
} // i32m_allocate

i32_matrix_ptr i32m_allocate_for_multiplying(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    return i32m_allocate(lhs->stor.rows, rhs->stor.cols);
} // i32m_allocate_for_multiplying

i32_matrix_ptr i32m_allocate_for_transposing(i32_matrix_ptr src)
{
    return i32m_allocate(src->stor.cols, src->stor.rows);
} // i32m_allocate_for_transposing

i32_matrix_ptr i32m_allocate_in_shape_of(i32_matrix_ptr src)
{
    return i32m_allocate(src->stor.rows, src->stor.cols);
} // i32m_allocate_in_shape_of

i32_matrix_ptr i32m_duplicate(i32_matrix_ptr src)
{
    i32_matrix_ptr mx = i32m_allocate_in_shape_of(src);
    if (! mx) {
        return NULL;
    } // if
    memcpy(mx->stor.data, src->stor.data, src->stor.bytes);
    return mx;
} // i32m_duplicate

void i32m_destroy(i32_matrix_ptr mx)
{
    if (mx) {
        mstr_clean(&mx->stor);
    } // if
    free(mx);
} // i32m_destroy

void i32m_init_identity(i32_matrix_ptr mx)
{
    mstr_v8si_init_identity(&mx->stor);
} // i32m_init_identity

void i32m_init_zeros(i32_matrix_ptr mx)
{
    mstr_v8si_init_zeros(&mx->stor);
} // i32m_init_zeros

void i32m_init_ones(i32_matrix_ptr mx)
{
    mstr_v8si_init_ones(&mx->stor);
} // i32m_init_ones

uint32_t i32m_rows(i32_matrix_ptr mx)
{
    return mstr_v8si_rows(&mx->stor);
} // i32m_rows

uint32_t i32m_columns(i32_matrix_ptr mx)
{
    return mstr_v8si_columns(&mx->stor);
} // i32m_columns

uint32_t i32m_values(i32_matrix_ptr mx)
{
    return mstr_v8si_values(&mx->stor);
} // i32m_values

int32_t i32m_get(i32_matrix_ptr mx, uint32_t row, uint32_t col)
{
    return mstr_v8si_get(&mx->stor, row, col);
} // i32m_get

void i32m_set(i32_matrix_ptr mx, uint32_t row, uint32_t col, int32_t src)
{
    mstr_v8si_set(&mx->stor, row, col, src);
} // i32m_set

void i32m_fill(i32_matrix_ptr mx, int32_t src)
{
    mstr_v8si_fill(&mx->stor, src);
} // i32m_fill

bool i32m_can_add(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    return (mstr_v8si_rows(&lhs->stor) == mstr_v8si_rows(&rhs->stor)) && (mstr_v8si_columns(&lhs->stor) == mstr_v8si_columns(&rhs->stor));
} // i32m_can_add

bool i32m_can_multiply(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    return (mstr_v8si_columns(&lhs->stor) == mstr_v8si_rows(&rhs->stor));
} // i32m_can_multiply

void i32m_transpose_and_store(i32_matrix_ptr mx, i32_matrix_ptr src)
{
    mops_v8si_transpose(&mx->stor, &src->stor);
} // i32m_transpose_and_store

void i32m_add_and_store(i32_matrix_ptr mx, i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    mops_v8si_add(&mx->stor, &lhs->stor, &rhs->stor);
} // i32m_add_and_store

void i32m_subtract_and_store(i32_matrix_ptr mx, i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    mops_v8si_subtract(&mx->stor, &lhs->stor, &rhs->stor);
} // i32m_subtract_and_store

void i32m_multiply_and_store(i32_matrix_ptr mx, i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    mops_v8si_multiply(&mx->stor, &lhs->stor, &rhs->stor);
} // i32m_multiply_and_store

void i32m_multiply_scalar_and_store(i32_matrix_ptr mx, i32_matrix_ptr src, int32_t val)
{
    mops_v8si_multiply_scalar(&mx->stor, &src->stor, val);
} // i32m_multiply_scalar_and_store
