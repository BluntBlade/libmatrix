#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "src/config.h"
#include "src/mx_common.h"
#include "src/v8sf_storage.h"

// ---- V8SF related definitions ----

v8sf_t v8sf_zero = { .val = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} };
v8sf_t v8sf_mask[9] = {
    { .val = { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0} },
};

void mstr_v8sf_init_zeros(mx_stor_ptr ms)
{
    memset(ms->data, 0, ms->bytes);
} // mstr_v8sf_init_zeros

void mstr_v8sf_load_row_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t col_off, float def_val, v8sf_t * dst)
{
    v4sf_t tmp = { .val = {def_val} };
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t off = 0;
    uint32_t cnt = 0;
    uint32_t base_ridx = 0;
    uint32_t base_cidx = 0;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;
    uint32_t rmd = 0;
    float * base = NULL;

    mx_type_reg(*dst) = _mm256_broadcast_ps(&mx_type_reg(tmp));

    if (val_ridx >= ms->rows || ms->rows == 0) {
        return;
    } // if

    row = val_ridx;
    col = val_cidx;
    off = abs(col_off);

    // Calibrate the column index.
    if (col_off < 0) {
        if (col < off) {
            // The actual column index is beyond the LEFT boundary.
            off -= col;
            col = 0;
            if (off >= F32_VALS_IN_V8SF) {
                // There is not enough values to load.
                return;
            } // if
        } else {
            col -= off;
            off = 0;
        } // if
    } else if (col_off > 0) {
        col += col_off;
        off = 0;
    } // if

    if (col >= ms->cols) {
        // The actual column index is beyond the RIGHT boundary.
        return;
    } // if

    cnt = F32_VALS_IN_V8SF - off;

    base_ridx = row & (~(F32_VALS_IN_CACHE_LINE - 1)); // Refer to values.
    base_cidx = col & (~(F32_VALS_IN_CACHE_LINE - 1)); // Refer to values.
    rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);

    base = mstr_v8sf_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
    rmd = cols_in_chk - (col - base_cidx);

    if (rmd >= cnt) {
        memcpy((float *)dst + off, base + (row - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (col - base_cidx), sizeof(float) * cnt);
        return;
    } // if
    
    memcpy((float *)dst + off, base + (row - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (col - base_cidx), sizeof(float) * rmd);
    if ((col += rmd) >= ms->cols) {
        return;
    } // if

    cnt -= rmd;
    off += rmd;

    base += rows_in_chk * cols_in_chk;
    cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - col); // col must be equal to the base_cidx of the right adjacent chunk.
    memcpy((float *)dst + off, base + (row - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk), sizeof(float) * cnt);
} // mstr_v8sf_load_row_vector

void mstr_v8sf_init_identity(mx_stor_ptr ms)
{
    uint32_t i = ms->chks_in_width;
    uint32_t last_rows = 0;
    uint32_t cols_padded = 0;
    uint32_t val_idx = 0;
    float * base = NULL;

    mstr_v8sf_init_zeros(ms);

    last_rows = ms->last_chk_height;
    cols_padded = mx_round_to_multiples_of_8(last_rows);
    val_idx = (i - 1) * F32_VALS_IN_CACHE_LINE;
    base = mstr_v8sf_calc_base(ms, val_idx, val_idx, last_rows);
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
                val_idx = (i - 1) * F32_VALS_IN_CACHE_LINE;
                base = mstr_v8sf_calc_base(ms, val_idx, val_idx, 16);
                cols_padded = 16;
            } while (1);
            break;
    } // switch
} // mstr_v8sf_init_identity

inline static float * v8sf_locate_value(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    uint32_t base_ridx = mx_floor_to_multiples_of_16(val_ridx);
    uint32_t base_cidx = mx_floor_to_multiples_of_16(val_cidx);
    // NOTE: In the case that val's index is equal to base's index, then the difference between them will be zero.
    uint32_t rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    uint32_t cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);
    float * base = mstr_v8sf_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
    return base + (val_ridx - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (val_cidx - base_cidx);
} // v8sf_locate_value

