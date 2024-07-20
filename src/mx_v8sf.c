#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "src/config.h"
#include "src/mx_common.h"
#include "src/mx_v8si.h"
#include "src/mx_v8sf.h"

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

void mstr_v8sf_load_row_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, int32_t col_off, float def_val, v8sf_t * dst)
{
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t off = 0;

    mx_type_reg(*dst) = _mm256_broadcast_ss(&def_val);

    row = val_ridx;
    col = val_cidx;
    mstr_calibrate_index(ms, &row, &col, &row_off, &col_off);
    off = abs(col_off);
    if (row_off < 0 || off >= ms->pck_len || row >= ms->rows || col >= ms->cols) {
        // Out of range, no need to transfer.
        return;
    } // if
    
    mstr_transfer_row_vector(ms, row, col, off, MSTR_LOAD_VECTOR, dst);
} // mstr_v8sf_load_row_vector

void mstr_v8sf_store_row_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, int32_t col_off, v8sf_t * src)
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
} // mstr_v8sf_store_row_vector

void mstr_v8sf_init_identity(mx_stor_ptr ms)
{
    uint32_t i = ms->chks_in_width;
    uint32_t last_rows = 0;
    uint32_t cols_padded = 0;
    uint32_t val_idx = 0;
    float * base = NULL;

    mstr_v8sf_init_zeros(ms);

    // NOTE: ms->rows maybe equal to mx_ceil_to_multiples_of_16(ms->rows).
    last_rows = ms->rows - (mx_ceil_to_multiples_of_16(ms->rows) - ms->chk_len);
    cols_padded = mx_ceil_to_multiples_of_8(last_rows);
    val_idx = (i - 1) * F32S_IN_CACHE_LINE;
    base = mstr_calc_base(ms, val_idx, val_idx, last_rows);
    switch (last_rows) {
        default: assert(1); break;
        case 16:
            do { *base = 1.0; base += cols_padded + 1;   // 0
        case 15: *base = 1.0; base += cols_padded + 1;   // 1
        case 14: *base = 1.0; base += cols_padded + 1;   // 2
        case 13: *base = 1.0; base += cols_padded + 1;   // 3
        case 12: *base = 1.0; base += cols_padded + 1;   // 4
        case 11: *base = 1.0; base += cols_padded + 1;   // 5
        case 10: *base = 1.0; base += cols_padded + 1;   // 6
        case  9: *base = 1.0; base += cols_padded + 1;   // 7
        case  8: *base = 1.0; base += cols_padded + 1;   // 8
        case  7: *base = 1.0; base += cols_padded + 1;   // 9
        case  6: *base = 1.0; base += cols_padded + 1;   // 10
        case  5: *base = 1.0; base += cols_padded + 1;   // 11
        case  4: *base = 1.0; base += cols_padded + 1;   // 12
        case  3: *base = 1.0; base += cols_padded + 1;   // 13
        case  2: *base = 1.0; base += cols_padded + 1;   // 14
        case  1: *base = 1.0;                            // 15
                if (--i == 0) {
                    break;
                } // if
                val_idx = (i - 1) * F32S_IN_CACHE_LINE;
                base = mstr_calc_base(ms, val_idx, val_idx, F32S_IN_CACHE_LINE);
                cols_padded = F32S_IN_CACHE_LINE;
            } while (1);
            break;
    } // switch
} // mstr_v8sf_init_identity

void mstr_v8sf_fill(mx_stor_ptr ms, float src)
{
    v8sf_t vals = { .val = {src, src, src, src, src, src, src, src} };
    uint32_t i = 0;
    v8sf_t * base = NULL;

    base = ms->data;
    for (i = 0; i < ms->cols_padded / F32S_IN_V8SF; i += 1) {
        base[i] = vals;
    } // for
    for (i = 1; i < ms->rows; i += 1) {
        base += ms->cols_padded / F32S_IN_V8SF;
        memcpy(base, ms->data, F32_SIZE * ms->cols_padded);
    } // for
    // NOTE: No need to zero out any padded float values.
} // mstr_v8sf_fill

void mstr_v8sf_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr dchk, uint32_t * dchk_rows, uint32_t * dchk_cols)
{
#define v8sf_transpose_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8sf_16x1[row][0]) = _mm256_mask_i32gather_ps(mx_type_reg(v8sf_zero), base, mx_type_reg(idx[sel][0]), mx_type_reg(*mask[0]), F32_SIZE); \
    }

