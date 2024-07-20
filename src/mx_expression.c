#include <string.h>

#include "src/mx_expression.h"
#include "src/mx_v8si.h"
#include "src/mx_v8sf.h"

#define push_ins_with_3_args(me, fn, a0, a1, a2) \
    { \
        uint32_t iidx = ins_new_index(me); \
        if (iidx == 0) { \
            return false; \
        } \
        me->ins[iidx].op = fn; \
        me->ins[iidx].arg[0] = a0; \
        me->ins[iidx].arg[1] = a1; \
        me->ins[iidx].arg[2] = a2; \
        return true; \
    }

#define push_ins_with_4_args(me, fn, a0, a1, a2, a3) \
    { \
        uint32_t iidx = ins_new_index(me); \
        if (iidx == 0) { \
            return false; \
        } \
        me->ins[iidx].op = fn; \
        me->ins[iidx].arg[0] = a0; \
        me->ins[iidx].arg[1] = a1; \
        me->ins[iidx].arg[2] = a2; \
        me->ins[iidx].arg[3] = a3; \
        return true; \
    }

#define push_ins_with_5_args(me, fn, a0, a1, a2, a3, a4) \
    { \
        uint32_t iidx = ins_new_index(me); \
        if (iidx == 0) { \
            return false; \
        } \
        me->ins[iidx].op = fn; \
        me->ins[iidx].arg[0] = a0; \
        me->ins[iidx].arg[1] = a1; \
        me->ins[iidx].arg[2] = a2; \
        me->ins[iidx].arg[3] = a3; \
        me->ins[iidx].arg[4] = a4; \
        return true; \
    }

#define push_ins_with_6_args(me, fn, a0, a1, a2, a3, a4, a5) \
    { \
        uint32_t iidx = ins_new_index(me); \
        if (iidx == 0) { \
            return false; \
        } \
        me->ins[iidx].op = fn; \
        me->ins[iidx].arg[0] = a0; \
        me->ins[iidx].arg[1] = a1; \
        me->ins[iidx].arg[2] = a2; \
        me->ins[iidx].arg[3] = a3; \
        me->ins[iidx].arg[4] = a4; \
        me->ins[iidx].arg[5] = a5; \
        return true; \
    }

#define push_ins_with_7_args(me, fn, a0, a1, a2, a3, a4, a5, a6) \
    { \
        uint32_t iidx = ins_new_index(me); \
        if (iidx == 0) { \
            return false; \
        } \
        me->ins[iidx].op = fn; \
        me->ins[iidx].arg[0] = a0; \
        me->ins[iidx].arg[1] = a1; \
        me->ins[iidx].arg[2] = a2; \
        me->ins[iidx].arg[3] = a3; \
        me->ins[iidx].arg[4] = a4; \
        me->ins[iidx].arg[5] = a5; \
        me->ins[iidx].arg[6] = a6; \
        return true; \
    }

static uint32_t ins_new_index(mx_expr_ptr me)
{
    mexp_ins_ptr ins = NULL; 
    if (me->ins_len == me->ins_cap) {
        ins = calloc(sizeof(mexp_ins_t) * (me->ins_cap + me->ins_cap / 2), 1);
        if (! ins) {
            return 0;
        } // if
        memcpy(ins, me->ins, sizeof(mexp_ins_t) * me->ins_len);
        free(me->ins);
        me->ins = ins;
    } // if
    return me->ins_len++;
} // ins_new_index

mx_expr_ptr mexp_init(mx_expr_ptr me, uint32_t ins_cap)
{
    if (ins_cap < 2) {
        ins_cap = 8;
    } // if

    me->ins = calloc(sizeof(mexp_ins_t), ins_cap);
    if (! me->ins) {
        return NULL;
    } // if

    // The first element of the instruction or bonding array SHALL NOT be used.
    me->ins_len = 1;
    me->ins_cap = ins_cap;

    return me;
} // mexp_init

void mexp_clean(mx_expr_ptr me)
{
    free(me->ins);
} // mexp_clean

mx_expr_ptr mexp_create(uint32_t ins_cap)
{
    mx_expr_ptr me = calloc(sizeof(mx_expr_t), 1);
    if (! me) {
        return NULL;
    } // if
    if (! mexp_init(me, ins_cap)) {
        free(me);
        return NULL;
    } // if
    return me;
} // mexp_create

void mexp_destroy(mx_expr_ptr me)
{
    if (me) {
        mexp_clean(me);
        free(me);
    } // if
} // mexp_destroy

