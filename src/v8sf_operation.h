#ifndef V8SF_OPERATION_H
#define V8SF_OPERATION_H 1

#include "mx_storage.h"

extern void mops_v8sf_add(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8sf_subtract(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8sf_multiply(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mops_v8sf_multiply_scalar(mx_stor_ptr dst, mx_stor_ptr src, float val);

extern void mops_v8sf_transpose(mx_stor_ptr dst, mx_stor_ptr src);

#endif // V8SF_OPERATION_H

