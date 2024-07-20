#ifndef MX_STORAGE_H
#define MX_STORAGE_H 1

// =============================================================================
// MODULE: Matrix Storage
// DESIGN GOAL:
//   1. Cope with the allocation, alignment, and fast access to memory;
//   2. Optimize memory access by using proper instructions, depends on CPU
//      model and features.
// =============================================================================

#include <string.h>

#include "src/mx_types.h"
#include "src/mx_common.h"

enum {
    MSTR_LOAD_VECTOR    = 0,
    MSTR_STORE_VECTOR   = 1,
};

typedef struct MX_STORAGE {
    uint16_t    val_sz;             // The size of one value, in bytes.
    uint16_t    alignment;          // The byte boundary to align with.
    uint16_t    pck_len;            // The number of values in one pack.
    uint16_t    chk_len;            // The number of rows or columns in one chunk.
    uint32_t    chks_in_width;      // The number of chunks in the width of the whole matrix.
    uint32_t    chks_in_height;     // The number of chunks in the height of the whole matrix.
    uint32_t    rows;               // The number of rows.
    uint32_t    cols;               // The number of columns.
    uint32_t    cols_padded;        // The actual number of columns, including padding ones.
    size_t      bytes;              // Total bytes that store values, including no aligned ones.

    void *      buf;                // Non-aligned address of allocated memory, only for calling free().
    void *      data;               // Aligned address of allocated memory.
} mx_stor_t, *mx_stor_ptr;

extern mx_stor_ptr mstr_init(mx_stor_ptr ms, uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len);
extern void mstr_clean(mx_stor_ptr ms);

extern mx_stor_ptr mstr_create(uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len);
extern void mstr_destroy(mx_stor_ptr ms);

extern void mstr_calibrate_index(mx_stor_ptr ms, uint32_t * row, uint32_t * col, int32_t * row_off, int32_t * col_off);
extern void mstr_transfer_row_vector(mx_stor_ptr ms, uint32_t row, uint32_t col, uint32_t off, uint32_t dir, void * vec);

inline static uint32_t mstr_rows(mx_stor_ptr ms)
{
    return ms->rows;
} // mstr_rows

inline static uint32_t mstr_columns(mx_stor_ptr ms)
{
    return ms->cols;
} // mstr_columns

inline static uint32_t mstr_values(mx_stor_ptr ms)
{
    return ms->rows * ms->cols;
} // mstr_values

inline static uint32_t mstr_value_size(mx_stor_ptr ms)
{
    return ms->val_sz;
} // mstr_value_size

inline static uint32_t mstr_chunks_in_height(mx_stor_ptr ms)
{
    return ms->chks_in_height;
} // mstr_chunks_in_height

inline static uint32_t mstr_chunks_in_width(mx_stor_ptr ms)
{
    return ms->chks_in_width;
} // mstr_chunks_in_width

inline static uint32_t mstr_chunks(mx_stor_ptr ms)
{
    return ms->chks_in_height * ms->chks_in_width;
} // mstr_chunks

inline static void * mstr_calc_base(mx_stor_ptr ms, uint32_t base_ridx, uint32_t base_cidx, uint32_t rows_in_chk)
{
    return ms->data + ms->val_sz * (base_ridx * ms->cols_padded + rows_in_chk * base_cidx);
} // mstr_calc_base

inline static void * mstr_locate_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, uint32_t * rows_in_chk, uint32_t * cols_in_chk)
{
    uint32_t base_ridx = chk_ridx * ms->chk_len; // Refer to values.
    uint32_t base_cidx = chk_cidx * ms->chk_len; // Refer to values.
    *rows_in_chk = mx_less_than_or_equal_to(ms->rows - base_ridx, ms->chk_len);
    *cols_in_chk = mx_less_than_or_equal_to(ms->cols - base_cidx, ms->chk_len);
    return mstr_calc_base(ms, base_ridx, base_cidx, *rows_in_chk);
} // mstr_locate_chunk

inline static void mstr_init_zeros(mx_stor_ptr ms)
{
    memset(ms->data, 0, ms->bytes);
} // mstr_init_zeros

inline static void * mstr_locate_value(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    uint32_t base_ridx = mx_floor_to_multiples_of_16(val_ridx);
    uint32_t base_cidx = mx_floor_to_multiples_of_16(val_cidx);
    // NOTE: In the case that val's index is equal to base's index, then the difference between them will be zero.
    uint32_t rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    uint32_t cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);
    int32_t * base = mstr_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
    return base + (val_ridx - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (val_cidx - base_cidx);
} // mstr_locate_value

inline static int32_t mstr_get_i32(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    return ((int32_t *)mstr_locate_value(ms, val_ridx, val_cidx))[0];
} // mstr_get_i32

inline static void mstr_set_i32(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t src)
{
    ((int32_t *)mstr_locate_value(ms, val_ridx, val_cidx))[0] = src;
} // mstr_set_i32

inline static float mstr_get_f32(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    return ((float *)mstr_locate_value(ms, val_ridx, val_cidx))[0];
} // mstr_get_f32

inline static void mstr_set_f32(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, float src)
{
    ((float *)mstr_locate_value(ms, val_ridx, val_cidx))[0] = src;
} // mstr_set_f32

#endif // MX_STORAGE_H

