#include <stdlib.h>
#include <string.h>

#include "src/mx_v8sf.h"
#include "src/f32_matrix.h"

typedef struct F32_MATRIX
{
    mx_stor_t stor;
} f32_matrix_t;

f32_matrix_ptr f32m_allocate(uint32_t rows, uint32_t cols)
{
    f32_matrix_ptr mx = calloc(sizeof(f32_matrix_t), 1);
    if (! mx) {
        return NULL;
    } // if
    mstr_v8sf_init(&mx->stor, rows, cols);
    return mx;
} // f32m_allocate

f32_matrix_ptr f32m_allocate_for_multiplying(f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    return f32m_allocate(lhs->stor.rows, rhs->stor.cols);
} // f32m_allocate_for_multiplying

f32_matrix_ptr f32m_allocate_for_transposing(f32_matrix_ptr src)
{
    return f32m_allocate(src->stor.cols, src->stor.rows);
} // f32m_allocate_for_transposing

f32_matrix_ptr f32m_allocate_in_shape_of(f32_matrix_ptr src)
{
    return f32m_allocate(src->stor.rows, src->stor.cols);
} // f32m_allocate_in_shape_of

f32_matrix_ptr f32m_duplicate(f32_matrix_ptr src)
{
    f32_matrix_ptr mx = f32m_allocate_in_shape_of(src);
    if (! mx) {
        return NULL;
    } // if
    memcpy(mx->stor.data, src->stor.data, src->stor.bytes);
    return mx;
} // f32m_duplicate

void f32m_destroy(f32_matrix_ptr mx)
{
    if (mx) {
        mstr_clean(&mx->stor);
    } // if
    free(mx);
} // f32m_destroy

void f32m_init_identity(f32_matrix_ptr mx)
{
    mstr_v8sf_init_identity(&mx->stor);
} // f32m_init_identity

void f32m_init_zeros(f32_matrix_ptr mx)
{
    mstr_v8sf_init_zeros(&mx->stor);
} // f32m_init_zeros

void f32m_init_ones(f32_matrix_ptr mx)
{
    mstr_v8sf_init_ones(&mx->stor);
} // f32m_init_ones

uint32_t f32m_rows(f32_matrix_ptr mx)
{
    return mstr_rows(&mx->stor);
} // f32m_rows

uint32_t f32m_columns(f32_matrix_ptr mx)
{
    return mstr_columns(&mx->stor);
} // f32m_columns

uint32_t f32m_values(f32_matrix_ptr mx)
{
    return mstr_values(&mx->stor);
} // f32m_values

float f32m_get(f32_matrix_ptr mx, uint32_t row, uint32_t col)
{
    return mstr_get_f32(&mx->stor, row, col);
} // f32m_get

void f32m_set(f32_matrix_ptr mx, uint32_t row, uint32_t col, float src)
{
    mstr_set_f32(&mx->stor, row, col, src);
} // f32m_set

void f32m_fill(f32_matrix_ptr mx, float src)
{
    mstr_v8sf_fill(&mx->stor, src);
} // f32m_fill

bool f32m_can_add(f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    return (mstr_rows(&lhs->stor) == mstr_rows(&rhs->stor)) && (mstr_columns(&lhs->stor) == mstr_columns(&rhs->stor));
} // f32m_can_add

bool f32m_can_multiply(f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    return (mstr_columns(&lhs->stor) == mstr_rows(&rhs->stor));
} // f32m_can_multiply

void f32m_transpose_and_store(f32_matrix_ptr mx, f32_matrix_ptr src)
{
    mstr_v8sf_transpose(&mx->stor, &src->stor);
} // f32m_transpose_and_store

void f32m_add_and_store(f32_matrix_ptr mx, f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    mstr_v8sf_add(&mx->stor, &lhs->stor, &rhs->stor);
} // f32m_add_and_store

void f32m_subtract_and_store(f32_matrix_ptr mx, f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    mstr_v8sf_subtract(&mx->stor, &lhs->stor, &rhs->stor);
} // f32m_subtract_and_store

void f32m_multiply_and_store(f32_matrix_ptr mx, f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    mstr_v8sf_multiply(&mx->stor, &lhs->stor, &rhs->stor);
} // f32m_multiply_and_store

void f32m_multiply_scalar_and_store(f32_matrix_ptr mx, f32_matrix_ptr src, float val)
{
    mstr_v8sf_multiply_scalar(&mx->stor, &src->stor, val);
} // f32m_multiply_scalar_and_store