#define v8sf_transpose_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8sf_16x2[row][0]) = _mm256_mask_i32gather_ps(mx_type_reg(v8sf_zero), base, mx_type_reg(idx[sel][0]), mx_type_reg(*mask[0]), F32_SIZE); \
        mx_type_reg(dchk->v8sf_16x2[row][1]) = _mm256_mask_i32gather_ps(mx_type_reg(v8sf_zero), base, mx_type_reg(idx[sel][1]), mx_type_reg(*mask[1]), F32_SIZE); \
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
    float * base = mstr_locate_chunk(ms, chk_ridx, chk_cidx, &schk_rows, &schk_cols);

    sel = mx_ceil_to_multiples_of_8(schk_cols) / 8 - 1;
    if (schk_rows <= 8) {
        mask[0] = &v8si_mask[schk_rows];
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
        mask[0] = &v8si_mask[8];
        mask[1] = &v8si_mask[schk_rows - 8];
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

#define v8sf_add_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8sf_16x2[row][0]) = _mm256_add_ps(mx_type_reg(lchk->v8sf_16x2[row][0]), mx_type_reg(rchk->v8sf_16x2[row][0])); \
        mx_type_reg(dchk->v8sf_16x2[row][1]) = _mm256_add_ps(mx_type_reg(lchk->v8sf_16x2[row][1]), mx_type_reg(rchk->v8sf_16x2[row][1])); \
    }

#define v8sf_add_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8sf_16x1[row][0]) = _mm256_add_ps(mx_type_reg(lchk->v8sf_16x1[row][0]), mx_type_reg(rchk->v8sf_16x1[row][0])); \
    }

static void v8sf_add_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk)
{
    v8sf_add_chunk_full( 0);
    v8sf_add_chunk_full( 1);
    v8sf_add_chunk_full( 2);
    v8sf_add_chunk_full( 3);
    v8sf_add_chunk_full( 4);
    v8sf_add_chunk_full( 5);
    v8sf_add_chunk_full( 6);
    v8sf_add_chunk_full( 7);
    v8sf_add_chunk_full( 8);
    v8sf_add_chunk_full( 9);
    v8sf_add_chunk_full(10);
    v8sf_add_chunk_full(11);
    v8sf_add_chunk_full(12);
    v8sf_add_chunk_full(13);
    v8sf_add_chunk_full(14);
    v8sf_add_chunk_full(15);
} // v8sf_add_chunk_full

static void v8sf_add_chunk_partly(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t lchk_rows, uint32_t lchk_cols)
{
    uint32_t i = 0;

    if (lchk_cols <= 8) {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8sf_add_chunk_half(i); i += 1;
            case 15: v8sf_add_chunk_half(i); i += 1;
            case 14: v8sf_add_chunk_half(i); i += 1;
            case 13: v8sf_add_chunk_half(i); i += 1;
            case 12: v8sf_add_chunk_half(i); i += 1;
            case 11: v8sf_add_chunk_half(i); i += 1;
            case 10: v8sf_add_chunk_half(i); i += 1;
            case  9: v8sf_add_chunk_half(i); i += 1;
            case  8: v8sf_add_chunk_half(i); i += 1;
            case  7: v8sf_add_chunk_half(i); i += 1;
            case  6: v8sf_add_chunk_half(i); i += 1;
            case  5: v8sf_add_chunk_half(i); i += 1;
            case  4: v8sf_add_chunk_half(i); i += 1;
            case  3: v8sf_add_chunk_half(i); i += 1;
            case  2: v8sf_add_chunk_half(i); i += 1;
            case  1: v8sf_add_chunk_half(i);
        } // switch
    } else {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8sf_add_chunk_full(i); i += 1;
            case 15: v8sf_add_chunk_full(i); i += 1;
            case 14: v8sf_add_chunk_full(i); i += 1;
            case 13: v8sf_add_chunk_full(i); i += 1;
            case 12: v8sf_add_chunk_full(i); i += 1;
            case 11: v8sf_add_chunk_full(i); i += 1;
            case 10: v8sf_add_chunk_full(i); i += 1;
            case  9: v8sf_add_chunk_full(i); i += 1;
            case  8: v8sf_add_chunk_full(i); i += 1;
            case  7: v8sf_add_chunk_full(i); i += 1;
            case  6: v8sf_add_chunk_full(i); i += 1;
            case  5: v8sf_add_chunk_full(i); i += 1;
            case  4: v8sf_add_chunk_full(i); i += 1;
            case  3: v8sf_add_chunk_full(i); i += 1;
            case  2: v8sf_add_chunk_full(i); i += 1;
            case  1: v8sf_add_chunk_full(i);
        } // switch
    } // if
} // v8sf_add_chunk_partly

