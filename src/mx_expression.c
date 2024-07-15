#include <string.h>

#include "src/mx_expression.h"
#include "src/v8si_storage.h"
#include "src/v8si_operation.h"
#include "src/v8sf_storage.h"
#include "src/v8sf_operation.h"

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

enum
{
    MEXP_BOND_I32               = 0x01,
    MEXP_BOND_I64               = 0x02,
    MEXP_BOND_F32               = 0x03,
    MEXP_BOND_D64               = 0x04,
    MEXP_BOND_V8SI_VEC          = 0x11,
    MEXP_BOND_V8SI_MAT          = 0x12,
    MEXP_BOND_V8SF_VEC          = 0x21,
    MEXP_BOND_V8SF_MAT          = 0x22,
};

static uint32_t bond_new_index(mx_expr_ptr me)
{
    mexp_bond_ptr bond = NULL; 
    if (me->bond_len == me->bond_cap) {
        bond = calloc(sizeof(mexp_bond_t) * (me->bond_cap + me->bond_cap / 2), 1);
        if (! bond) {
            return 0;
        } // if
        memcpy(bond, me->bond, sizeof(mexp_bond_t) * me->bond_len);
        free(me->bond);
        me->bond = bond;
    } // if
    return me->bond_len++;
} // bond_new_index

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

mx_expr_ptr mexp_init(mx_expr_ptr me, uint32_t ins_cap, uint32_t bond_cap)
{
    if (ins_cap < 2) {
        ins_cap = 8;
    } // if
    if (bond_cap < 2) {
        bond_cap = 8;
    } // if

    me->ins = calloc(sizeof(mexp_ins_t), ins_cap);
    if (! me->ins) {
        return NULL;
    } // if
    me->bond = calloc(sizeof(mexp_bond_t), bond_cap);
    if (! me->bond) {
        free(me->ins);
        return NULL;
    } // if

    // The first element of the instruction or bonding array SHALL NOT be used.
    me->ins_len = 1;
    me->ins_cap = ins_cap;
    me->bond_len = 1;
    me->bond_cap = bond_cap;

    return me;
} // mexp_init

void mexp_clean(mx_expr_ptr me)
{
    free(me->ins);
    free(me->bond);
} // mexp_clean

mx_expr_ptr mexp_create(uint32_t ins_cap, uint32_t bond_cap)
{
    mx_expr_ptr me = calloc(sizeof(mx_expr_t), 1);
    if (! me) {
        return NULL;
    } // if
    if (! mexp_init(me, ins_cap, bond_cap)) {
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

uint32_t mexp_i32_bind_scalar(mx_expr_ptr me, int32_t * val)
{
    uint32_t bidx = bond_new_index(me);
    if (bidx > 0) {
        me->bond[bidx].type = MEXP_BOND_I32;
        me->bond[bidx].i32 = val;
    } // if
    return bidx;
} // mexp_i32_bind_scalar

void mexp_i32_rebind_scalar(mx_expr_ptr me, uint32_t bidx, int32_t * val)
{
    me->bond[bidx].type = MEXP_BOND_I32;
    me->bond[bidx].i32 = val;
} // mexp_i32_rebind_scalar

uint32_t mexp_f32_bind_scalar(mx_expr_ptr me, float * val)
{
    uint32_t bidx = bond_new_index(me);
    if (bidx > 0) {
        me->bond[bidx].type = MEXP_BOND_F32;
        me->bond[bidx].f32 = val;
    } // if
    return bidx;
} // mexp_f32_bind_scalar

void mexp_f32_rebind_scalar(mx_expr_ptr me, uint32_t bidx, float * val)
{
    me->bond[bidx].type = MEXP_BOND_F32;
    me->bond[bidx].f32 = val;
} // mexp_f32_rebind_scalar

uint32_t mexp_v8si_bind_vector(mx_expr_ptr me, v8si_t * vec)
{
    uint32_t bidx = bond_new_index(me);
    if (bidx > 0) {
        me->bond[bidx].type = MEXP_BOND_V8SI_VEC;
        me->bond[bidx].v8si = vec;
    } // if
    return bidx;
} // mexp_v8si_bind_vector

void mexp_v8si_rebind_vector(mx_expr_ptr me, uint32_t bidx, v8si_t * vec)
{
    me->bond[bidx].type = MEXP_BOND_V8SI_VEC;
    me->bond[bidx].v8si = vec;
} // mexp_v8si_rebind_vector

uint32_t mexp_v8si_bind_matrix(mx_expr_ptr me, mx_stor_ptr mx)
{
    uint32_t bidx = bond_new_index(me);
    if (bidx > 0) {
        me->bond[bidx].type = MEXP_BOND_V8SI_MAT;
        me->bond[bidx].mat = mx;
    } // if
    return bidx;
} // mexp_v8si_bind_matrix

void mexp_v8si_rebind_matrix(mx_expr_ptr me, uint32_t bidx, mx_stor_ptr mx)
{
    me->bond[bidx].type = MEXP_BOND_V8SI_MAT;
    me->bond[bidx].mat = mx;
} // mexp_v8si_rebind_matrix

uint32_t mexp_v8sf_bind_vector(mx_expr_ptr me, v8sf_t * vec)
{
    uint32_t bidx = bond_new_index(me);
    if (bidx > 0) {
        me->bond[bidx].type = MEXP_BOND_V8SI_VEC;
        me->bond[bidx].v8sf = vec;
    } // if
    return bidx;
} // mexp_v8sf_bind_vector

void mexp_v8sf_rebind_vector(mx_expr_ptr me, uint32_t bidx, v8sf_t * vec)
{
    me->bond[bidx].type = MEXP_BOND_V8SI_VEC;
    me->bond[bidx].v8sf = vec;
} // mexp_v8sf_rebind_vector

uint32_t mexp_v8sf_bind_matrix(mx_expr_ptr me, mx_stor_ptr mx)
{
    uint32_t bidx = bond_new_index(me);
    if (bidx > 0) {
        me->bond[bidx].type = MEXP_BOND_V8SF_MAT;
        me->bond[bidx].mat = mx;
    } // if
    return bidx;
} // mexp_v8sf_bind_matrix

void mexp_v8sf_rebind_matrix(mx_expr_ptr me, uint32_t bidx, mx_stor_ptr mx)
{
    me->bond[bidx].type = MEXP_BOND_V8SF_MAT;
    me->bond[bidx].mat = mx;
} // mexp_v8sf_rebind_matrix

// ---- Scalar-related instruction-adding functions ---- //

static void i32_add(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].i32) = *(bond[ins->arg[1]].i32) + *(bond[ins->arg[2]].i32);
} // i32_add