extern mx_expr_ptr mexp_duplicate(mx_expr_ptr me);

// ---- Scalar-related instruction-adding functions ---- //

static void i32_add(mexp_ins_ptr ins)
{
    *((int32_t *)ins->arg[0]) = *((int32_t *)ins->arg[1]) + *((int32_t *)ins->arg[2]);
} // i32_add

bool mexp_i32_add(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &i32_add, dst, src1, src2);
} // mexp_i32_add

static void i32_sub(mexp_ins_ptr ins)
{
    *((int32_t *)ins->arg[0]) = *((int32_t *)ins->arg[1]) - *((int32_t *)ins->arg[2]);
} // i32_sub

bool mexp_i32_sub(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &i32_sub, dst, src1, src2);
} // mexp_i32_sub

static void i32_mul(mexp_ins_ptr ins)
{
    *((int32_t *)ins->arg[0]) = *((int32_t *)ins->arg[1]) * *((int32_t *)ins->arg[2]);
} // i32_mul

bool mexp_i32_mul(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &i32_mul, dst, src1, src2);
} // mexp_i32_mul

static void i32_div(mexp_ins_ptr ins)
{
    *((int32_t *)ins->arg[0]) = *((int32_t *)ins->arg[1]) / *((int32_t *)ins->arg[2]);
} // i32_div

bool mexp_i32_div(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &i32_div, dst, src1, src2);
} // mexp_i32_div

// ---- //

static void f32_add(mexp_ins_ptr ins)
{
    *((float *)ins->arg[0]) = *((float *)ins->arg[1]) + *((float *)ins->arg[2]);
} // f32_add

bool mexp_f32_add(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &f32_add, dst, src1, src2);
} // mexp_f32_add

static void f32_sub(mexp_ins_ptr ins)
{
    *((float *)ins->arg[0]) = *((float *)ins->arg[1]) - *((float *)ins->arg[2]);
} // f32_sub

bool mexp_f32_sub(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &f32_sub, dst, src1, src2);
} // mexp_f32_sub

static void f32_mul(mexp_ins_ptr ins)
{
    *((float *)ins->arg[0]) = *((float *)ins->arg[1]) * *((float *)ins->arg[2]);
} // f32_mul

bool mexp_f32_mul(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &f32_mul, dst, src1, src2);
} // mexp_f32_mul

static void f32_div(mexp_ins_ptr ins)
{
    *((float *)ins->arg[0]) = *((float *)ins->arg[1]) / *((float *)ins->arg[2]);
} // f32_div

bool mexp_f32_div(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &f32_div, dst, src1, src2);
} // mexp_f32_div

// ---- Vector-related instruction-adding functions ---- //

