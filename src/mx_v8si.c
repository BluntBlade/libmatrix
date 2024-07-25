#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "src/config.h"
#include "src/mx_common.h"
#include "src/mx_v8si.h"

void mstr_v8si_load_row_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, int32_t col_off, int32_t def_val, v8si_t * dst)
{
    v4si_t tmp = { .val = {def_val} };
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t off = 0;

    mx_type_reg(*dst) = _mm256_broadcastd_epi32(mx_type_reg(tmp));

    row = val_ridx;
    col = val_cidx;
    mstr_calibrate_index(ms, &row, &col, &row_off, &col_off);
    off = abs(col_off);
    if (row_off < 0 || off >= ms->pck_len || row >= ms->rows || col >= ms->cols) {
        // Out of range, no need to transfer.
        return;
    } // if
    
    mstr_transfer_row_vector(ms, row, col, off, MSTR_LOAD_VECTOR, dst);
} // mstr_v8si_load_row_vector

void mstr_v8si_store_row_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, int32_t col_off, v8si_t * src)
{
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t off = 0;

    row = val_ridx;
    col = val_cidx;
    mstr_calibrate_index(ms, &row, &col, &row_off, &col_off);
    off = abs(col_off);
    if (row_off < 0 || off >= ms->pck_len || row >= ms->rows || col >= ms->cols) {
        // Out of range, no need to transfer.
        return;
    } // if
    
    mstr_transfer_row_vector(ms, row, col, off, MSTR_STORE_VECTOR, src);
} // mstr_v8si_store_row_vector

void mstr_v8si_init_identity(mx_stor_ptr ms)
{
    uint32_t i = ms->chks_in_width;
    uint32_t last_rows = 0;
    uint32_t cols_padded = 0;
    uint32_t val_idx = 0;
    int32_t * base = NULL;

    mstr_v8si_init_zeros(ms);

    // NOTE: ms->rows maybe equal to mx_ceil_to_multiples_of_16(ms->rows).
    last_rows = ms->rows - (mx_ceil_to_multiples_of_16(ms->rows) - ms->chk_len);
    cols_padded = mx_ceil_to_multiples_of_8(last_rows);
    val_idx = (i - 1) * I32S_IN_CACHE_LINE;
    base = mstr_calc_base(ms, val_idx, val_idx, last_rows);
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
                val_idx = (i - 1) * I32S_IN_CACHE_LINE;
                base = mstr_calc_base(ms, val_idx, val_idx, I32S_IN_CACHE_LINE);
                cols_padded = I32S_IN_CACHE_LINE;
            } while (1);
            break;
    } // switch
} // mstr_v8si_init_identity

void mstr_v8si_fill(mx_stor_ptr ms, int32_t src)
{
    v8si_t vals = { .val = {src, src, src, src, src, src, src, src} };
    uint32_t i = 0;
    v8si_t * base = NULL;

    base = ms->data;
    for (i = 0; i < ms->cols_padded / I32S_IN_V8SI; i += 1) {
        base[i] = vals;
    } // for
    for (i = 1; i < ms->rows; i += 1) {
        base += ms->cols_padded / I32S_IN_V8SI;
        memcpy(base, ms->data, I32_SIZE * ms->cols_padded);
    } // for
    // NOTE: No need to zero out any padded int32_t values.
} // mstr_v8si_fill

void mstr_v8si_row_multiply_scalar(mx_stor_ptr ms, uint32_t row, int32_t src)
{
    v4si_t tmp = { .val = {src} };
    v8si_t vals;
    uint32_t i = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    uint32_t row_off = 0;
    uint32_t chk_off = 0;
    mx_chunk_ptr dchk = NULL;

    mx_type_reg(vals) = _mm256_broadcastd_epi32(mx_type_reg(tmp));

    row_off = row - mx_floor_to_multiples_of_16(row);

    // Process the rightmost chunk, maybe full.
    dchk = mstr_locate_chunk_by_value_index(ms, row, (mstr_chunks_in_width(ms) - 1) * I32S_IN_CACHE_LINE, &dchk_rows, &dchk_cols);
    if (dchk_cols <= I32S_IN_V8SI) {
        mx_type_reg(dchk->v8si_16x1[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x1[row_off][0]), mx_type_reg(vals));
    } else {
        mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
        mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
    } // if

    // Process other full chunks.
    chk_off = I32_SIZE * dchk_rows * I32S_IN_CACHE_LINE;
    i = (mstr_chunks_in_width(ms) - 1) / 8;
    switch ((mstr_chunks_in_width(ms) - 1) % 8) {
        case 0:
            while (i-- > 0) {
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
        case 7:
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
        case 6:
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
        case 5:
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
        case 4:
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
        case 3:
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
        case 2:
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
        case 1:
                dchk = (void *)dchk - chk_off;
                mx_type_reg(dchk->v8si_16x2[row_off][0]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][0]), mx_type_reg(vals));
                mx_type_reg(dchk->v8si_16x2[row_off][1]) = _mm256_mullo_epi32(mx_type_reg(dchk->v8si_16x2[row_off][1]), mx_type_reg(vals));
            } // while
    } // switch
} // mstr_v8si_row_multiply_scalar

