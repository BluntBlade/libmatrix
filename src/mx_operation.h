#ifndef MX_OPERATION_H
#define MX_OPERATION_H 1

#include "mx_storage.h"

struct MX_OPERATION;
typedef struct MX_OPERATION * mx_oper_ptr;

extern mx_oper_ptr mops_v8si_create(void);
extern void mops_v8si_destroy(mx_oper_ptr mp);

extern void mops_v8si_add(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr ret);
extern void mops_v8si_subtract(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr ret);
extern void mops_v8si_multiply(mx_oper_ptr mp, mx_stor_ptr lhs, mx_stor_ptr rhs, mx_stor_ptr ret);
extern void mops_v8si_scalar_multiply(mx_oper_ptr mp, int32_t lhs, mx_stor_ptr rhs, mx_stor_ptr ret);

#endif // MX_OPERATION_H