static void v8si_add(mexp_ins_ptr ins)
{
    v8si_t * dst = ins->arg[0];
    v8si_t * lhs = ins->arg[1];
    v8si_t * rhs = ins->arg[2];
    mx_type_reg(*dst) = _mm256_add_epi32(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8si_add

bool mexp_v8si_add(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8si_add, dst, src1, src2);
} // mexp_v8si_add

static void v8si_sub(mexp_ins_ptr ins)
{
    v8si_t * dst = ins->arg[0];
    v8si_t * lhs = ins->arg[1];
    v8si_t * rhs = ins->arg[2];
    mx_type_reg(*dst) = _mm256_sub_epi32(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8si_sub

bool mexp_v8si_sub(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8si_sub, dst, src1, src2);
} // mexp_v8si_sub

static void v8si_mul(mexp_ins_ptr ins)
{
    v8si_t * dst = ins->arg[0];
    v8si_t * lhs = ins->arg[1];
    v8si_t * rhs = ins->arg[2];
    mx_type_reg(*dst) = _mm256_mullo_epi32(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8si_mul

bool mexp_v8si_mul(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8si_mul, dst, src1, src2);
} // mexp_v8si_mul

static void v8si_div(mexp_ins_ptr ins)
{
    v8si_t * dst = ins->arg[0];
    v8si_t * lhs = ins->arg[1];
    v8si_t * rhs = ins->arg[2];

    mx_type_val(*dst)[0] = mx_type_val(*lhs)[0] / mx_type_val(*rhs)[0];
    mx_type_val(*dst)[1] = mx_type_val(*lhs)[1] / mx_type_val(*rhs)[1];
    mx_type_val(*dst)[2] = mx_type_val(*lhs)[2] / mx_type_val(*rhs)[2];
    mx_type_val(*dst)[3] = mx_type_val(*lhs)[3] / mx_type_val(*rhs)[3];
    mx_type_val(*dst)[4] = mx_type_val(*lhs)[4] / mx_type_val(*rhs)[4];
    mx_type_val(*dst)[5] = mx_type_val(*lhs)[5] / mx_type_val(*rhs)[5];
    mx_type_val(*dst)[6] = mx_type_val(*lhs)[6] / mx_type_val(*rhs)[6];
    mx_type_val(*dst)[7] = mx_type_val(*lhs)[7] / mx_type_val(*rhs)[7];
} // v8si_div

bool mexp_v8si_div(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8si_div, dst, src1, src2);
} // mexp_v8si_div

static void v8si_load_i32(mexp_ins_ptr ins)
{
    int32_t * dst = ins->arg[0];
    int32_t * off = ins->arg[1];
    v8si_t * src = ins->arg[2];

    *dst = mx_type_val(*src)[*off];
} // v8si_load_i32

bool mexp_v8si_load_i32(mx_expr_ptr me, void * dst, void * off, void * src)
{
    push_ins_with_3_args(me, &v8si_load_i32, dst, off, src);
} // mexp_v8si_load_i32

static void v8si_store_i32(mexp_ins_ptr ins)
{
    v8si_t * dst = ins->arg[0];
    int32_t * off = ins->arg[1];
    int32_t * src = ins->arg[2];

    mx_type_val(*dst)[*off] = *src;
} // v8si_store_i32

bool mexp_v8si_store_i32(mx_expr_ptr me, void * dst, void * off, void * src)
{
    push_ins_with_3_args(me, &v8si_store_i32, dst, off, src);
} // mexp_v8si_store_i32

// ---- //

static void v8sf_add(mexp_ins_ptr ins)
{
    v8sf_t * dst = ins->arg[0];
    v8sf_t * lhs = ins->arg[1];
    v8sf_t * rhs = ins->arg[2];
    mx_type_reg(*dst) = _mm256_add_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_add

bool mexp_v8sf_add(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8sf_add, dst, src1, src2);
} // mexp_v8sf_add

static void v8sf_sub(mexp_ins_ptr ins)
{
    v8sf_t * dst = ins->arg[0];
    v8sf_t * lhs = ins->arg[1];
    v8sf_t * rhs = ins->arg[2];
    mx_type_reg(*dst) = _mm256_sub_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_sub

bool mexp_v8sf_sub(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8sf_sub, dst, src1, src2);
} // mexp_v8sf_sub

static void v8sf_mul(mexp_ins_ptr ins)
{
    v8sf_t * dst = ins->arg[0];
    v8sf_t * lhs = ins->arg[1];
    v8sf_t * rhs = ins->arg[2];
    mx_type_reg(*dst) = _mm256_mul_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_mul

bool mexp_v8sf_mul(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8sf_mul, dst, src1, src2);
} // mexp_v8sf_mul

static void v8sf_div(mexp_ins_ptr ins)
{
    v8sf_t * dst = ins->arg[0];
    v8sf_t * lhs = ins->arg[1];
    v8sf_t * rhs = ins->arg[2];
    mx_type_reg(*dst) = _mm256_div_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_div

bool mexp_v8sf_div(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8sf_div, dst, src1, src2);
} // mexp_v8sf_div

static void v8sf_load_f32(mexp_ins_ptr ins)
{
    float * dst = ins->arg[0];
    int32_t * off = ins->arg[1];
    v8sf_t * src = ins->arg[2];

    *dst = mx_type_val(*src)[*off];
} // v8sf_load_f32

bool mexp_v8sf_load_f32(mx_expr_ptr me, void * dst, void * off, void * src)
{
    push_ins_with_3_args(me, &v8sf_load_f32, dst, off, src);
} // mexp_v8sf_load_f32

static void v8sf_store_f32(mexp_ins_ptr ins)
{
    v8sf_t * dst = ins->arg[0];
    int32_t * off = ins->arg[1];
    float * src = ins->arg[2];

    mx_type_val(*dst)[*off] = *src;
} // v8sf_store_f32

bool mexp_v8sf_store_f32(mx_expr_ptr me, void * dst, void * off, void * src)
{
    push_ins_with_3_args(me, &v8sf_store_f32, dst, off, src);
} // mexp_v8sf_store_f32

// ---- Matrix-related instruction-adding functions ---- //

static void v8si_mat_add(mexp_ins_ptr ins)
{
    mstr_v8si_add(ins->arg[0], ins->arg[1], ins->arg[2]);
} // v8si_mat_add

bool mexp_v8si_mat_add(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8si_mat_add, dst, src1, src2);
} // mexp_v8si_mat_add

static void v8si_mat_sub(mexp_ins_ptr ins)
{
    mstr_v8si_subtract(ins->arg[0], ins->arg[1], ins->arg[2]);
} // v8si_mat_sub

bool mexp_v8si_mat_sub(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8si_mat_sub, dst, src1, src2);
} // mexp_v8si_mat_sub