#undef v8sf_add_chunk_half
#undef v8sf_add_chunk_full

void mstr_v8sf_add(mx_stor_ptr ms, mx_stor_ptr lhs, mx_stor_ptr rhs)
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
            if (lchk_rows == F32S_IN_CACHE_LINE && lchk_cols == F32S_IN_CACHE_LINE) {
                v8sf_add_chunk_fully(dchk, lchk, rchk);
            } else {
                v8sf_add_chunk_partly(dchk, lchk, rchk, lchk_rows, lchk_cols);
            } // if
        } // for
    } // for
} // mstr_v8sf_add

#define v8sf_subtract_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8sf_16x2[row][0]) = _mm256_sub_ps(mx_type_reg(lchk->v8sf_16x2[row][0]), mx_type_reg(rchk->v8sf_16x2[row][0])); \
        mx_type_reg(dchk->v8sf_16x2[row][1]) = _mm256_sub_ps(mx_type_reg(lchk->v8sf_16x2[row][1]), mx_type_reg(rchk->v8sf_16x2[row][1])); \
    }

#define v8sf_subtract_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8sf_16x1[row][0]) = _mm256_sub_ps(mx_type_reg(lchk->v8sf_16x1[row][0]), mx_type_reg(rchk->v8sf_16x1[row][0])); \
    }

static void v8sf_subtract_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk)
{
    v8sf_subtract_chunk_full( 0);
    v8sf_subtract_chunk_full( 1);
    v8sf_subtract_chunk_full( 2);
    v8sf_subtract_chunk_full( 3);
    v8sf_subtract_chunk_full( 4);
    v8sf_subtract_chunk_full( 5);
    v8sf_subtract_chunk_full( 6);
    v8sf_subtract_chunk_full( 7);
    v8sf_subtract_chunk_full( 8);
    v8sf_subtract_chunk_full( 9);
    v8sf_subtract_chunk_full(10);
    v8sf_subtract_chunk_full(11);
    v8sf_subtract_chunk_full(12);
    v8sf_subtract_chunk_full(13);
    v8sf_subtract_chunk_full(14);
    v8sf_subtract_chunk_full(15);
} // v8sf_subtract_chunk_fully

static void v8sf_subtract_chunk_partly(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t lchk_rows, uint32_t lchk_cols)
{
    uint32_t i = 0;

    if (lchk_cols <= 8) {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8sf_subtract_chunk_half(i); i += 1;
            case 15: v8sf_subtract_chunk_half(i); i += 1;
            case 14: v8sf_subtract_chunk_half(i); i += 1;
            case 13: v8sf_subtract_chunk_half(i); i += 1;
            case 12: v8sf_subtract_chunk_half(i); i += 1;
            case 11: v8sf_subtract_chunk_half(i); i += 1;
            case 10: v8sf_subtract_chunk_half(i); i += 1;
            case  9: v8sf_subtract_chunk_half(i); i += 1;
            case  8: v8sf_subtract_chunk_half(i); i += 1;
            case  7: v8sf_subtract_chunk_half(i); i += 1;
            case  6: v8sf_subtract_chunk_half(i); i += 1;
            case  5: v8sf_subtract_chunk_half(i); i += 1;
            case  4: v8sf_subtract_chunk_half(i); i += 1;
            case  3: v8sf_subtract_chunk_half(i); i += 1;
            case  2: v8sf_subtract_chunk_half(i); i += 1;
            case  1: v8sf_subtract_chunk_half(i);
        } // switch
    } else {
        switch (lchk_rows) {
            default: assert(1); break;
            case 16: v8sf_subtract_chunk_full(i); i += 1;
            case 15: v8sf_subtract_chunk_full(i); i += 1;
            case 14: v8sf_subtract_chunk_full(i); i += 1;
            case 13: v8sf_subtract_chunk_full(i); i += 1;
            case 12: v8sf_subtract_chunk_full(i); i += 1;
            case 11: v8sf_subtract_chunk_full(i); i += 1;
            case 10: v8sf_subtract_chunk_full(i); i += 1;
            case  9: v8sf_subtract_chunk_full(i); i += 1;
            case  8: v8sf_subtract_chunk_full(i); i += 1;
            case  7: v8sf_subtract_chunk_full(i); i += 1;
            case  6: v8sf_subtract_chunk_full(i); i += 1;
            case  5: v8sf_subtract_chunk_full(i); i += 1;
            case  4: v8sf_subtract_chunk_full(i); i += 1;
            case  3: v8sf_subtract_chunk_full(i); i += 1;
            case  2: v8sf_subtract_chunk_full(i); i += 1;
            case  1: v8sf_subtract_chunk_full(i);
        } // switch
    } // if
} // v8sf_subtract_chunk_partly

