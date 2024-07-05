#include <assert.h>
#include <stdlib.h>

#include "src/mx_common.h"
#include "src/v8si_storage.h"
#include "src/v8si_operation.h"

mx_oper_ptr mops_v8si_create(void)
{
    return calloc(sizeof(mx_oper_t), 1);
} // mops_v8si_create 

void mops_v8si_destroy(mx_oper_ptr mp)
{
    free(mp);
} // mops_v8si_destroy

#define v8si_add_chunk_full(row) \
    { \
        chk->v8si_16x2[row][0] = __builtin_ia32_paddd256(lhs->v8si_16x2[row][0], rhs->v8si_16x2[row][0]); \
        chk->v8si_16x2[row][1] = __builtin_ia32_paddd256(lhs->v8si_16x2[row][1], rhs->v8si_16x2[row][1]); \
    }

#define v8si_add_chunk_half(row) \
    { \
        chk->v8si_16x1[row][0] = __builtin_ia32_paddd256(lhs->v8si_16x1[row][0], rhs->v8si_16x1[row][0]); \
    }

static void v8si_add_chunk_fully(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs)
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

static void v8si_add_chunk_partly(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, uint32_t lchk_rows, uint32_t lchk_cols)
{
    uint32_t i = 0;

    if (lchk_cols <= 8) {
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

void mops_v8si_add(mx_stor_ptr ms, mx_stor_ptr lhs, mx_stor_ptr rhs)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t lchk_rows = 0;
    uint32_t lchk_cols = 0;
    uint32_t rchk_rows = 0;
    uint32_t rchk_cols = 0;
    uint32_t mchk_rows = 0;
    uint32_t mchk_cols = 0;
    bool lchk_full = false;
    bool rchk_full = false;
    bool mchk_full = false;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr mchk = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(lhs); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(lhs); j += 1) {
            lchk = mstr_v8si_locate_chunk(lhs, i, j, &lchk_rows, &lchk_cols, &lchk_full);
            rchk = mstr_v8si_locate_chunk(rhs, i, j, &rchk_rows, &rchk_cols, &rchk_full);
            mchk = mstr_v8si_locate_chunk(ms, i, j, &mchk_rows, &mchk_cols, &mchk_full);
            if (lchk_full && rchk_full) {
                v8si_add_chunk_fully(mchk, lchk, rchk);
            } else {
                v8si_add_chunk_partly(mchk, lchk, rchk, lchk_rows, lchk_cols);
            } // if
        } // for
    } // for
} // mops_v8si_add

#define v8si_subtract_chunk_full(row) \
    { \
        chk->v8si_16x2[row][0] = __builtin_ia32_psubd256(lhs->v8si_16x2[row][0], rhs->v8si_16x2[row][0]); \
        chk->v8si_16x2[row][1] = __builtin_ia32_psubd256(lhs->v8si_16x2[row][1], rhs->v8si_16x2[row][1]); \
    }

#define v8si_subtract_chunk_half(row) \
    { \
        chk->v8si_16x1[row][0] = __builtin_ia32_psubd256(lhs->v8si_8x1[row][0], rhs->v8si_16x1[row][0]); \
    }

static void v8si_subtract_chunk_fully(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs)
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

static void v8si_subtract_chunk_partly(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, uint32_t lchk_rows, uint32_t lchk_cols)
{
    uint32_t i = 0;

    if (lchk_cols <= 8) {
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

void mops_v8si_subtract(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t lchk_rows = 0;
    uint32_t lchk_cols = 0;
    uint32_t rchk_rows = 0;
    uint32_t rchk_cols = 0;
    uint32_t mchk_rows = 0;
    uint32_t mchk_cols = 0;
    bool lchk_full = false;
    bool rchk_full = false;
    bool mchk_full = false;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr mchk = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(lhs); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(lhs); j += 1) {
            lchk = mstr_v8si_locate_chunk(lhs, i, j, &lchk_rows, &lchk_cols, &lchk_full);
            rchk = mstr_v8si_locate_chunk(rhs, i, j, &rchk_rows, &rchk_cols, &rchk_full);
            mchk = mstr_v8si_locate_chunk(dst, i, j, &mchk_rows, &mchk_cols, &mchk_full);
            if (lchk_full && rchk_full) {
                v8si_subtract_chunk_fully(mchk, lchk, rchk);
            } else {
                v8si_subtract_chunk_partly(mchk, lchk, rchk, lchk_rows, lchk_cols);
            } // if
        } // for
    } // for
} // mops_v8si_subtract