void mstr_v8si_column_multiply_scalar(mx_stor_ptr ms, uint32_t col, int32_t src)
{
    uint32_t n = 0;
    uint32_t i = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    uint32_t col_off = 0;
    uint32_t base_cidx = 0;
    mx_chunk_ptr dchk = NULL;

    base_cidx = mx_floor_to_multiples_of_16(col);
    col_off = col - base_cidx;
    n = mstr_chunks_in_height(ms) - 1;
    dchk = mstr_locate_chunk_by_value_index(ms, (mstr_chunks_in_height(ms) - 1) * I32S_IN_CACHE_LINE, col, &dchk_rows, &dchk_cols);
    if (dchk_cols <= I32S_IN_V8SI) {
        switch ((i = dchk_rows)) {
            case 16: do { dchk->i32_16x8[--i][col_off] *= src;
            case 15:      dchk->i32_16x8[--i][col_off] *= src;
            case 14:      dchk->i32_16x8[--i][col_off] *= src;
            case 13:      dchk->i32_16x8[--i][col_off] *= src;
            case 12:      dchk->i32_16x8[--i][col_off] *= src;
            case 11:      dchk->i32_16x8[--i][col_off] *= src;
            case 10:      dchk->i32_16x8[--i][col_off] *= src;
            case  9:      dchk->i32_16x8[--i][col_off] *= src;
            case  8:      dchk->i32_16x8[--i][col_off] *= src;
            case  7:      dchk->i32_16x8[--i][col_off] *= src;
            case  6:      dchk->i32_16x8[--i][col_off] *= src;
            case  5:      dchk->i32_16x8[--i][col_off] *= src;
            case  4:      dchk->i32_16x8[--i][col_off] *= src;
            case  3:      dchk->i32_16x8[--i][col_off] *= src;
            case  2:      dchk->i32_16x8[--i][col_off] *= src;
            case  1:      dchk->i32_16x8[--i][col_off] *= src;
                        if (n-- == 0) {
                            return;
                        } // if
                        i = 16;
                        dchk = (void *)dchk - (I32_SIZE * (dchk_rows * base_cidx + I32S_IN_CACHE_LINE * (ms->cols_padded - base_cidx)));
                        dchk_rows = I32S_IN_CACHE_LINE;
                    } while (1);
        } // switch
    } else {
        switch ((i = dchk_rows)) {
            case 16: do { dchk->i32_16x16[--i][col_off] *= src;
            case 15:      dchk->i32_16x16[--i][col_off] *= src;
            case 14:      dchk->i32_16x16[--i][col_off] *= src;
            case 13:      dchk->i32_16x16[--i][col_off] *= src;
            case 12:      dchk->i32_16x16[--i][col_off] *= src;
            case 11:      dchk->i32_16x16[--i][col_off] *= src;
            case 10:      dchk->i32_16x16[--i][col_off] *= src;
            case  9:      dchk->i32_16x16[--i][col_off] *= src;
            case  8:      dchk->i32_16x16[--i][col_off] *= src;
            case  7:      dchk->i32_16x16[--i][col_off] *= src;
            case  6:      dchk->i32_16x16[--i][col_off] *= src;
            case  5:      dchk->i32_16x16[--i][col_off] *= src;
            case  4:      dchk->i32_16x16[--i][col_off] *= src;
            case  3:      dchk->i32_16x16[--i][col_off] *= src;
            case  2:      dchk->i32_16x16[--i][col_off] *= src;
            case  1:      dchk->i32_16x16[--i][col_off] *= src;
                        if (n-- == 0) {
                            return;
                        } // if
                        i = 16;
                        dchk = (void *)dchk - (I32_SIZE * (dchk_rows * base_cidx + I32S_IN_CACHE_LINE * (ms->cols_padded - base_cidx)));
                        dchk_rows = I32S_IN_CACHE_LINE;
                    } while (1);
        } // switch
    } // if
} // mstr_v8si_column_multiply_scalar

