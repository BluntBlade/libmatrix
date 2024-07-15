#ifndef MX_EXPRESSION_H
#define MX_EXPRESSION_H 1

#include <stdbool.h>

#include "src/mx_types.h"
#include "src/mx_storage.h"

struct MX_INSTRUCTION;
typedef struct MX_INSTRUCTION *mexp_ins_ptr;

typedef void (*ins_fn)(mexp_ins_ptr ins);

typedef struct MX_INSTRUCTION
{
    void*    arg[8];
    ins_fn   op;
} mexp_ins_t;

typedef struct MX_EXPRESSION
{
    uint32_t ins_cap;
    uint32_t ins_len;

    mexp_ins_ptr    ins;
} mx_expr_t, *mx_expr_ptr;

extern mx_expr_ptr mexp_init(mx_expr_ptr me, uint32_t ins_cap);
extern void mexp_clean(mx_expr_ptr me);

extern mx_expr_ptr mexp_create(uint32_t ins_cap);
extern void mexp_destroy(mx_expr_ptr me);

extern mx_expr_ptr mexp_duplicate(mx_expr_ptr me);

// ---- Scalar-related instruction-adding functions ---- //

extern bool mexp_i32_add(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_i32_sub(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_i32_mul(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_i32_div(mx_expr_ptr me, void * dst, void * src1, void * src2);

extern bool mexp_f32_add(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_f32_sub(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_f32_mul(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_f32_div(mx_expr_ptr me, void * dst, void * src1, void * src2);

// ---- Vector-related instruction-adding functions ---- //

extern bool mexp_v8si_add(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8si_sub(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8si_mul(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8si_div(mx_expr_ptr me, void * dst, void * src1, void * src2);

extern bool mexp_v8si_load_i32(mx_expr_ptr me, void * dst, void * off, void * src);
extern bool mexp_v8si_store_i32(mx_expr_ptr me, void * dst, void * off, void * src);

extern bool mexp_v8sf_add(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8sf_sub(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8sf_mul(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8sf_div(mx_expr_ptr me, void * dst, void * src1, void * src2);

extern bool mexp_v8sf_load_f32(mx_expr_ptr me, void * dst, void * off, void * src);
extern bool mexp_v8sf_store_f32(mx_expr_ptr me, void * dst, void * off, void * src);

// ---- Matrix-related instruction-adding functions ---- //

extern bool mexp_v8si_mat_add(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8si_mat_sub(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8si_mat_mul(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8si_mat_mul_by_scr(mx_expr_ptr me, void * dst, void * mat, void * i32);

extern bool mexp_v8si_mat_load_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * off, void * dval, void * src);
extern bool mexp_v8si_mat_store_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * off, void * src);

extern bool mexp_v8sf_mat_add(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8sf_mat_sub(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8sf_mat_mul(mx_expr_ptr me, void * dst, void * src1, void * src2);
extern bool mexp_v8sf_mat_mul_by_scr(mx_expr_ptr me, void * dst, void * mat, void * f32);

extern bool mexp_v8sf_mat_load_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * off, void * dval, void * src);
extern bool mexp_v8sf_mat_store_row_vec(mx_expr_ptr me, void * dst, void * row, void * col, void * off, void * src);

// ---- Evaluation functions ---- //

extern void mexp_evaluate(mx_expr_ptr me);
extern void mexp_v8si_filter(mx_expr_ptr me, void * row, void * col, void * src);

#endif // MX_EXPRESSION_H

