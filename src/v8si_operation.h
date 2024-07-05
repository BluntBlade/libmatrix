#ifndef MX_OPERATION_H
#define MX_OPERATION_H 1

#include "mx_storage.h"

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
} mx_oper_t, *mx_oper_ptr;

extern mx_oper_ptr mops_v8si_create(void);
extern void mops_v8si_destroy(mx_oper_ptr mp);

extern void mops_v8si_add(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8si_subtract(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8si_multiply(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr dst);
extern void mops_v8si_multiply_scalar(mx_stor_ptr dst, mx_stor_ptr src, int32_t val);

extern void mops_v8si_transpose(mx_stor_ptr dst, mx_stor_ptr src);

#endif // MX_OPERATION_H