#undef v8sf_subtract_chunk_half
#undef v8sf_subtract_chunk_full

void mstr_v8sf_subtract(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs)
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
            if (lchk_rows == F32S_IN_CACHE_LINE && lchk_cols == F32S_IN_CACHE_LINE) {
                v8sf_subtract_chunk_fully(dchk, lchk, rchk);
            } else {
                v8sf_subtract_chunk_partly(dchk, lchk, rchk, lchk_rows, lchk_cols);
            } // if
        } // for
    } // for
} // mstr_v8sf_subtract

static void v8sf_multiply_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8sf_multiply_chunk_full(row, col) \
    { \
        mx_type_reg(ltmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x2[row][0]), mx_type_reg(rchk->v8sf_16x2[col][0])); \
        mx_type_reg(rtmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x2[row][1]), mx_type_reg(rchk->v8sf_16x2[col][1])); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(rtmp)); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(v8sf_zero)); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(v8sf_zero)); \
        dchk->f32_16x16[row][col] += mx_type_val(ltmp)[0] + mx_type_val(ltmp)[4]; \
    }

    v8sf_t ltmp;
    v8sf_t rtmp;
    uint32_t i = 0;

    for (i = 0; i < F32S_IN_CACHE_LINE; i += 1) {
        v8sf_multiply_chunk_full(i,  0);
        v8sf_multiply_chunk_full(i,  1);
        v8sf_multiply_chunk_full(i,  2);
        v8sf_multiply_chunk_full(i,  3);
        v8sf_multiply_chunk_full(i,  4);
        v8sf_multiply_chunk_full(i,  5);
        v8sf_multiply_chunk_full(i,  6);
        v8sf_multiply_chunk_full(i,  7);
        v8sf_multiply_chunk_full(i,  8);
        v8sf_multiply_chunk_full(i,  9);
        v8sf_multiply_chunk_full(i, 10);
        v8sf_multiply_chunk_full(i, 11);
        v8sf_multiply_chunk_full(i, 12);
        v8sf_multiply_chunk_full(i, 13);
        v8sf_multiply_chunk_full(i, 14);
        v8sf_multiply_chunk_full(i, 15);
    } // for

#undef v8sf_multiply_chunk_full
} // v8sf_multiply_chunk_fully

static void v8sf_multiply_chunk_half_to_half(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8sf_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(tmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x1[row][0]), mx_type_reg(rchk->v8sf_16x1[col][0])); \
        mx_type_reg(tmp) = _mm256_maskload_ps(mx_type_val(tmp), mx_type_reg(*mask)); \
        mx_type_reg(tmp) = _mm256_hadd_ps(mx_type_reg(tmp), mx_type_reg(v8sf_zero)); \
        mx_type_reg(tmp) = _mm256_hadd_ps(mx_type_reg(tmp), mx_type_reg(v8sf_zero)); \
        dchk->f32_16x8[row][col] += mx_type_val(tmp)[0] + mx_type_val(tmp)[4]; \
    }

    v8sf_t tmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask = &v8si_mask[msks];

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case  8: v8sf_multiply_chunk_op(i, j); j += 1;
            case  7: v8sf_multiply_chunk_op(i, j); j += 1;
            case  6: v8sf_multiply_chunk_op(i, j); j += 1;
            case  5: v8sf_multiply_chunk_op(i, j); j += 1;
            case  4: v8sf_multiply_chunk_op(i, j); j += 1;
            case  3: v8sf_multiply_chunk_op(i, j); j += 1;
            case  2: v8sf_multiply_chunk_op(i, j); j += 1;
            case  1: v8sf_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8sf_multiply_chunk_op
} // v8sf_multiply_chunk_half_to_half