bool mexp_i32_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &i32_add, dst_idx, src1_idx, src2_idx);
} // mexp_i32_add

static void i32_sub(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].i32) = *(bond[ins->arg[1]].i32) - *(bond[ins->arg[2]].i32);
} // i32_sub

bool mexp_i32_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &i32_sub, dst_idx, src1_idx, src2_idx);
} // mexp_i32_sub

static void i32_mul(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].i32) = *(bond[ins->arg[1]].i32) * *(bond[ins->arg[2]].i32);
} // i32_mul

bool mexp_i32_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &i32_mul, dst_idx, src1_idx, src2_idx);
} // mexp_i32_mul

static void i32_div(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].i32) = *(bond[ins->arg[1]].i32) / *(bond[ins->arg[2]].i32);
} // i32_div

bool mexp_i32_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &i32_div, dst_idx, src1_idx, src2_idx);
} // mexp_i32_div

// ---- //

static void f32_add(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].f32) = *(bond[ins->arg[1]].f32) + *(bond[ins->arg[2]].f32);
} // f32_add

bool mexp_f32_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &f32_add, dst_idx, src1_idx, src2_idx);
} // mexp_f32_add

static void f32_sub(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].f32) = *(bond[ins->arg[1]].f32) - *(bond[ins->arg[2]].f32);
} // f32_sub

bool mexp_f32_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &f32_sub, dst_idx, src1_idx, src2_idx);
} // mexp_f32_sub

static void f32_mul(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].f32) = *(bond[ins->arg[1]].f32) * *(bond[ins->arg[2]].f32);
} // f32_mul

bool mexp_f32_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &f32_mul, dst_idx, src1_idx, src2_idx);
} // mexp_f32_mul

static void f32_div(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    *(bond[ins->arg[0]].f32) = *(bond[ins->arg[1]].f32) / *(bond[ins->arg[2]].f32);
} // f32_div

bool mexp_f32_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &f32_div, dst_idx, src1_idx, src2_idx);
} // mexp_f32_div

// ---- Vector-related instruction-adding functions ---- //

static void v8si_add(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8si_t * dst = bond[ins->arg[0]].v8si;
    v8si_t * lhs = bond[ins->arg[1]].v8si;
    v8si_t * rhs = bond[ins->arg[2]].v8si;
    mx_type_reg(*dst) = _mm256_add_epi32(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8si_add

bool mexp_v8si_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8si_add, dst_idx, src1_idx, src2_idx);
} // mexp_v8si_add

