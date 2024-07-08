#ifndef V8SI_STORAGE_H
#define V8SI_STORAGE_H 1

// =============================================================================
// MODULE: Matrix Storage
// DESIGN GOAL:
//   1. Cope with the allocation, alignment, and fast access to memory;
//   2. Optimize memory access by using proper instructions, depends on CPU
//      model and features.
// =============================================================================

#include "mx_types.h"
#include "mx_common.h"
#include "mx_storage.h"

// ==== Storage Module ==== //

extern void mstr_v8si_init_zeros(mx_stor_ptr ms);
extern void mstr_v8si_init_identity(mx_stor_ptr ms);

extern int32_t mstr_v8si_get(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx);
extern void mstr_v8si_set(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t src);

extern void mstr_v8si_fill(mx_stor_ptr ms, int32_t src);

extern void mstr_v8si_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr dchk, uint32_t * dchk_rows, uint32_t * dchk_cols);

extern void mstr_v8si_store_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk);

inline static void mstr_v8si_init(mx_stor_ptr ms, uint32_t rows, uint32_t cols)
{
    mstr_init(ms, rows, cols, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);
} // mstr_v8si_init

inline static mx_stor_ptr mstr_v8si_create(uint32_t rows, uint32_t cols)
{
    return mstr_create(rows, cols, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);
} // mstr_v8si_create

inline static void mstr_v8si_destroy(mx_stor_ptr ms)
{
    return mstr_destroy(ms);
} // mstr_v8si_destroy

inline static uint32_t mstr_v8si_rows(mx_stor_ptr ms)
{
    return ms->rows;
} // mstr_v8si_rows

inline static uint32_t mstr_v8si_columns(mx_stor_ptr ms)
{
    return ms->cols;
} // mstr_v8si_columns

inline static uint32_t mstr_v8si_values(mx_stor_ptr ms)
{
    return ms->rows * ms->cols;
} // mstr_v8si_values

inline static uint32_t mstr_v8si_value_size(mx_stor_ptr ms)
{
    return ms->val_sz;
} // mstr_v8si_value_size

inline static uint32_t mstr_v8si_chunks_in_height(mx_stor_ptr ms)
{
    return ms->chks_in_height;
} // mstr_chunks_in_height

inline static uint32_t mstr_v8si_chunks_in_width(mx_stor_ptr ms)
{
    return ms->chks_in_width;
} // mstr_chunks_in_width

inline static uint32_t mstr_v8si_chunks(mx_stor_ptr ms)
{
    return ms->chks_in_height * ms->chks_in_width;
} // mstr_v8si_chunks

inline static void mstr_v8si_init_ones(mx_stor_ptr ms)
{
    return mstr_v8si_fill(ms, 1);
} // mstr_v8si_init_ones

inline static void * mstr_v8si_calc_base(mx_stor_ptr ms, uint32_t base_ridx, uint32_t base_cidx, uint32_t rows_in_chk)
{
    return ms->data + (base_ridx * ms->cols_padded + rows_in_chk * base_cidx) * sizeof(int32_t);
} // mstr_v8si_calc_base

inline static void * mstr_v8si_locate_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, uint32_t * rows_in_chk, uint32_t * cols_in_chk)
{
    uint32_t base_ridx = chk_ridx * I32_VALS_IN_CACHE_LINE; // Refer to values.
    uint32_t base_cidx = chk_cidx * I32_VALS_IN_CACHE_LINE; // Refer to values.
    *rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    *cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);
    return mstr_v8si_calc_base(ms, base_ridx, base_cidx, *rows_in_chk);
} // mstr_v8si_locate_chunk

#endif // V8SI_STORAGE_H