float mstr_v8sf_get(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    return v8sf_locate_value(ms, val_ridx, val_cidx)[0];
} // mstr_v8sf_get

void mstr_v8sf_set(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, float src)
{
    v8sf_locate_value(ms, val_ridx, val_cidx)[0] = src;
} // mstr_v8sf_set

void mstr_v8sf_fill(mx_stor_ptr ms, float src)
{
    v8sf_t vals = { .val = {src, src, src, src, src, src, src, src} };
    uint32_t i = 0;
    v8sf_t * base = NULL;

    base = ms->data;
    for (i = 0; i < ms->cols_padded / F32_VALS_IN_V8SF; i += 1) {
        base[i] = vals;
    } // for
    for (i = 1; i < ms->rows; i += 1) {
        base += ms->cols_padded / F32_VALS_IN_V8SF;
        memcpy(base, ms->data, sizeof(float) * ms->cols_padded);
    } // for
    // NOTE: No need to zero out any padded float values.
} // mstr_v8sf_fill

void mstr_v8sf_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr dchk, uint32_t * dchk_rows, uint32_t * dchk_cols)
{
#define v8sf_transpose_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8sf_16x1[row][0]) = _mm256_mask_i32gather_ps(mx_type_reg(v8sf_zero), base, mx_type_reg(idx[sel][0]), mx_type_reg(*mask[0]), sizeof(float)); \
    }

#define v8sf_transpose_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8sf_16x2[row][0]) = _mm256_mask_i32gather_ps(mx_type_reg(v8sf_zero), base, mx_type_reg(idx[sel][0]), mx_type_reg(*mask[0]), sizeof(float)); \
        mx_type_reg(dchk->v8sf_16x2[row][1]) = _mm256_mask_i32gather_ps(mx_type_reg(v8sf_zero), base, mx_type_reg(idx[sel][1]), mx_type_reg(*mask[1]), sizeof(float)); \
    }

    static v8sf_t idx[2][2] = {
        {
            { .val = {  0,   8,  16,  24,  32,  40,  48,  56 } },
            { .val = { 64,  72,  80,  88,  96, 104, 112, 120 } },
        },
        {
            { .val = {  0,  16,  32,  48,  64,  80,  96, 112} },
            { .val = {128, 144, 160, 176, 192, 208, 224, 240} },
        },
    };
    uint32_t schk_rows = 0; 
    uint32_t schk_cols = 0; 
    uint32_t i = 0;
    uint32_t sel = 0;
    v8sf_t * mask[2];
    float * base = mstr_v8sf_locate_chunk(ms, chk_ridx, chk_cidx, &schk_rows, &schk_cols);

    sel = mx_round_to_multiples_of_8(schk_cols) / 8 - 1;
    if (schk_rows <= 8) {
        mask[0] = &v8sf_mask[schk_rows];
        switch (schk_cols) {
            default: assert(1); break;
            case 16: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case 15: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case 14: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case 13: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case 12: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case 11: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case 10: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  9: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  8: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  7: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  6: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  5: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  4: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  3: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  2: v8sf_transpose_chunk_half(i); i += 1; base += 1;
            case  1: v8sf_transpose_chunk_half(i);
        } // switch
    } else {
        mask[0] = &v8sf_mask[8];
        mask[1] = &v8sf_mask[schk_rows - 8];
        switch (schk_cols) {
            default: assert(1); break;
            case 16: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case 15: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case 14: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case 13: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case 12: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case 11: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case 10: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  9: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  8: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  7: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  6: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  5: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  4: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  3: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  2: v8sf_transpose_chunk_full(i); i += 1; base += 1;
            case  1: v8sf_transpose_chunk_full(i);
        } // switch
    } // if

    // Swap the number of rows and columns of the target chunk since it is transposed.
    *dchk_rows = schk_cols;
    *dchk_cols = schk_rows;

#undef v8sf_transpose_chunk_full
#undef v8sf_transpose_chunk_half
} // mstr_v8sf_transpose_chunk