static void v8sf_multiply_chunk_half_to_full(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8sf_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(tmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x1[row][0]), mx_type_reg(rchk->v8sf_16x1[col][0])); \
        mx_type_reg(tmp) = _mm256_maskload_ps(mx_type_val(tmp), mx_type_reg(*mask)); \
        mx_type_reg(tmp) = _mm256_hadd_ps(mx_type_reg(tmp), mx_type_reg(v8sf_zero)); \
        mx_type_reg(tmp) = _mm256_hadd_ps(mx_type_reg(tmp), mx_type_reg(v8sf_zero)); \
        dchk->f32_16x16[row][col] += mx_type_val(tmp)[0] + mx_type_val(tmp)[4]; \
    }

    v8sf_t tmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask = &v8si_mask[msks];

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case 16: v8sf_multiply_chunk_op(i, j); j += 1;
            case 15: v8sf_multiply_chunk_op(i, j); j += 1;
            case 14: v8sf_multiply_chunk_op(i, j); j += 1;
            case 13: v8sf_multiply_chunk_op(i, j); j += 1;
            case 12: v8sf_multiply_chunk_op(i, j); j += 1;
            case 11: v8sf_multiply_chunk_op(i, j); j += 1;
            case 10: v8sf_multiply_chunk_op(i, j); j += 1;
            case  9: v8sf_multiply_chunk_op(i, j); j += 1;
            case  8: v8sf_multiply_chunk_op(i, j); j += 1;
            case  7: v8sf_multiply_chunk_op(i, j); j += 1;
            case  6: v8sf_multiply_chunk_op(i, j); j += 1;
            case  5: v8sf_multiply_chunk_op(i, j); j += 1;
            case  4: v8sf_multiply_chunk_op(i, j); j += 1;
            case  3: v8sf_multiply_chunk_op(i, j); j += 1;
            case  2: v8sf_multiply_chunk_op(i, j); j += 1;
            case  1: v8sf_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8sf_multiply_chunk_op
} // v8sf_multiply_chunk_half_to_full

static void v8sf_multiply_chunk_full_to_half(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8sf_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(ltmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x2[row][0]), mx_type_reg(rchk->v8sf_16x2[col][0])); \
        mx_type_reg(rtmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x2[row][1]), mx_type_reg(rchk->v8sf_16x2[col][1])); \
        mx_type_reg(ltmp) = _mm256_maskload_ps(mx_type_val(ltmp), mx_type_reg(*mask[0])); \
        mx_type_reg(rtmp) = _mm256_maskload_ps(mx_type_val(rtmp), mx_type_reg(*mask[1])); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(rtmp)); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(v8sf_zero)); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(v8sf_zero)); \
        dchk->f32_16x8[row][col] += mx_type_val(ltmp)[0] + mx_type_val(ltmp)[4]; \
    }

    v8sf_t ltmp;
    v8sf_t rtmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask[2] = {&v8si_mask[8], &v8si_mask[msks - 8]};

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case  8: v8sf_multiply_chunk_op(i, j); j += 1;
            case  7: v8sf_multiply_chunk_op(i, j); j += 1;
            case  6: v8sf_multiply_chunk_op(i, j); j += 1;
            case  5: v8sf_multiply_chunk_op(i, j); j += 1;
            case  4: v8sf_multiply_chunk_op(i, j); j += 1;
            case  3: v8sf_multiply_chunk_op(i, j); j += 1;
            case  2: v8sf_multiply_chunk_op(i, j); j += 1;
            case  1: v8sf_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8sf_multiply_chunk_op
} // v8sf_multiply_chunk_full_to_half