static void v8si_sub(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8si_t * dst = bond[ins->arg[0]].v8si;
    v8si_t * lhs = bond[ins->arg[1]].v8si;
    v8si_t * rhs = bond[ins->arg[2]].v8si;
    mx_type_reg(*dst) = _mm256_sub_epi32(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8si_sub

bool mexp_v8si_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8si_sub, dst_idx, src1_idx, src2_idx);
} // mexp_v8si_sub

static void v8si_mul(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8si_t * dst = bond[ins->arg[0]].v8si;
    v8si_t * lhs = bond[ins->arg[1]].v8si;
    v8si_t * rhs = bond[ins->arg[2]].v8si;
    mx_type_reg(*dst) = _mm256_mullo_epi32(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8si_mul

bool mexp_v8si_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8si_mul, dst_idx, src1_idx, src2_idx);
} // mexp_v8si_mul

static void v8si_div(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8si_t * dst = bond[ins->arg[0]].v8si;
    v8si_t * lhs = bond[ins->arg[1]].v8si;
    v8si_t * rhs = bond[ins->arg[2]].v8si;

    mx_type_val(*dst)[0] = mx_type_val(*lhs)[0] / mx_type_val(*rhs)[0];
    mx_type_val(*dst)[1] = mx_type_val(*lhs)[1] / mx_type_val(*rhs)[1];
    mx_type_val(*dst)[2] = mx_type_val(*lhs)[2] / mx_type_val(*rhs)[2];
    mx_type_val(*dst)[3] = mx_type_val(*lhs)[3] / mx_type_val(*rhs)[3];
    mx_type_val(*dst)[4] = mx_type_val(*lhs)[4] / mx_type_val(*rhs)[4];
    mx_type_val(*dst)[5] = mx_type_val(*lhs)[5] / mx_type_val(*rhs)[5];
    mx_type_val(*dst)[6] = mx_type_val(*lhs)[6] / mx_type_val(*rhs)[6];
    mx_type_val(*dst)[7] = mx_type_val(*lhs)[7] / mx_type_val(*rhs)[7];
} // v8si_div

bool mexp_v8si_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8si_div, dst_idx, src1_idx, src2_idx);
} // mexp_v8si_div

static void v8si_load_i32(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    int32_t * dst = bond[ins->arg[0]].i32;
    int32_t * off = bond[ins->arg[1]].i32;
    v8si_t * src = bond[ins->arg[2]].v8si;

    *dst = mx_type_val(*src)[*off];
} // v8si_load_i32

bool mexp_v8si_load_i32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx)
{
    push_ins_with_3_args(me, &v8si_load_i32, dst_idx, off_idx, src_idx);
} // mexp_v8si_load_i32

static void v8si_store_i32(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8si_t * dst = bond[ins->arg[0]].v8si;
    int32_t * off = bond[ins->arg[1]].i32;
    int32_t * src = bond[ins->arg[2]].i32;

    mx_type_val(*dst)[*off] = *src;
} // v8si_store_i32

bool mexp_v8si_store_i32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx)
{
    push_ins_with_3_args(me, &v8si_store_i32, dst_idx, off_idx, src_idx);
} // mexp_v8si_store_i32

// ---- //

static void v8sf_add(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8sf_t * dst = bond[ins->arg[0]].v8sf;
    v8sf_t * lhs = bond[ins->arg[1]].v8sf;
    v8sf_t * rhs = bond[ins->arg[2]].v8sf;
    mx_type_reg(*dst) = _mm256_add_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_add

bool mexp_v8sf_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8sf_add, dst_idx, src1_idx, src2_idx);
} // mexp_v8sf_add

static void v8sf_sub(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8sf_t * dst = bond[ins->arg[0]].v8sf;
    v8sf_t * lhs = bond[ins->arg[1]].v8sf;
    v8sf_t * rhs = bond[ins->arg[2]].v8sf;
    mx_type_reg(*dst) = _mm256_sub_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_sub

bool mexp_v8sf_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8sf_sub, dst_idx, src1_idx, src2_idx);
} // mexp_v8sf_sub

static void v8sf_mul(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8sf_t * dst = bond[ins->arg[0]].v8sf;
    v8sf_t * lhs = bond[ins->arg[1]].v8sf;
    v8sf_t * rhs = bond[ins->arg[2]].v8sf;
    mx_type_reg(*dst) = _mm256_mul_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_mul

bool mexp_v8sf_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8sf_mul, dst_idx, src1_idx, src2_idx);
} // mexp_v8sf_mul