static void v8si_mat_mul(mexp_ins_ptr ins)
{
    mstr_v8si_multiply(ins->arg[0], ins->arg[1], ins->arg[2]);
} // v8si_mat_mul

bool mexp_v8si_mat_mul(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8si_mat_mul, dst, src1, src2);
} // mexp_v8si_mat_mul

static void v8si_mat_mul_by_scr(mexp_ins_ptr ins)
{
    mstr_v8si_multiply_scalar(ins->arg[0], ins->arg[1], *((int32_t *)ins->arg[2]));
} // v8si_mat_mul_by_scr

bool mexp_v8si_mat_mul_by_scr(mx_expr_ptr me, void * dst, void * mat, void * i32)
{
    push_ins_with_3_args(me, &v8si_mat_mul_by_scr, dst, mat, i32);
} // mexp_v8si_mat_mul_by_scr

static void v8si_mat_load_row_vec(mexp_ins_ptr ins)
{
    v8si_t * dst = ins->arg[0];
    uint32_t val_ridx = *((uint32_t *)ins->arg[1]);
    uint32_t val_cidx = *((uint32_t *)ins->arg[2]);
    int32_t row_off = *((int32_t *)ins->arg[3]);
    int32_t col_off = *((int32_t *)ins->arg[4]);
    int32_t def_val = *((int32_t *)ins->arg[5]);
    mx_stor_ptr src = ins->arg[6];

    mstr_v8si_load_row_vector(src, val_ridx, val_cidx, row_off, col_off, def_val, dst);
} // v8si_mat_load_row_vec

bool mexp_v8si_mat_load_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * row_off, void * col_off, void * dval, void * src)
{
    push_ins_with_7_args(me, &v8si_mat_load_row_vec, dst, row, col, row_off, col_off, dval, src);
} // mexp_v8si_mat_load_row_vec

static void v8si_mat_store_row_vec(mexp_ins_ptr ins)
{
    mx_stor_ptr dst = ins->arg[0];
    uint32_t val_ridx = *((uint32_t *)ins->arg[1]);
    uint32_t val_cidx = *((uint32_t *)ins->arg[2]);
    int32_t row_off = *((int32_t *)ins->arg[3]);
    int32_t col_off = *((int32_t *)ins->arg[4]);
    v8si_t * src = ins->arg[5];

    mstr_v8si_store_row_vector(dst, val_ridx, val_cidx, row_off, col_off, src);
} // v8si_mat_store_row_vec

bool mexp_v8si_mat_store_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * row_off, void * col_off, void * src)
{
    push_ins_with_6_args(me, &v8si_mat_store_row_vec, dst, row, col, row_off, col_off, src);
} // mexp_v8si_mat_store_row_vec

// ---- //

static void v8sf_mat_add(mexp_ins_ptr ins)
{
    mstr_v8sf_add(ins->arg[0], ins->arg[1], ins->arg[2]);
} // v8sf_mat_add

bool mexp_v8sf_mat_add(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8sf_mat_add, dst, src1, src2);
} // mexp_v8sf_mat_add

static void v8sf_mat_sub(mexp_ins_ptr ins)
{
    mstr_v8sf_subtract(ins->arg[0], ins->arg[1], ins->arg[2]);
} // v8sf_mat_sub

bool mexp_v8sf_mat_sub(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8sf_mat_sub, dst, src1, src2);
} // mexp_v8sf_mat_sub

static void v8sf_mat_mul(mexp_ins_ptr ins)
{
    mstr_v8sf_multiply(ins->arg[0], ins->arg[1], ins->arg[2]);
} // v8sf_mat_mul

bool mexp_v8sf_mat_mul(mx_expr_ptr me, void * dst, void * src1, void * src2)
{
    push_ins_with_3_args(me, &v8sf_mat_mul, dst, src1, src2);
} // mexp_v8sf_mat_mul

