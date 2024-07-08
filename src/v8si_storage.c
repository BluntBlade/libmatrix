#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "src/config.h"
#include "src/mx_common.h"
#include "src/v8si_storage.h"

// ---- V8SI related definitions ----

static v8si_t v8si_idx[2] = {{0, 1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}};

void mstr_v8si_init_zeros(mx_stor_ptr ms)
{
    memset(ms->data, 0, ms->bytes);
} // mstr_v8si_init_zeros

void mstr_v8si_init_identity(mx_stor_ptr ms)
{
    uint32_t i = ms->chks_in_width;
    uint32_t last_rows = 0;
    uint32_t cols_padded = 0;
    uint32_t val_idx = 0;
    int32_t * base = NULL;

    mstr_v8si_init_zeros(ms);

    last_rows = ms->last_chk_height;
    cols_padded = mx_round_to_multiples_of_8(last_rows);
    val_idx = (i - 1) * I32_VALS_IN_CACHE_LINE;
    base = mstr_v8si_calc_base(ms, val_idx, val_idx, last_rows);
    switch (last_rows) {
        default: assert(1); break;
        case 16:
            do { *base = 1; base += cols_padded + 1;   // 0
        case 15: *base = 1; base += cols_padded + 1;   // 1
        case 14: *base = 1; base += cols_padded + 1;   // 2
        case 13: *base = 1; base += cols_padded + 1;   // 3
        case 12: *base = 1; base += cols_padded + 1;   // 4
        case 11: *base = 1; base += cols_padded + 1;   // 5
        case 10: *base = 1; base += cols_padded + 1;   // 6
        case  9: *base = 1; base += cols_padded + 1;   // 7
        case  8: *base = 1; base += cols_padded + 1;   // 8
        case  7: *base = 1; base += cols_padded + 1;   // 9
        case  6: *base = 1; base += cols_padded + 1;   // 10
        case  5: *base = 1; base += cols_padded + 1;   // 11
        case  4: *base = 1; base += cols_padded + 1;   // 12
        case  3: *base = 1; base += cols_padded + 1;   // 13
        case  2: *base = 1; base += cols_padded + 1;   // 14
        case  1: *base = 1;                            // 15
                if (--i == 0) {
                    break;
                } // if
                val_idx = (i - 1) * I32_VALS_IN_CACHE_LINE;
                base = mstr_v8si_calc_base(ms, val_idx, val_idx, 16);
                cols_padded = 16;
            } while (1);
            break;
    } // switch
} // mstr_v8si_init_identity

inline static int32_t * v8si_locate_value(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    uint32_t base_ridx = mx_floor_to_multiples_of_16(val_ridx);
    uint32_t base_cidx = mx_floor_to_multiples_of_16(val_cidx);
    // NOTE: In the case that val's index is equal to base's index, then the difference between them will be zero.
    uint32_t rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    uint32_t cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);
    int32_t * base = mstr_v8si_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
    return base + (val_ridx - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (val_cidx - base_cidx);
} // v8si_locate_value

int32_t mstr_v8si_get(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    return v8si_locate_value(ms, val_ridx, val_cidx)[0];
} // mstr_v8si_get

void mstr_v8si_set(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t src)
{
    v8si_locate_value(ms, val_ridx, val_cidx)[0] = src;
} // mstr_v8si_set

void mstr_v8si_fill(mx_stor_ptr ms, int32_t src)
{
    v8si_t vals = {src, src, src, src, src, src, src, src};
    uint32_t i = 0;
    v8si_t * base = NULL;

    base = ms->data;
    for (i = 0; i < ms->cols_padded / I32_VALS_IN_V8SI; i += 1) {
        base[i] = vals;
    } // for
    for (i = 1; i < ms->rows; i += 1) {
        base += ms->cols_padded / I32_VALS_IN_V8SI;
        memcpy(base, ms->data, sizeof(int32_t) * ms->cols_padded);
    } // for
    // NOTE: No need to zero out any padded int32_t values.
} // mstr_v8si_fill