static void v8sf_multiply_chunk_full_to_full(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks)
{
#define v8sf_multiply_chunk_op(row, col) \
    { \
        mx_type_reg(ltmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x2[row][0]), mx_type_reg(rchk->v8sf_16x2[col][0])); \
        mx_type_reg(rtmp) = _mm256_mul_ps(mx_type_reg(lchk->v8sf_16x2[row][1]), mx_type_reg(rchk->v8sf_16x2[col][1])); \
        mx_type_reg(ltmp) = _mm256_maskload_ps(mx_type_val(ltmp), mx_type_reg(*mask[0])); \
        mx_type_reg(rtmp) = _mm256_maskload_ps(mx_type_val(rtmp), mx_type_reg(*mask[1])); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(rtmp)); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(v8sf_zero)); \
        mx_type_reg(ltmp) = _mm256_hadd_ps(mx_type_reg(ltmp), mx_type_reg(v8sf_zero)); \
        dchk->f32_16x16[row][col] += mx_type_val(ltmp)[0] + mx_type_val(ltmp)[4]; \
    }

    v8sf_t ltmp;
    v8sf_t rtmp;
    uint32_t i = 0;
    uint32_t j = 0;
    v8si_t * mask[2] = {&v8si_mask[8], &v8si_mask[msks - 8]};

    for (i = 0; i < dchk_rows; i += 1, j = 0) {
        switch (dchk_cols) {
            default: assert(1); break;
            case 16: v8sf_multiply_chunk_op(i, j); j += 1;
            case 15: v8sf_multiply_chunk_op(i, j); j += 1;
            case 14: v8sf_multiply_chunk_op(i, j); j += 1;
            case 13: v8sf_multiply_chunk_op(i, j); j += 1;
            case 12: v8sf_multiply_chunk_op(i, j); j += 1;
            case 11: v8sf_multiply_chunk_op(i, j); j += 1;
            case 10: v8sf_multiply_chunk_op(i, j); j += 1;
            case  9: v8sf_multiply_chunk_op(i, j); j += 1;
            case  8: v8sf_multiply_chunk_op(i, j); j += 1;
            case  7: v8sf_multiply_chunk_op(i, j); j += 1;
            case  6: v8sf_multiply_chunk_op(i, j); j += 1;
            case  5: v8sf_multiply_chunk_op(i, j); j += 1;
            case  4: v8sf_multiply_chunk_op(i, j); j += 1;
            case  3: v8sf_multiply_chunk_op(i, j); j += 1;
            case  2: v8sf_multiply_chunk_op(i, j); j += 1;
            case  1: v8sf_multiply_chunk_op(i, j);
        } // switch
    } // for

#undef v8sf_multiply_chunk_op
} // v8sf_multiply_chunk_full_to_full

typedef void (*v8sf_multiply_chunk_fn)(mx_chunk_ptr dchk, mx_chunk_ptr lchk, mx_chunk_ptr rchk, uint32_t dchk_rows, uint32_t dchk_cols, uint32_t msks);