void mstr_v8si_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr dchk, uint32_t * dchk_rows, uint32_t * dchk_cols)
{
#define v8si_transpose_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8si_16x1[row][0]) = _mm256_mask_i32gather_epi32(mx_type_reg(v8si_zero), base, mx_type_reg(idx[sel][0]), mx_type_reg(*mask[0]), I32_SIZE); \
    }

#define v8si_transpose_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8si_16x2[row][0]) = _mm256_mask_i32gather_epi32(mx_type_reg(v8si_zero), base, mx_type_reg(idx[sel][0]), mx_type_reg(*mask[0]), I32_SIZE); \
        mx_type_reg(dchk->v8si_16x2[row][1]) = _mm256_mask_i32gather_epi32(mx_type_reg(v8si_zero), base, mx_type_reg(idx[sel][1]), mx_type_reg(*mask[1]), I32_SIZE); \
    }

    static v8si_t idx[2][2] = {
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
    v8si_t * mask[2];
    int32_t * base = mstr_locate_chunk(ms, chk_ridx, chk_cidx, &schk_rows, &schk_cols);

    sel = mx_ceil_to_multiples_of_8(schk_cols) / I32S_IN_V8SI - 1;
    if (schk_rows <= I32S_IN_V8SI) {
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
        mask[0] = &v8si_mask[I32S_IN_V8SI];
        mask[1] = &v8si_mask[schk_rows - I32S_IN_V8SI];
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

#define v8si_add_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8si_16x2[row][0]) = _mm256_add_epi32(mx_type_reg(lchk->v8si_16x2[row][0]), mx_type_reg(rchk->v8si_16x2[row][0])); \
        mx_type_reg(dchk->v8si_16x2[row][1]) = _mm256_add_epi32(mx_type_reg(lchk->v8si_16x2[row][1]), mx_type_reg(rchk->v8si_16x2[row][1])); \
    }

#define v8si_add_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8si_16x1[row][0]) = _mm256_add_epi32(mx_type_reg(lchk->v8si_16x1[row][0]), mx_type_reg(rchk->v8si_16x1[row][0])); \
    }

static void v8si_add_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk)
{
    v8si_add_chunk_full( 0);
    v8si_add_chunk_full( 1);
    v8si_add_chunk_full( 2);
    v8si_add_chunk_full( 3);
    v8si_add_chunk_full( 4);
    v8si_add_chunk_full( 5);
    v8si_add_chunk_full( 6);
    v8si_add_chunk_full( 7);
    v8si_add_chunk_full( 8);
    v8si_add_chunk_full( 9);
    v8si_add_chunk_full(10);
    v8si_add_chunk_full(11);
    v8si_add_chunk_full(12);
    v8si_add_chunk_full(13);
    v8si_add_chunk_full(14);
    v8si_add_chunk_full(15);
} // v8si_add_chunk_full

static void v8si_add_chunk_partly(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t lchk_rows, uint32_t lchk_cols)
{
    uint32_t i = 0;

    if (lchk_cols <= I32S_IN_V8SI) {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8si_add_chunk_half(i); i += 1;
            case 15: v8si_add_chunk_half(i); i += 1;
            case 14: v8si_add_chunk_half(i); i += 1;
            case 13: v8si_add_chunk_half(i); i += 1;
            case 12: v8si_add_chunk_half(i); i += 1;
            case 11: v8si_add_chunk_half(i); i += 1;
            case 10: v8si_add_chunk_half(i); i += 1;
            case  9: v8si_add_chunk_half(i); i += 1;
            case  8: v8si_add_chunk_half(i); i += 1;
            case  7: v8si_add_chunk_half(i); i += 1;
            case  6: v8si_add_chunk_half(i); i += 1;
            case  5: v8si_add_chunk_half(i); i += 1;
            case  4: v8si_add_chunk_half(i); i += 1;
            case  3: v8si_add_chunk_half(i); i += 1;
            case  2: v8si_add_chunk_half(i); i += 1;
            case  1: v8si_add_chunk_half(i);
        } // switch
    } else {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8si_add_chunk_full(i); i += 1;
            case 15: v8si_add_chunk_full(i); i += 1;
            case 14: v8si_add_chunk_full(i); i += 1;
            case 13: v8si_add_chunk_full(i); i += 1;
            case 12: v8si_add_chunk_full(i); i += 1;
            case 11: v8si_add_chunk_full(i); i += 1;
            case 10: v8si_add_chunk_full(i); i += 1;
            case  9: v8si_add_chunk_full(i); i += 1;
            case  8: v8si_add_chunk_full(i); i += 1;
            case  7: v8si_add_chunk_full(i); i += 1;
            case  6: v8si_add_chunk_full(i); i += 1;
            case  5: v8si_add_chunk_full(i); i += 1;
            case  4: v8si_add_chunk_full(i); i += 1;
            case  3: v8si_add_chunk_full(i); i += 1;
            case  2: v8si_add_chunk_full(i); i += 1;
            case  1: v8si_add_chunk_full(i);
        } // switch
    } // if
} // v8si_add_chunk_partly

