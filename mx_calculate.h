#ifndef MX_CALCULATE_H
#define MX_CALCULATE_H 1

#include "mx_storage.h"

extern void mclt_v8si_add_chunk(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs);
extern void mclt_v8si_subtract_chunk(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs);
extern void mclt_v8si_multiply_chunk(mx_chunk_ptr chk, mx_chunk_ptr lhs, mx_chunk_ptr rhs);

extern void mclt_v8si_add(mx_stor_ptr ms, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mclt_v8si_subtract(mx_stor_ptr ms, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mclt_v8si_multiply(mx_stor_ptr ms, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mclt_v8si_scalar_multiply(mx_stor_ptr ms, int32_t lhs, mx_stor_ptr rhs);

#endif // MX_CALCULATE_H