void mstr_v8sf_multiply(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs)
{
    static v8sf_multiply_chunk_fn ops[3][3] = {
        {
            &v8sf_multiply_chunk_half_to_half, // [0][0]  <=  16x1 * 1x8
            &v8sf_multiply_chunk_half_to_full, // [0][1]  <=  16x1 * 1x15
            &v8sf_multiply_chunk_half_to_full, // [0][2]  <=  16x1 * 1x16
        },
        {
            &v8sf_multiply_chunk_full_to_half, // [1][0]  <=  16x15 * 15x8
            &v8sf_multiply_chunk_full_to_full, // [1][1]  <=  16x15 * 15x15
            &v8sf_multiply_chunk_full_to_full, // [1][2]  <=  16x15 * 15x16
        },
        {
            &v8sf_multiply_chunk_full_to_half, // [2][0]  <=   8x16 * 16x8
            &v8sf_multiply_chunk_full_to_full, // [2][1]  <=   8x16 * 16x15
            &v8sf_multiply_chunk_fully,        // [2][2]  <=  16x16 * 16x16 (Only this case)
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

    mstr_v8sf_init_zeros(dst);
    for (k = 0; k < mstr_chunks_in_height(rhs); k += 1) {
        for (j = 0; j < mstr_chunks_in_width(rhs); j += 1) {
            mstr_v8sf_transpose_chunk(rhs, k, j, &rchk, &rchk_rows, &rchk_cols);

            for (i = 0; i < mstr_chunks_in_height(lhs); i += 1) {
                lchk = mstr_locate_chunk(lhs, i, k, &lchk_rows, &lchk_cols);
                dchk = mstr_locate_chunk(dst, i, j, &dchk_rows, &dchk_cols);

                ssel = mx_ceil_to_multiples_of_8(rchk_cols) / 8 - 1 + (uint32_t)(lchk_cols == F32S_IN_CACHE_LINE);
                dsel = mx_ceil_to_multiples_of_8(dchk_cols) / 8 - 1 + (uint32_t)(dchk_cols == F32S_IN_CACHE_LINE);
                (*ops[ssel][dsel])(dchk, lchk, &rchk, dchk_rows, dchk_cols, lchk_cols);
            } // for
        } // for
    } // for
} // mstr_v8sf_multiply

#define v8sf_scalar_multiply_chunk_full(row) \
    { \
        mx_type_reg(dchk->v8sf_16x2[row][0]) = _mm256_mul_ps(mx_type_reg(*vals), mx_type_reg(schk->v8sf_16x2[row][0])); \
        mx_type_reg(dchk->v8sf_16x2[row][1]) = _mm256_mul_ps(mx_type_reg(*vals), mx_type_reg(schk->v8sf_16x2[row][1])); \
    }

#define v8sf_scalar_multiply_chunk_half(row) \
    { \
        mx_type_reg(dchk->v8sf_16x1[row][0]) = _mm256_mul_ps(mx_type_reg(*vals), mx_type_reg(schk->v8sf_16x1[row][0])); \
    }

static void v8sf_multiply_scalar_chunk_fully(mx_chunk_ptr dchk, mx_chunk_ptr schk, v8sf_t * vals)
{
    v8sf_scalar_multiply_chunk_full( 0);
    v8sf_scalar_multiply_chunk_full( 1);
    v8sf_scalar_multiply_chunk_full( 2);
    v8sf_scalar_multiply_chunk_full( 3);
    v8sf_scalar_multiply_chunk_full( 4);
    v8sf_scalar_multiply_chunk_full( 5);
    v8sf_scalar_multiply_chunk_full( 6);
    v8sf_scalar_multiply_chunk_full( 7);
    v8sf_scalar_multiply_chunk_full( 8);
    v8sf_scalar_multiply_chunk_full( 9);
    v8sf_scalar_multiply_chunk_full(10);
    v8sf_scalar_multiply_chunk_full(11);
    v8sf_scalar_multiply_chunk_full(12);
    v8sf_scalar_multiply_chunk_full(13);
    v8sf_scalar_multiply_chunk_full(14);
    v8sf_scalar_multiply_chunk_full(15);
} // v8sf_multiply_scalar_chunk_fully

static void v8sf_multiply_scalar_chunk_partly(mx_chunk_ptr dchk, mx_chunk_ptr schk, v8sf_t * vals, uint32_t src_rows, uint32_t src_cols)
{
    uint32_t i = 0;

    if (src_cols <= 8) {
        switch (src_rows) {
            default: assert(1); break;
            case 16: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case 15: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case 14: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case 13: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case 12: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case 11: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case 10: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  9: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  8: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  7: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  6: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  5: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  4: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  3: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  2: v8sf_scalar_multiply_chunk_half(i); i += 1;
            case  1: v8sf_scalar_multiply_chunk_half(i);
        } // switch
    } else {
        switch (src_rows) {
            default: assert(1); break;
            case 16: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case 15: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case 14: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case 13: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case 12: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case 11: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case 10: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  9: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  8: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  7: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  6: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  5: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  4: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  3: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  2: v8sf_scalar_multiply_chunk_full(i); i += 1;
            case  1: v8sf_scalar_multiply_chunk_full(i);
        } // switch
    } // if
} // v8sf_multiply_scalar_chunk_partly

void mstr_v8sf_multiply_scalar(mx_stor_ptr dst, mx_stor_ptr src, float val)
{
    v8sf_t vals;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t schk_rows = 0;
    uint32_t schk_cols = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    mx_chunk_ptr schk = NULL;
    mx_chunk_ptr dchk = NULL;

    mx_type_reg(vals) = _mm256_broadcast_ss(&val);

    for (i = 0; i < mstr_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_chunks_in_width(src); j += 1) {
            schk = mstr_locate_chunk(src, i, j, &schk_rows, &schk_cols);
            dchk = mstr_locate_chunk(dst, i, j, &dchk_rows, &dchk_cols);
            if (schk_rows == F32S_IN_CACHE_LINE && schk_cols == F32S_IN_CACHE_LINE) {
                v8sf_multiply_scalar_chunk_fully(dchk, schk, &vals);
            } else {
                v8sf_multiply_scalar_chunk_partly(dchk, schk, &vals, schk_rows, schk_cols);
            } // if
        } // for
    } // for
} // mstr_v8sf_multiply_scalar

void mstr_v8sf_transpose(mx_stor_ptr dst, mx_stor_ptr src)
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
            mstr_v8sf_transpose_chunk(src, i, j, dchk, &dchk_rows, &dchk_cols);
        } // for
    } // for
} // mstr_v8sf_transpose