static void v8sf_div(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8sf_t * dst = bond[ins->arg[0]].v8sf;
    v8sf_t * lhs = bond[ins->arg[1]].v8sf;
    v8sf_t * rhs = bond[ins->arg[2]].v8sf;
    mx_type_reg(*dst) = _mm256_div_ps(mx_type_reg(*lhs), mx_type_reg(*rhs));
} // v8sf_div

bool mexp_v8sf_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8sf_div, dst_idx, src1_idx, src2_idx);
} // mexp_v8sf_div

static void v8sf_load_f32(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    float * dst = bond[ins->arg[0]].f32;
    int32_t * off = bond[ins->arg[1]].i32;
    v8sf_t * src = bond[ins->arg[2]].v8sf;

    *dst = mx_type_val(*src)[*off];
} // v8sf_load_f32

bool mexp_v8sf_load_f32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx)
{
    push_ins_with_3_args(me, &v8sf_load_f32, dst_idx, off_idx, src_idx);
} // mexp_v8sf_load_f32

static void v8sf_store_f32(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8sf_t * dst = bond[ins->arg[0]].v8sf;
    int32_t * off = bond[ins->arg[1]].i32;
    float * src = bond[ins->arg[2]].f32;

    mx_type_val(*dst)[*off] = *src;
} // v8sf_store_f32

bool mexp_v8sf_store_f32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx)
{
    push_ins_with_3_args(me, &v8sf_store_f32, dst_idx, off_idx, src_idx);
} // mexp_v8sf_store_f32

// ---- Matrix-related instruction-adding functions ---- //

static void v8si_mat_add(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8si_add(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, bond[ins->arg[2]].mat);
} // v8si_mat_add

bool mexp_v8si_mat_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8si_mat_add, dst_idx, src1_idx, src2_idx);
} // mexp_v8si_mat_add

static void v8si_mat_sub(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8si_subtract(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, bond[ins->arg[2]].mat);
} // v8si_mat_sub

bool mexp_v8si_mat_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8si_mat_sub, dst_idx, src1_idx, src2_idx);
} // mexp_v8si_mat_sub

static void v8si_mat_mul(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8si_multiply(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, bond[ins->arg[2]].mat);
} // v8si_mat_mul

bool mexp_v8si_mat_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8si_mat_mul, dst_idx, src1_idx, src2_idx);
} // mexp_v8si_mat_mul

static void v8si_mat_mul_by_scr(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8si_multiply_scalar(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, *(bond[ins->arg[2]].i32));
} // v8si_mat_mul_by_scr

bool mexp_v8si_mat_mul_by_scr(mx_expr_ptr me, uint32_t dst_idx, uint32_t mat_idx, uint32_t i32_idx)
{
    push_ins_with_3_args(me, &v8si_mat_mul_by_scr, dst_idx, mat_idx, i32_idx);
} // mexp_v8si_mat_mul_by_scr

static void v8si_mat_load_row_vec(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8si_t * dst = bond[ins->arg[0]].v8si;
    uint32_t val_ridx = (uint32_t) *(bond[ins->arg[1]].i32);
    uint32_t val_cidx = (uint32_t) *(bond[ins->arg[2]].i32);
    int32_t col_off = *(bond[ins->arg[3]].i32);
    int32_t def_val = *(bond[ins->arg[4]].i32);
    mx_stor_ptr src = bond[ins->arg[5]].mat;

    mstr_v8si_load_row_vector(src, val_ridx, val_cidx, col_off, def_val, dst);
} // v8si_mat_load_row_vec

bool mexp_v8si_mat_load_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t dval_idx, uint32_t src_idx)
{
    push_ins_with_6_args(me, &v8si_mat_load_row_vec, dst_idx, row_idx, col_idx, off_idx, dval_idx, src_idx);
} // mexp_v8si_mat_load_row_vec

static void v8si_mat_store_row_vec(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mx_stor_ptr dst = bond[ins->arg[0]].mat;
    uint32_t val_ridx = (uint32_t) *(bond[ins->arg[1]].i32);
    uint32_t val_cidx = (uint32_t) *(bond[ins->arg[2]].i32);
    int32_t col_off = *(bond[ins->arg[3]].i32);
    v8si_t * src = bond[ins->arg[4]].v8si;

    mstr_v8si_store_row_vector(dst, val_ridx, val_cidx, col_off, src);
} // v8si_mat_store_row_vec

bool mexp_v8si_mat_store_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t src_idx)
{
    push_ins_with_5_args(me, &v8si_mat_store_row_vec, dst_idx, row_idx, col_idx, off_idx, src_idx);
} // mexp_v8si_mat_store_row_vec