#undef v8si_add_chunk_half
#undef v8si_add_chunk_full

void mstr_v8si_add(mx_stor_ptr ms, mx_stor_ptr lhs, mx_stor_ptr rhs)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t lchk_rows = 0;
    uint32_t lchk_cols = 0;
    uint32_t rchk_rows = 0;
    uint32_t rchk_cols = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr dchk = NULL;

    for (i = 0; i < mstr_chunks_in_height(lhs); i += 1) {
        for (j = 0; j < mstr_chunks_in_width(lhs); j += 1) {
            lchk = mstr_locate_chunk(lhs, i, j, &lchk_rows, &lchk_cols);
            rchk = mstr_locate_chunk(rhs, i, j, &rchk_rows, &rchk_cols);
            dchk = mstr_locate_chunk(ms, i, j, &dchk_rows, &dchk_cols);
            if (lchk_rows == I32S_IN_CACHE_LINE && lchk_cols == I32S_IN_CACHE_LINE) {
                v8si_add_chunk_fully(dchk, lchk, rchk);
            } else {
                v8si_add_chunk_partly(dchk, lchk, rchk, lchk_rows, lchk_cols);
            } // if
        } // for
    } // for
} // mstr_v8si_add

#define v8si_subtract_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8si_16x2[row][0]) = _mm256_sub_epi32(mx_type_reg(lchk->v8si_16x2[row][0]), mx_type_reg(rchk->v8si_16x2[row][0])); \
        mx_type_reg(dchk->v8si_16x2[row][1]) = _mm256_sub_epi32(mx_type_reg(lchk->v8si_16x2[row][1]), mx_type_reg(rchk->v8si_16x2[row][1])); \
    }

#define v8si_subtract_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8si_16x1[row][0]) = _mm256_sub_epi32(mx_type_reg(lchk->v8si_16x1[row][0]), mx_type_reg(rchk->v8si_16x1[row][0])); \
    }

static void v8si_subtract_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk)
{
    v8si_subtract_chunk_full( 0);
    v8si_subtract_chunk_full( 1);
    v8si_subtract_chunk_full( 2);
    v8si_subtract_chunk_full( 3);
    v8si_subtract_chunk_full( 4);
    v8si_subtract_chunk_full( 5);
    v8si_subtract_chunk_full( 6);
    v8si_subtract_chunk_full( 7);
    v8si_subtract_chunk_full( 8);
    v8si_subtract_chunk_full( 9);
    v8si_subtract_chunk_full(10);
    v8si_subtract_chunk_full(11);
    v8si_subtract_chunk_full(12);
    v8si_subtract_chunk_full(13);
    v8si_subtract_chunk_full(14);
    v8si_subtract_chunk_full(15);
} // v8si_subtract_chunk_fully

static void v8si_subtract_chunk_partly(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t lchk_rows, uint32_t lchk_cols)
{
    uint32_t i = 0;

    if (lchk_cols <= I32S_IN_V8SI) {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8si_subtract_chunk_half(i); i += 1;
            case 15: v8si_subtract_chunk_half(i); i += 1;
            case 14: v8si_subtract_chunk_half(i); i += 1;
            case 13: v8si_subtract_chunk_half(i); i += 1;
            case 12: v8si_subtract_chunk_half(i); i += 1;
            case 11: v8si_subtract_chunk_half(i); i += 1;
            case 10: v8si_subtract_chunk_half(i); i += 1;
            case  9: v8si_subtract_chunk_half(i); i += 1;
            case  8: v8si_subtract_chunk_half(i); i += 1;
            case  7: v8si_subtract_chunk_half(i); i += 1;
            case  6: v8si_subtract_chunk_half(i); i += 1;
            case  5: v8si_subtract_chunk_half(i); i += 1;
            case  4: v8si_subtract_chunk_half(i); i += 1;
            case  3: v8si_subtract_chunk_half(i); i += 1;
            case  2: v8si_subtract_chunk_half(i); i += 1;
            case  1: v8si_subtract_chunk_half(i);
        } // switch
    } else {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8si_subtract_chunk_full(i); i += 1;
            case 15: v8si_subtract_chunk_full(i); i += 1;
            case 14: v8si_subtract_chunk_full(i); i += 1;
            case 13: v8si_subtract_chunk_full(i); i += 1;
            case 12: v8si_subtract_chunk_full(i); i += 1;
            case 11: v8si_subtract_chunk_full(i); i += 1;
            case 10: v8si_subtract_chunk_full(i); i += 1;
            case  9: v8si_subtract_chunk_full(i); i += 1;
            case  8: v8si_subtract_chunk_full(i); i += 1;
            case  7: v8si_subtract_chunk_full(i); i += 1;
            case  6: v8si_subtract_chunk_full(i); i += 1;
            case  5: v8si_subtract_chunk_full(i); i += 1;
            case  4: v8si_subtract_chunk_full(i); i += 1;
            case  3: v8si_subtract_chunk_full(i); i += 1;
            case  2: v8si_subtract_chunk_full(i); i += 1;
            case  1: v8si_subtract_chunk_full(i);
        } // switch
    } // if
} // v8si_subtract_chunk_partly