void mstr_v8si_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr dchk, uint32_t * dchk_rows, uint32_t * dchk_cols)
{
#define v8si_transpose_chunk_half(row) \
    { \
        dchk->v8si_16x1[row][0] = __builtin_ia32_gathersiv8si(v8si_zero, base, idx[sel][0], *mask[0], sizeof(int32_t)); \
    }

#define v8si_transpose_chunk_full(row) \
    { \
        dchk->v8si_16x2[row][0] = __builtin_ia32_gathersiv8si(v8si_zero, base, idx[sel][0], *mask[0], sizeof(int32_t)); \
        dchk->v8si_16x2[row][1] = __builtin_ia32_gathersiv8si(v8si_zero, base, idx[sel][1], *mask[1], sizeof(int32_t)); \
    }

    static v8si_t idx[2][2] = {
        {{0,  8, 16, 24, 32, 40, 48,  56}, { 64,  72,  80,  88,  96, 104, 112, 120}},
        {{0, 16, 32, 48, 64, 80, 96, 112}, {128, 144, 160, 176, 192, 208, 224, 240}},
    };
    uint32_t schk_rows = 0; 
    uint32_t schk_cols = 0; 
    uint32_t i = 0;
    uint32_t sel = 0;
    v8si_t * mask[2];
    int32_t * base = mstr_v8si_locate_chunk(ms, chk_ridx, chk_cidx, &schk_rows, &schk_cols);

    sel = mx_round_to_multiples_of_8(schk_cols) / 8 - 1;
    if (schk_rows <= 8) {
        mask[0] = &v8si_mask[schk_rows];
        switch (schk_cols) {
            default: assert(1); break;
            case 16: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case 15: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case 14: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case 13: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case 12: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case 11: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case 10: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  9: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  8: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  7: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  6: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  5: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  4: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  3: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  2: v8si_transpose_chunk_half(i); i += 1; base += 1;
            case  1: v8si_transpose_chunk_half(i);
        } // switch
    } else {
        mask[0] = &v8si_mask[8];
        mask[1] = &v8si_mask[schk_rows - 8];
        switch (schk_cols) {
            default: assert(1); break;
            case 16: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case 15: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case 14: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case 13: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case 12: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case 11: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case 10: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  9: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  8: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  7: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  6: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  5: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  4: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  3: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  2: v8si_transpose_chunk_full(i); i += 1; base += 1;
            case  1: v8si_transpose_chunk_full(i);
        } // switch
    } // if

    // Swap the number of rows and columns of the target chunk since it is transposed.
    *dchk_rows = schk_cols;
    *dchk_cols = schk_rows;

#undef v8si_transpose_chunk_full
#undef v8si_transpose_chunk_half
} // mstr_v8si_transpose_chunk

void mstr_v8si_store_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk)
{
    uint32_t i = 0;
    uint32_t bytes = 0;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;
    void * base = mstr_v8si_locate_chunk(ms, chk_ridx, chk_cidx, &rows_in_chk, &cols_in_chk);

    if (base == chk) {
        return;
    } // if

    // NOTE: Copy bytes depends on the number of columns in the target chunk.
    bytes = sizeof(int32_t) * mx_round_to_multiples_of_8(cols_in_chk);
    switch (rows_in_chk) {
        default: assert(1); break;
        case 16: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case 15: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case 14: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case 13: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case 12: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case 11: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case 10: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  9: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  8: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  7: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  6: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  5: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  4: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  3: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  2: memcpy(base, &chk->i32_16x16[i][0], bytes); base += bytes; i += 1;
        case  1: memcpy(base, &chk->i32_16x16[i][0], bytes);
    } // switch
    // NOTE: No need to zero out any padded int32_t values.
} // mstr_v8si_store_chunk
