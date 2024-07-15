#ifndef MX_EXPRESSION_H
#define MX_EXPRESSION_H 1

#include <stdbool.h>

#include "src/mx_types.h"
#include "src/mx_storage.h"

typedef struct MEXP_BOND
{
    uint32_t type;

    union {
        int32_t *       i32;
        float *         f32;
        v8si_t *        v8si;
        v8sf_t *        v8sf;
        mx_stor_ptr     mat;
    };
} mexp_bond_t, *mexp_bond_ptr;

struct MX_INSTRUCTION;
typedef struct MX_INSTRUCTION *mexp_ins_ptr;

typedef void (*ins_fn)(mexp_ins_ptr ins, mexp_bond_ptr bond);

typedef struct MX_INSTRUCTION
{
    uint32_t arg[8];
    ins_fn   op;
} mexp_ins_t;

typedef struct MX_EXPRESSION
{
    uint32_t ins_cap;
    uint32_t ins_len;
    uint32_t bond_cap;
    uint32_t bond_len;

    mexp_ins_ptr    ins;
    mexp_bond_ptr   bond;
} mx_expr_t, *mx_expr_ptr;

extern mx_expr_ptr mexp_init(mx_expr_ptr me, uint32_t ins_cap, uint32_t bond_cap);
extern void mexp_clean(mx_expr_ptr me);

extern mx_expr_ptr mexp_create(uint32_t ins_cap, uint32_t bond_cap);
extern void mexp_destroy(mx_expr_ptr me);

extern mx_expr_ptr mexp_duplicate(mx_expr_ptr me);

extern uint32_t mexp_i32_bind_scalar(mx_expr_ptr me, int32_t * val);
extern void mexp_i32_rebind_scalar(mx_expr_ptr me, uint32_t bidx, int32_t * val);

extern uint32_t mexp_f32_bind_scalar(mx_expr_ptr me, float * val);
extern void mexp_f32_rebind_scalar(mx_expr_ptr me, uint32_t bidx, float * val);

extern uint32_t mexp_v8si_bind_vector(mx_expr_ptr me, v8si_t * vec);
extern void mexp_v8si_rebind_vector(mx_expr_ptr me, uint32_t bidx, v8si_t * vec);

extern uint32_t mexp_v8si_bind_matrix(mx_expr_ptr me, mx_stor_ptr mx);
extern void mexp_v8si_rebind_matrix(mx_expr_ptr me, uint32_t bidx, mx_stor_ptr mx);

extern uint32_t mexp_v8sf_bind_vector(mx_expr_ptr me, v8sf_t * vec);
extern void mexp_v8sf_rebind_vector(mx_expr_ptr me, uint32_t bidx, v8sf_t * vec);

extern uint32_t mexp_v8sf_bind_matrix(mx_expr_ptr me, mx_stor_ptr mx);
extern void mexp_v8sf_rebind_matrix(mx_expr_ptr me, uint32_t bidx, mx_stor_ptr mx);

// ---- Scalar-related instruction-adding functions ---- //

extern bool mexp_i32_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_i32_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_i32_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_i32_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);

extern bool mexp_f32_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_f32_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_f32_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_f32_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);

// ---- Vector-related instruction-adding functions ---- //

extern bool mexp_v8si_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8si_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8si_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8si_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);

extern bool mexp_v8si_load_i32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx);
extern bool mexp_v8si_store_i32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx);

extern bool mexp_v8sf_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8sf_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8sf_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8sf_div(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);

extern bool mexp_v8sf_load_f32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx);
extern bool mexp_v8sf_store_f32(mx_expr_ptr me, uint32_t dst_idx, uint32_t off_idx, uint32_t src_idx);

// ---- Matrix-related instruction-adding functions ---- //

extern bool mexp_v8si_mat_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8si_mat_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8si_mat_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8si_mat_mul_by_scr(mx_expr_ptr me, uint32_t dst_idx, uint32_t mat_idx, uint32_t i32_idx);

extern bool mexp_v8si_mat_load_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t dval_idx, uint32_t src_idx);
extern bool mexp_v8si_mat_store_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t src_idx);

extern bool mexp_v8sf_mat_add(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8sf_mat_sub(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8sf_mat_mul(mx_expr_ptr me, uint32_t dst_idx, uint32_t src1_idx, uint32_t src2_idx);
extern bool mexp_v8sf_mat_mul_by_scr(mx_expr_ptr me, uint32_t dst_idx, uint32_t mat_idx, uint32_t f32_idx);

extern bool mexp_v8sf_mat_load_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t dval_idx, uint32_t src_idx);
extern bool mexp_v8sf_mat_store_row_vec(mx_expr_ptr me, uint32_t dst_idx, uint32_t row_idx, uint32_t col_idx, uint32_t off_idx, uint32_t src_idx);

// ---- Evaluation functions ---- //

extern void mexp_evaluate(mx_expr_ptr me);
extern void mexp_v8si_filter(mx_expr_ptr me, uint32_t row_idx, uint32_t col_idx, uint32_t src_idx);

#endif // MX_EXPRESSION_H