#undef v8si_subtract_chunk_half
#undef v8si_subtract_chunk_full

void mstr_v8si_subtract(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t lchk_rows = 0;
    uint32_t lchk_cols = 0;
    uint32_t rchk_rows = 0;
    uint32_t rchk_cols = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr dchk = NULL;

    for (i = 0; i < mstr_chunks_in_height(lhs); i += 1) {
        for (j = 0; j < mstr_chunks_in_width(lhs); j += 1) {
            lchk = mstr_locate_chunk(lhs, i, j, &lchk_rows, &lchk_cols);
            rchk = mstr_locate_chunk(rhs, i, j, &rchk_rows, &rchk_cols);
            dchk = mstr_locate_chunk(dst, i, j, &dchk_rows, &dchk_cols);
            if (lchk_rows == I32S_IN_CACHE_LINE && lchk_cols == I32S_IN_CACHE_LINE) {
                v8si_subtract_chunk_fully(dchk, lchk, rchk);
            } else {
                v8si_subtract_chunk_partly(dchk, lchk, rchk, lchk_rows, lchk_cols);
            } // if
        } // for
    } // for
} // mstr_v8si_subtract

static void v8si_multiply_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8si_multiply_chunk_full(row, col) \
    { \
        mx_type_reg(ltmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x2[row][0]), mx_type_reg(rchk->v8si_16x2[col][0])); \
        mx_type_reg(rtmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x2[row][1]), mx_type_reg(rchk->v8si_16x2[col][1])); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(rtmp)); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(v8si_zero)); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(v8si_zero)); \
        dchk->i32_16x16[row][col] += mx_type_val(ltmp)[0] + mx_type_val(ltmp)[4]; \
    }

    v8si_t ltmp;
    v8si_t rtmp;
    uint32_t i = 0;

    for (i = 0; i < I32S_IN_CACHE_LINE; i += 1) {
        v8si_multiply_chunk_full(i,  0);
        v8si_multiply_chunk_full(i,  1);
        v8si_multiply_chunk_full(i,  2);
        v8si_multiply_chunk_full(i,  3);
        v8si_multiply_chunk_full(i,  4);
        v8si_multiply_chunk_full(i,  5);
        v8si_multiply_chunk_full(i,  6);
        v8si_multiply_chunk_full(i,  7);
        v8si_multiply_chunk_full(i,  8);
        v8si_multiply_chunk_full(i,  9);
        v8si_multiply_chunk_full(i, 10);
        v8si_multiply_chunk_full(i, 11);
        v8si_multiply_chunk_full(i, 12);
        v8si_multiply_chunk_full(i, 13);
        v8si_multiply_chunk_full(i, 14);
        v8si_multiply_chunk_full(i, 15);
    } // for

#undef v8si_multiply_chunk_full
} // v8si_multiply_chunk_fully

static void v8si_multiply_chunk_half_to_half(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8si_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(tmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x1[row][0]), mx_type_reg(rchk->v8si_16x1[col][0])); \
        mx_type_reg(tmp) &= mx_type_reg(*mask); \
        mx_type_reg(tmp) = _mm256_hadd_epi32(mx_type_reg(tmp), mx_type_reg(v8si_zero)); \
        mx_type_reg(tmp) = _mm256_hadd_epi32(mx_type_reg(tmp), mx_type_reg(v8si_zero)); \
        dchk->i32_16x8[row][col] += mx_type_val(tmp)[0] + mx_type_val(tmp)[4]; \
    }

    v8si_t tmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask = &v8si_mask[msks];

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case  8: v8si_multiply_chunk_op(i, j); j += 1;
            case  7: v8si_multiply_chunk_op(i, j); j += 1;
            case  6: v8si_multiply_chunk_op(i, j); j += 1;
            case  5: v8si_multiply_chunk_op(i, j); j += 1;
            case  4: v8si_multiply_chunk_op(i, j); j += 1;
            case  3: v8si_multiply_chunk_op(i, j); j += 1;
            case  2: v8si_multiply_chunk_op(i, j); j += 1;
            case  1: v8si_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8si_multiply_chunk_op
} // v8si_multiply_chunk_half_to_half

