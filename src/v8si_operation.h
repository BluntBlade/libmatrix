#ifndef MX_OPERATION_H
#define MX_OPERATION_H 1

#include "mx_storage.h"

extern void mops_v8si_add(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8si_subtract(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8si_multiply(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8si_multiply_scalar(mx_stor_ptr dst, mx_stor_ptr src, int32_t val);

extern void mops_v8si_transpose(mx_stor_ptr dst, mx_stor_ptr src);

#endif // MX_OPERATION_H