#define v8si_multiply_chunk_full(row, col) \
    { \
        ltmp = __builtin_ia32_pmulld256(lhs->v8si_16x2[row][0], rhs->v8si_16x2[col][0]); \
        rtmp = __builtin_ia32_pmulld256(lhs->v8si_16x2[row][1], rhs->v8si_16x2[col][1]); \
        ltmp = __builtin_ia32_phaddd256(ltmp, rtmp); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        chk->i32_16x16[row][col] += ltmp[0] + ltmp[4]; \
    }

#define v8si_multiply_chunk_full_with_mask(row, col, lmask, rmask) \
    { \
        ltmp = __builtin_ia32_pmulld256(lhs->v8si_16x2[row][0], rhs->v8si_16x2[col][0]); \
        rtmp = __builtin_ia32_pmulld256(lhs->v8si_16x2[row][1], rhs->v8si_16x2[col][1]); \
        ltmp &= lmask; \
        rtmp &= rmask; \
        ltmp = __builtin_ia32_phaddd256(ltmp, rtmp); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        base[col] += ltmp[0] + ltmp[4]; \
    }

#define v8si_multiply_chunk_half_with_mask(row, col, mask) \
    { \
        ltmp = __builtin_ia32_pmulld256(lhs->v8si_8x1[row][0], rhs->v8si_16x2[col][0]); \
        ltmp &= mask; \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        base[col] += ltmp[0] + ltmp[4]; \
    }

static void v8si_multiply_chunk_fully(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
{
    v8si_t ltmp;
    v8si_t rtmp;
    uint32_t i = 0;

    for (i = 0; i < I32_VALS_IN_CACHE_LINE; i += 1) {
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
} // v8si_multiply_chunk_fully

static void v8si_multiply_chunk_partly(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
{
    v8si_t ltmp;
    v8si_t rtmp;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t col_off = mx_round_to_multiples_of_8(mp->mchk_cols);
    int32_t * base = (int32_t *)&chk->i32_16x16[0][0];
    v8si_t * mask[2];

    mask[0] = &v8si_mask[(mp->lchk_cols <= 8) ? (mp->lchk_cols) : 8];
    mask[1] = &v8si_mask[(mp->lchk_cols <= 8) ? 0 : (mp->lchk_cols - 8)];

    if (mp->lchk_cols <= 8) {
        for (i = 0; i < mp->lchk_rows; i += 1, j = 0) {
            switch (mp->rchk_rows) { // Chunk on the right hand side has swapped rows and cols.
                default: assert(1); break;
                case 16: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case 15: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case 14: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case 13: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case 12: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case 11: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case 10: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  9: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  8: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  7: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  6: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  5: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  4: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  3: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  2: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]); j += 1;
                case  1: v8si_multiply_chunk_half_with_mask(i, j, *mask[0]);
            } // switch
            base += col_off;
        } // for
    } else {
        for (i = 0; i < mp->lchk_rows; i += 1, j = 0) {
            switch (mp->rchk_rows) { // Chunk on the right hand side has swapped rows and cols.
                default: assert(1); break;
                case 16: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case 15: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case 14: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case 13: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case 12: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case 11: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case 10: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  9: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  8: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  7: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  6: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  5: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  4: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  3: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  2: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]); j += 1;
                case  1: v8si_multiply_chunk_full_with_mask(i, j, *mask[0], *mask[1]);
            } // switch
            base += col_off;
        } // for
    } // if
} // v8si_multiply_chunk_partly