static void v8si_multiply_chunk_half_to_full(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8si_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(tmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x1[row][0]), mx_type_reg(rchk->v8si_16x1[col][0])); \
        mx_type_reg(tmp) &= mx_type_reg(*mask); \
        mx_type_reg(tmp) = _mm256_hadd_epi32(mx_type_reg(tmp), mx_type_reg(v8si_zero)); \
        mx_type_reg(tmp) = _mm256_hadd_epi32(mx_type_reg(tmp), mx_type_reg(v8si_zero)); \
        dchk->i32_16x16[row][col] += mx_type_val(tmp)[0] + mx_type_val(tmp)[4]; \
    }

    v8si_t tmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask = &v8si_mask[msks];

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case 16: v8si_multiply_chunk_op(i, j); j += 1;
            case 15: v8si_multiply_chunk_op(i, j); j += 1;
            case 14: v8si_multiply_chunk_op(i, j); j += 1;
            case 13: v8si_multiply_chunk_op(i, j); j += 1;
            case 12: v8si_multiply_chunk_op(i, j); j += 1;
            case 11: v8si_multiply_chunk_op(i, j); j += 1;
            case 10: v8si_multiply_chunk_op(i, j); j += 1;
            case  9: v8si_multiply_chunk_op(i, j); j += 1;
            case  8: v8si_multiply_chunk_op(i, j); j += 1;
            case  7: v8si_multiply_chunk_op(i, j); j += 1;
            case  6: v8si_multiply_chunk_op(i, j); j += 1;
            case  5: v8si_multiply_chunk_op(i, j); j += 1;
            case  4: v8si_multiply_chunk_op(i, j); j += 1;
            case  3: v8si_multiply_chunk_op(i, j); j += 1;
            case  2: v8si_multiply_chunk_op(i, j); j += 1;
            case  1: v8si_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8si_multiply_chunk_op
} // v8si_multiply_chunk_half_to_full

static void v8si_multiply_chunk_full_to_half(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8si_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(ltmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x2[row][0]), mx_type_reg(rchk->v8si_16x2[col][0])); \
        mx_type_reg(rtmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x2[row][1]), mx_type_reg(rchk->v8si_16x2[col][1])); \
        mx_type_reg(ltmp) &= mx_type_reg(*mask[0]); \
        mx_type_reg(rtmp) &= mx_type_reg(*mask[1]); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(rtmp)); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(v8si_zero)); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(v8si_zero)); \
        dchk->i32_16x8[row][col] += mx_type_val(ltmp)[0] + mx_type_val(ltmp)[4]; \
    }

    v8si_t ltmp;
    v8si_t rtmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask[2] = {&v8si_mask[I32S_IN_V8SI], &v8si_mask[msks - I32S_IN_V8SI]};

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case  8: v8si_multiply_chunk_op(i, j); j += 1;
            case  7: v8si_multiply_chunk_op(i, j); j += 1;
            case  6: v8si_multiply_chunk_op(i, j); j += 1;
            case  5: v8si_multiply_chunk_op(i, j); j += 1;
            case  4: v8si_multiply_chunk_op(i, j); j += 1;
            case  3: v8si_multiply_chunk_op(i, j); j += 1;
            case  2: v8si_multiply_chunk_op(i, j); j += 1;
            case  1: v8si_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8si_multiply_chunk_op
} // v8si_multiply_chunk_full_to_half

