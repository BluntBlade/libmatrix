#include <assert.h>
#include <stdlib.h>

#include "mx_operation.h"

typedef struct MX_OPERATION {
    mx_chunk_t      lchk;
    mx_chunk_t      rchk;
    mx_chunk_t      mchk;

    uint32_t        lchk_rows;
    uint32_t        lchk_cols;

    uint32_t        rchk_rows;
    uint32_t        rchk_cols;

    uint32_t        mchk_rows;
    uint32_t        mchk_cols;

    bool            lchk_full;
    bool            rchk_full;
    bool            mchk_full;
} mx_oper_t;

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
        chk->v8si_pcks[row][0] = __builtin_ia32_paddd256(lhs->v8si_pcks[row][0], rhs->v8si_pcks[row][0]); \
        chk->v8si_pcks[row][1] = __builtin_ia32_paddd256(lhs->v8si_pcks[row][1], rhs->v8si_pcks[row][1]); \
    }

#define v8si_add_chunk_half(row) \
    { \
        chk->v8si_pcks[row][0] = __builtin_ia32_paddd256(lhs->v8si_pcks[row][0], rhs->v8si_pcks[row][0]); \
    }

static void v8si_add_chunk_fully(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
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

static void v8si_add_chunk_partly(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
{
    uint32_t i = 0;

    if (mp->lchk_cols <= 8) {
        switch (mp->lchk_rows) {
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
        switch (mp->lchk_rows) {
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

void mops_v8si_add(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr ret)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr mchk = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(lhs); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(lhs); j += 1) {
            lchk = mstr_v8si_copy_chunk(lhs, i, j, &mp->lchk, &mp->lchk_rows, &mp->lchk_cols, &mp->lchk_full);
            rchk = mstr_v8si_copy_chunk(rhs, i, j, &mp->rchk, &mp->rchk_rows, &mp->rchk_cols, &mp->rchk_full);
            mchk = mstr_v8si_copy_chunk(ret, i, j, &mp->mchk, &mp->mchk_rows, &mp->mchk_cols, &mp->mchk_full);
            if (mp->mchk_full) {
                v8si_add_chunk_fully(mchk, lchk, rchk, mp);
            } else {
                v8si_add_chunk_partly(mchk, lchk, rchk, mp);
                mstr_v8si_store_chunk(ret, i, j, mchk);
            } // if
        } // for
    } // for
} // mops_v8si_add

#define v8si_subtract_chunk_full(row) \
    { \
        chk->v8si_pcks[row][0] = __builtin_ia32_psubd256(lhs->v8si_pcks[row][0], rhs->v8si_pcks[row][0]); \
        chk->v8si_pcks[row][1] = __builtin_ia32_psubd256(lhs->v8si_pcks[row][1], rhs->v8si_pcks[row][1]); \
    }

#define v8si_subtract_chunk_half(row) \
    { \
        chk->v8si_pcks[row][0] = __builtin_ia32_psubd256(lhs->v8si_pcks[row][0], rhs->v8si_pcks[row][0]); \
    }

static void v8si_subtract_chunk_fully(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
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

static void v8si_subtract_chunk_partly(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
{
    uint32_t i = 0;

    if (mp->lchk_cols <= 8) {
        switch (mp->lchk_rows) {
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
        switch (mp->lchk_rows) {
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

void mops_v8si_subtract(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr ret)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr mchk = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(lhs); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(lhs); j += 1) {
            lchk = mstr_v8si_copy_chunk(lhs, i, j, &mp->lchk, &mp->lchk_rows, &mp->lchk_cols, &mp->lchk_full);
            rchk = mstr_v8si_copy_chunk(rhs, i, j, &mp->rchk, &mp->rchk_rows, &mp->rchk_cols, &mp->rchk_full);
            mchk = mstr_v8si_copy_chunk(ret, i, j, &mp->mchk, &mp->mchk_rows, &mp->mchk_cols, &mp->mchk_full);
            if (mp->mchk_full) {
                v8si_subtract_chunk_fully(mchk, lchk, rchk, mp);
            } else {
                v8si_subtract_chunk_partly(mchk, lchk, rchk, mp);
                mstr_v8si_store_chunk(ret, i, j, mchk);
            } // if
        } // for
    } // for
} // mops_v8si_subtract

#define v8si_multiply_chunk_full(row, col) \
    { \
        ltmp = __builtin_ia32_pmulld256(lhs->v8si_pcks[row][0], rhs->v8si_pcks[col][0]); \
        rtmp = __builtin_ia32_pmulld256(lhs->v8si_pcks[row][1], rhs->v8si_pcks[col][1]); \
        ltmp = __builtin_ia32_phaddd256(ltmp, rtmp); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        chk->i32_vals[row][col] += ltmp[0] + ltmp[4]; \
    }

#define v8si_multiply_chunk_full_with_mask(row, col, lmask, rmask) \
    { \
        ltmp = __builtin_ia32_pmulld256(lhs->v8si_pcks[row][0], rhs->v8si_pcks[col][0]); \
        rtmp = __builtin_ia32_pmulld256(lhs->v8si_pcks[row][1], rhs->v8si_pcks[col][1]); \
        ltmp &= lmask; \
        rtmp &= rmask; \
        ltmp = __builtin_ia32_phaddd256(ltmp, rtmp); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        chk->i32_vals[row][col] += ltmp[0] + ltmp[4]; \
    }

#define v8si_multiply_chunk_half_with_mask(row, col, mask) \
    { \
        ltmp = __builtin_ia32_pmulld256(lhs->v8si_pcks[row][0], rhs->v8si_pcks[col][0]); \
        ltmp &= mask; \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        ltmp = __builtin_ia32_phaddd256(ltmp, v8si_zero); \
        chk->i32_vals[row][col] += ltmp[0] + ltmp[4]; \
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
        } // for
    } // if
} // v8si_multiply_chunk_partly

void mops_v8si_multiply(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr ret)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    mx_chunk_ptr lchk = NULL;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr mchk = NULL;

    for (k = 0; k < mstr_v8si_chunks_in_height(rhs); k += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(rhs); j += 1) {
            rchk = mstr_v8si_transpose_chunk(rhs, k, j, &mp->rchk, &mp->rchk_rows, &mp->rchk_cols, &mp->rchk_full);

            for (i = 0; i < mstr_v8si_chunks_in_height(lhs); i += 1) {
                lchk = mstr_v8si_copy_chunk(lhs, i, k, &mp->lchk, &mp->lchk_rows, &mp->lchk_cols, &mp->lchk_full);
                mchk = mstr_v8si_copy_chunk(ret, i, j, &mp->mchk, &mp->mchk_rows, &mp->mchk_cols, &mp->mchk_full);
                if (mp->mchk_full) {
                    v8si_multiply_chunk_fully(mchk, lchk, rchk, mp);
                } else {
                    v8si_multiply_chunk_partly(mchk, lchk, rchk, mp);
                    mstr_v8si_store_chunk(ret, i, j, mchk);
                } // if
            } // for
        } // for
    } // for
} // mops_v8si_multiply

#define v8si_scalar_multiply_chunk_full(row) \
    { \
        chk->v8si_pcks[row][0] = __builtin_ia32_pmulld256(*lhs, rhs->v8si_pcks[row][0]); \
        chk->v8si_pcks[row][1] = __builtin_ia32_pmulld256(*lhs, rhs->v8si_pcks[row][1]); \
    }

#define v8si_scalar_multiply_chunk_half(row) \
    { \
        chk->v8si_pcks[row][0] = __builtin_ia32_pmulld256(*lhs, rhs->v8si_pcks[row][0]); \
    }

static void v8si_scalar_multiply_chunk_fully(mx_chunk_ptr chk, v8si_t * lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
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
} // v8si_scalar_multiply_chunk_fully

static void v8si_scalar_multiply_chunk_partly(mx_chunk_ptr chk, v8si_t * lhs, mx_chunk_ptr rhs, mx_oper_ptr mp)
{
    uint32_t i = 0;

    if (mp->rchk_cols <= 8) {
        switch (mp->rchk_rows) {
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
        switch (mp->rchk_rows) {
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
} // v8si_scalar_multiply_chunk_partly

void mops_v8si_scalar_multiply(mx_oper_ptr mp, int32_t lhs, mx_stor_ptr rhs, mx_stor_ptr ret)
{
    v8si_t src = {lhs, lhs, lhs, lhs, lhs, lhs, lhs, lhs};
    uint32_t i = 0;
    uint32_t j = 0;
    mx_chunk_ptr rchk = NULL;
    mx_chunk_ptr mchk = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(rhs); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(rhs); j += 1) {
            rchk = mstr_v8si_copy_chunk(rhs, i, j, &mp->rchk, &mp->rchk_rows, &mp->rchk_cols, &mp->rchk_full);
            mchk = mstr_v8si_copy_chunk(ret, i, j, &mp->mchk, &mp->mchk_rows, &mp->mchk_cols, &mp->mchk_full);
            if (mp->mchk_full) {
                v8si_scalar_multiply_chunk_fully(mchk, &src, rchk, mp);
            } else {
                v8si_scalar_multiply_chunk_partly(mchk, &src, rchk, mp);
                mstr_v8si_store_chunk(ret, i, j, mchk);
            } // if
        } // for
    } // for
} // mops_v8si_scalar_multiply