// ---- //

static void v8sf_mat_add(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8sf_add(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, bond[ins->arg[2]].mat);
} // v8sf_mat_add

bool mexp_v8sf_mat_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8sf_mat_add, dst_idx, src1_idx, src2_idx);
} // mexp_v8sf_mat_add

static void v8sf_mat_sub(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8sf_subtract(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, bond[ins->arg[2]].mat);
} // v8sf_mat_sub

bool mexp_v8sf_mat_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8sf_mat_sub, dst_idx, src1_idx, src2_idx);
} // mexp_v8sf_mat_sub

static void v8sf_mat_mul(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8sf_multiply(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, bond[ins->arg[2]].mat);
} // v8sf_mat_mul

bool mexp_v8sf_mat_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx)
{
    push_ins_with_3_args(me, &v8sf_mat_mul, dst_idx, src1_idx, src2_idx);
} // mexp_v8sf_mat_mul

static void v8sf_mat_mul_by_scr(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mops_v8sf_multiply_scalar(bond[ins->arg[0]].mat, bond[ins->arg[1]].mat, *(bond[ins->arg[2]].f32));
} // v8sf_mat_mul_by_scr

bool mexp_v8sf_mat_mul_by_scr(mx_expr_ptr me, uint32_t dst_idx, uint32_t mat_idx, uint32_t f32_idx)
{
    push_ins_with_3_args(me, &v8sf_mat_mul_by_scr, dst_idx, mat_idx, f32_idx);
} // mexp_v8sf_mat_mul_by_scr

static void v8sf_mat_load_row_vec(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    v8sf_t * dst = bond[ins->arg[0]].v8sf;
    uint32_t val_ridx = (uint32_t) *(bond[ins->arg[1]].i32);
    uint32_t val_cidx = (uint32_t) *(bond[ins->arg[2]].i32);
    int32_t col_off = *(bond[ins->arg[3]].i32);
    float def_val = *(bond[ins->arg[4]].f32);
    mx_stor_ptr src = bond[ins->arg[5]].mat;

    mstr_v8sf_load_row_vector(src, val_ridx, val_cidx, col_off, def_val, dst);
} // v8sf_mat_load_row_vec

bool mexp_v8sf_mat_load_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t dval_idx, uint32_t src_idx)
{
    push_ins_with_6_args(me, &v8sf_mat_load_row_vec, dst_idx, row_idx, col_idx, off_idx, dval_idx, src_idx);
} // mexp_v8sf_mat_load_row_vec

static void v8sf_mat_store_row_vec(mexp_ins_ptr ins, mexp_bond_ptr bond)
{
    mx_stor_ptr dst = bond[ins->arg[0]].mat;
    uint32_t val_ridx = (uint32_t) *(bond[ins->arg[1]].i32);
    uint32_t val_cidx = (uint32_t) *(bond[ins->arg[2]].i32);
    int32_t col_off = *(bond[ins->arg[3]].i32);
    v8sf_t * src = bond[ins->arg[4]].v8sf;

    mstr_v8sf_store_row_vector(dst, val_ridx, val_cidx, col_off, src);
} // v8sf_mat_store_row_vec

bool mexp_v8sf_mat_store_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t src_idx)
{
    push_ins_with_5_args(me, &v8sf_mat_store_row_vec, dst_idx, row_idx, col_idx, off_idx, src_idx);
} // mexp_v8sf_mat_store_row_vec

// ---- Evaluation functions ---- //

void mexp_evaluate(mx_expr_ptr me)
{
    uint32_t i = 0;
    mexp_ins_ptr ins = NULL;

    for (i = 1; i < me->ins_len; i += 1) {
        ins = &me->ins[i];
        (ins->op)(ins, me->bond);
    } // for
} // mexp_evaluate

void mexp_v8si_filter(mx_expr_ptr me, uint32_t row_idx, uint32_t col_idx, uint32_t src_idx)
{
    mx_stor_ptr src = me->bond[src_idx].mat;
    uint32_t * row = me->bond[row_idx].i32;
    uint32_t * col = me->bond[col_idx].i32;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t l = 0;
    uint32_t row_in_chk = 0;
    uint32_t col_in_chk = 0;
    void * base = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(src); j += 1) {
            base = mstr_v8si_locate_chunk(src, i, j, &row_in_chk, &col_in_chk);

            for (k = 0; k < row_in_chk; k += 1) {
                for (l = 0; l < col_in_chk; l += 8) {
                    *row = k;
                    *col = l;
                    mexp_evaluate(me);
                } // for
            } // for
        } // for
    } // for
} // mexp_filter