static void v8si_multiply_chunk_full_to_full(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8si_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(ltmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x2[row][0]), mx_type_reg(rchk->v8si_16x2[col][0])); \
        mx_type_reg(rtmp) = _mm256_mullo_epi32(mx_type_reg(lchk->v8si_16x2[row][1]), mx_type_reg(rchk->v8si_16x2[col][1])); \
        mx_type_reg(ltmp) &= mx_type_reg(*mask[0]); \
        mx_type_reg(rtmp) &= mx_type_reg(*mask[1]); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(rtmp)); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(v8si_zero)); \
        mx_type_reg(ltmp) = _mm256_hadd_epi32(mx_type_reg(ltmp), mx_type_reg(v8si_zero)); \
        dchk->i32_16x16[row][col] += mx_type_val(ltmp)[0] + mx_type_val(ltmp)[4]; \
    }

    v8si_t ltmp;
    v8si_t rtmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask[2] = {&v8si_mask[I32S_IN_V8SI], &v8si_mask[msks - I32S_IN_V8SI]};

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case 16: v8si_multiply_chunk_op(i, j); j += 1;
            case 15: v8si_multiply_chunk_op(i, j); j += 1;
            case 14: v8si_multiply_chunk_op(i, j); j += 1;
            case 13: v8si_multiply_chunk_op(i, j); j += 1;
            case 12: v8si_multiply_chunk_op(i, j); j += 1;
            case 11: v8si_multiply_chunk_op(i, j); j += 1;
            case 10: v8si_multiply_chunk_op(i, j); j += 1;
            case  9: v8si_multiply_chunk_op(i, j); j += 1;
            case  8: v8si_multiply_chunk_op(i, j); j += 1;
            case  7: v8si_multiply_chunk_op(i, j); j += 1;
            case  6: v8si_multiply_chunk_op(i, j); j += 1;
            case  5: v8si_multiply_chunk_op(i, j); j += 1;
            case  4: v8si_multiply_chunk_op(i, j); j += 1;
            case  3: v8si_multiply_chunk_op(i, j); j += 1;
            case  2: v8si_multiply_chunk_op(i, j); j += 1;
            case  1: v8si_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8si_multiply_chunk_op
} // v8si_multiply_chunk_full_to_full

typedef void (*v8si_multiply_chunk_fn)(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks);

void mstr_v8si_multiply(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs)
{
    static v8si_multiply_chunk_fn ops[3][3] = {
        {
            &v8si_multiply_chunk_half_to_half, // [0][0]  <=  16x1 * 1x8
            &v8si_multiply_chunk_half_to_full, // [0][1]  <=  16x1 * 1x15
            &v8si_multiply_chunk_half_to_full, // [0][2]  <=  16x1 * 1x16
        },
        {
            &v8si_multiply_chunk_full_to_half, // [1][0]  <=  16x15 * 15x8
            &v8si_multiply_chunk_full_to_full, // [1][1]  <=  16x15 * 15x15
            &v8si_multiply_chunk_full_to_full, // [1][2]  <=  16x15 * 15x16
        },
        {
            &v8si_multiply_chunk_full_to_half, // [2][0]  <=   8x16 * 16x8
            &v8si_multiply_chunk_full_to_full, // [2][1]  <=   8x16 * 16x15
            &v8si_multiply_chunk_fully,        // [2][2]  <=  16x16 * 16x16 (Only this case)
        },
    };

    mx_chunk_t rchk;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t lchk_rows = 0;
    uint32_t lchk_cols = 0;
    uint32_t rchk_rows = 0;
    uint32_t rchk_cols = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    uint32_t ssel = 0;
    uint32_t dsel = 0;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr dchk = NULL;

    mstr_v8si_init_zeros(dst);
    for (k = 0; k < mstr_chunks_in_height(rhs); k += 1) {
        for (j = 0; j < mstr_chunks_in_width(rhs); j += 1) {
            mstr_v8si_transpose_chunk(rhs, k, j, &rchk, &rchk_rows, &rchk_cols);

            for (i = 0; i < mstr_chunks_in_height(lhs); i += 1) {
                lchk = mstr_locate_chunk(lhs, i, k, &lchk_rows, &lchk_cols);
                dchk = mstr_locate_chunk(dst, i, j, &dchk_rows, &dchk_cols);

                ssel = mx_ceil_to_multiples_of_8(rchk_cols) / I32S_IN_V8SI - 1 + (uint32_t)(lchk_cols == I32S_IN_CACHE_LINE);
                dsel = mx_ceil_to_multiples_of_8(dchk_cols) / I32S_IN_V8SI - 1 + (uint32_t)(dchk_cols == I32S_IN_CACHE_LINE);
                (*ops[ssel][dsel])(dchk, lchk, &rchk, dchk_rows, dchk_cols, lchk_cols);
            } // for
        } // for
    } // for
} // mstr_v8si_multiply

#define v8si_scalar_multiply_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8si_16x2[row][0]) = _mm256_mullo_epi32(mx_type_reg(*vals), mx_type_reg(schk->v8si_16x2[row][0])); \
        mx_type_reg(dchk->v8si_16x2[row][1]) = _mm256_mullo_epi32(mx_type_reg(*vals), mx_type_reg(schk->v8si_16x2[row][1])); \
    }

#define v8si_scalar_multiply_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8si_16x1[row][0]) = _mm256_mullo_epi32(mx_type_reg(*vals), mx_type_reg(schk->v8si_16x1[row][0])); \
    }