static void v8sf_mat_mul_by_scr(mexp_ins_ptr ins)
{
    mstr_v8sf_multiply_scalar(ins->arg[0], ins->arg[1], *((float *)ins->arg[2]));
} // v8sf_mat_mul_by_scr

bool mexp_v8sf_mat_mul_by_scr(mx_expr_ptr me, void * dst, void * mat, void * f32)
{
    push_ins_with_3_args(me, &v8sf_mat_mul_by_scr, dst, mat, f32);
} // mexp_v8sf_mat_mul_by_scr

static void v8sf_mat_load_row_vec(mexp_ins_ptr ins)
{
    v8sf_t * dst = ins->arg[0];
    uint32_t val_ridx = *((uint32_t *)ins->arg[1]);
    uint32_t val_cidx = *((uint32_t *)ins->arg[2]);
    int32_t row_off = *((int32_t *)ins->arg[3]);
    int32_t col_off = *((int32_t *)ins->arg[4]);
    float def_val = *((float *)ins->arg[5]);
    mx_stor_ptr src = ins->arg[6];

    mstr_v8sf_load_row_vector(src, val_ridx, val_cidx, row_off, col_off, def_val, dst);
} // v8sf_mat_load_row_vec

bool mexp_v8sf_mat_load_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * row_off, void * col_off, void * dval, void * src)
{
    push_ins_with_7_args(me, &v8sf_mat_load_row_vec, dst, row, col, row_off, col_off, dval, src);
} // mexp_v8sf_mat_load_row_vec

static void v8sf_mat_store_row_vec(mexp_ins_ptr ins)
{
    mx_stor_ptr dst = ins->arg[0];
    uint32_t val_ridx = *((uint32_t *)ins->arg[1]);
    uint32_t val_cidx = *((uint32_t *)ins->arg[2]);
    int32_t row_off = *((int32_t *)ins->arg[3]);
    int32_t col_off = *((int32_t *)ins->arg[4]);
    v8sf_t * src = ins->arg[5];

    mstr_v8sf_store_row_vector(dst, val_ridx, val_cidx, row_off, col_off, src);
} // v8sf_mat_store_row_vec

bool mexp_v8sf_mat_store_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * row_off, void * col_off, void * src)
{
    push_ins_with_6_args(me, &v8sf_mat_store_row_vec, dst, row, col, row_off, col_off, src);
} // mexp_v8sf_mat_store_row_vec

// ---- Evaluation functions ---- //

void mexp_evaluate(mx_expr_ptr me)
{
    uint32_t i = 0;
    uint32_t n = 0;
    uint32_t r = 0;
    mexp_ins_ptr ins = NULL;

    r = (me->ins_len - 1) % 8;
    n = (me->ins_len - 1) / 8 + 1;
    switch (r) {
        case 8: do { ins = &me->ins[++i]; (ins->op)(ins);
        case 7:      ins = &me->ins[++i]; (ins->op)(ins);
        case 6:      ins = &me->ins[++i]; (ins->op)(ins);
        case 5:      ins = &me->ins[++i]; (ins->op)(ins);
        case 4:      ins = &me->ins[++i]; (ins->op)(ins);
        case 3:      ins = &me->ins[++i]; (ins->op)(ins);
        case 2:      ins = &me->ins[++i]; (ins->op)(ins);
        case 1:      ins = &me->ins[++i]; (ins->op)(ins);
                } while (--n > 0);
        default: break;
    } // switch
} // mexp_evaluate

void mexp_v8si_filter(mx_expr_ptr me, void * row, void * col, void * src)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t l = 0;
    uint32_t row_in_chk = 0;
    uint32_t col_in_chk = 0;
    void * base = NULL;

    for (i = 0; i < mstr_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_chunks_in_width(src); j += 1) {
            base = mstr_locate_chunk(src, i, j, &row_in_chk, &col_in_chk);

            for (k = I32_VALS_IN_CACHE_LINE * i; k < I32_VALS_IN_CACHE_LINE * i + row_in_chk; k += 1) {
                for (l = I32_VALS_IN_CACHE_LINE * j; l < I32_VALS_IN_CACHE_LINE * j + col_in_chk; l += 8) {
                    *((uint32_t *)row) = k;
                    *((uint32_t *)col) = l;
                    mexp_evaluate(me);
                } // for
            } // for
        } // for
    } // for
} // mexp_filter