void mops_v8si_multiply(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr dst)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr mchk = NULL;

    mstr_v8si_init_zeros(dst);
    for (k = 0; k < mstr_v8si_chunks_in_height(rhs); k += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(rhs); j += 1) {
            rchk = mstr_v8si_transpose_chunk(rhs, k, j, &mp->rchk, &mp->rchk_rows, &mp->rchk_cols, &mp->rchk_full);

            for (i = 0; i < mstr_v8si_chunks_in_height(lhs); i += 1) {
                lchk = mstr_v8si_locate_chunk(lhs, i, k, &mp->lchk_rows, &mp->lchk_cols, &mp->lchk_full);
                mchk = mstr_v8si_locate_chunk(dst, i, j, &mp->mchk_rows, &mp->mchk_cols, &mp->mchk_full);
                if (mp->lchk_full && mp->rchk_full) {
                    v8si_multiply_chunk_fully(mchk, lchk, rchk, mp);
                } else {
                    v8si_multiply_chunk_partly(mchk, lchk, rchk, mp);
                } // if
            } // for
        } // for
    } // for
} // mops_v8si_multiply

#define v8si_scalar_multiply_chunk_full(row) \
    { \
        dst->v8si_16x2[row][0] = __builtin_ia32_pmulld256(*vals, src->v8si_16x2[row][0]); \
        dst->v8si_16x2[row][1] = __builtin_ia32_pmulld256(*vals, src->v8si_16x2[row][1]); \
    }

#define v8si_scalar_multiply_chunk_half(row) \
    { \
        dst->v8si_16x1[row][0] = __builtin_ia32_pmulld256(*vals, src->v8si_16x1[row][0]); \
    }

static void v8si_multiply_scalar_chunk_fully(mx_chunk_ptr dst, mx_chunk_ptr src, v8si_t * vals)
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

static void v8si_multiply_scalar_chunk_partly(mx_chunk_ptr dst, mx_chunk_ptr src, v8si_t * vals, uint32_t src_rows, uint32_t src_cols)
{
    uint32_t i = 0;

    if (src_cols <= 8) {
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

void mops_v8si_multiply_scalar(mx_stor_ptr dst, mx_stor_ptr src, int32_t val)
{
    v8si_t vals = {val, val, val, val, val, val, val, val};
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t schk_rows = 0;
    uint32_t schk_cols = 0;
    uint32_t mchk_rows = 0;
    uint32_t mchk_cols = 0;
    bool schk_full = false;
    bool mchk_full = false;
    mx_chunk_ptr schk = NULL;
    mx_chunk_ptr mchk = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(src); j += 1) {
            schk = mstr_v8si_locate_chunk(src, i, j, &schk_rows, &schk_cols, &schk_full);
            mchk = mstr_v8si_locate_chunk(dst, i, j, &mchk_rows, &mchk_cols, &mchk_full);
            if (schk_full) {
                v8si_multiply_scalar_chunk_fully(mchk, schk, &vals);
            } else {
                v8si_multiply_scalar_chunk_partly(mchk, schk, &vals, schk_rows, schk_cols);
            } // if
        } // for
    } // for
} // mops_v8si_multiply_scalar

void mops_v8si_transpose(mx_stor_ptr dst, mx_stor_ptr src)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t schk_rows = 0;
    uint32_t schk_cols = 0;
    uint32_t dchk_rows = 0;
    uint32_t dchk_cols = 0;
    bool schk_full = false;
    bool dchk_full = false;
    void * base = NULL;
    mx_chunk_ptr dchk = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(src); j += 1) {
            base = mstr_v8si_locate_chunk(src, i, j, &schk_rows, &schk_cols, &schk_full);
            dchk = mstr_v8si_locate_chunk(dst, j, i, &dchk_rows, &dchk_cols, &dchk_full);
            mstr_v8si_transpose_chunk(src, i, j, dchk, &dchk_rows, &dchk_cols, &dchk_full);
        } // for
    } // for
} // mops_v8si_transpose