static void v8si_multiply_scalar_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr schk, v8si_t * vals)
{
    v8si_scalar_multiply_chunk_full( 0);
    v8si_scalar_multiply_chunk_full( 1);
    v8si_scalar_multiply_chunk_full( 2);
    v8si_scalar_multiply_chunk_full( 3);
    v8si_scalar_multiply_chunk_full( 4);
    v8si_scalar_multiply_chunk_full( 5);
    v8si_scalar_multiply_chunk_full( 6);
    v8si_scalar_multiply_chunk_full( 7);
    v8si_scalar_multiply_chunk_full( 8);
    v8si_scalar_multiply_chunk_full( 9);
    v8si_scalar_multiply_chunk_full(10);
    v8si_scalar_multiply_chunk_full(11);
    v8si_scalar_multiply_chunk_full(12);
    v8si_scalar_multiply_chunk_full(13);
    v8si_scalar_multiply_chunk_full(14);
    v8si_scalar_multiply_chunk_full(15);
} // v8si_multiply_scalar_chunk_fully

static void v8si_multiply_scalar_chunk_partly(mx_chunk_ptr dchk, mx_chunk_ptr schk, v8si_t * vals, uint32_t src_rows, uint32_t src_cols)
{
    uint32_t i = 0;

    if (src_cols <= I32S_IN_V8SI) {
        switch (src_rows) {
            default: assert(1); break;
            case 16: v8si_scalar_multiply_chunk_half(i); i += 1;
            case 15: v8si_scalar_multiply_chunk_half(i); i += 1;
            case 14: v8si_scalar_multiply_chunk_half(i); i += 1;
            case 13: v8si_scalar_multiply_chunk_half(i); i += 1;
            case 12: v8si_scalar_multiply_chunk_half(i); i += 1;
            case 11: v8si_scalar_multiply_chunk_half(i); i += 1;
            case 10: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  9: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  8: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  7: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  6: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  5: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  4: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  3: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  2: v8si_scalar_multiply_chunk_half(i); i += 1;
            case  1: v8si_scalar_multiply_chunk_half(i);
        } // switch
    } else {
        switch (src_rows) {
            default: assert(1); break;
            case 16: v8si_scalar_multiply_chunk_full(i); i += 1;
            case 15: v8si_scalar_multiply_chunk_full(i); i += 1;
            case 14: v8si_scalar_multiply_chunk_full(i); i += 1;
            case 13: v8si_scalar_multiply_chunk_full(i); i += 1;
            case 12: v8si_scalar_multiply_chunk_full(i); i += 1;
            case 11: v8si_scalar_multiply_chunk_full(i); i += 1;
            case 10: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  9: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  8: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  7: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  6: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  5: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  4: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  3: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  2: v8si_scalar_multiply_chunk_full(i); i += 1;
            case  1: v8si_scalar_multiply_chunk_full(i);
        } // switch
    } // if
} // v8si_multiply_scalar_chunk_partly

void mstr_v8si_multiply_scalar(mx_stor_ptr dst, mx_stor_ptr src, int32_t val)
{
    v4si_t tmp = { .val = {val} };
    v8si_t vals;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t schk_rows = 0;
    uint32_t schk_cols = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    mx_chunk_ptr schk = NULL;
    mx_chunk_ptr dchk = NULL;

    mx_type_reg(vals) = _mm256_broadcastd_epi32(mx_type_reg(tmp));

    for (i = 0; i < mstr_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_chunks_in_width(src); j += 1) {
            schk = mstr_locate_chunk(src, i, j, &schk_rows, &schk_cols);
            dchk = mstr_locate_chunk(dst, i, j, &dchk_rows, &dchk_cols);
            if (schk_rows == I32S_IN_CACHE_LINE && schk_cols == I32S_IN_CACHE_LINE) {
                v8si_multiply_scalar_chunk_fully(dchk, schk, &vals);
            } else {
                v8si_multiply_scalar_chunk_partly(dchk, schk, &vals, schk_rows, schk_cols);
            } // if
        } // for
    } // for
} // mstr_v8si_multiply_scalar

void mstr_v8si_transpose(mx_stor_ptr dst, mx_stor_ptr src)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t schk_rows = 0;
    uint32_t schk_cols = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    void * base = NULL;
    mx_chunk_ptr dchk = NULL;

    for (i = 0; i < mstr_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_chunks_in_width(src); j += 1) {
            base = mstr_locate_chunk(src, i, j, &schk_rows, &schk_cols);
            dchk = mstr_locate_chunk(dst, j, i, &dchk_rows, &dchk_cols);
            mstr_v8si_transpose_chunk(src, i, j, dchk, &dchk_rows, &dchk_cols);
        } // for
    } // for
} // mstr_v8si_transpose
