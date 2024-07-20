#ifndef MX_V8SF_H
#define MX_V8SF_H 1

#include "mx_types.h"
#include "mx_common.h"
#include "mx_storage.h"

extern v8sf_t v8sf_zero;
extern v8sf_t v8sf_mask[9];

extern void mstr_v8sf_init_identity(mx_stor_ptr ms);

extern void mstr_v8sf_load_row_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, int32_t col_off, float def_val, v8sf_t * dst);
extern void mstr_v8sf_store_row_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, int32_t col_off, v8sf_t * src);
// TODO: extern void mstr_v8sf_load_column_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, int32_t def_val, v8sf_t * dst);
// TODO: extern void mstr_v8sf_store_column_vector(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t row_off, v8sf_t * src);

extern void mstr_v8sf_fill(mx_stor_ptr ms, float src);

extern void mstr_v8sf_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr dchk, uint32_t * dchk_rows, uint32_t * dchk_cols);

extern void mstr_v8sf_add(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mstr_v8sf_subtract(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mstr_v8sf_multiply(mx_stor_ptr dst, mx_stor_ptr lhs, mx_stor_ptr rhs);
extern void mstr_v8sf_multiply_scalar(mx_stor_ptr dst, mx_stor_ptr src, float val);

extern void mstr_v8sf_transpose(mx_stor_ptr dst, mx_stor_ptr src);

inline static void mstr_v8sf_init(mx_stor_ptr ms, uint32_t rows, uint32_t cols)
{
    mstr_init(ms, rows, cols, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
} // mstr_v8sf_init

inline static mx_stor_ptr mstr_v8sf_create(uint32_t rows, uint32_t cols)
{
    return mstr_create(rows, cols, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
} // mstr_v8sf_create

inline static void mstr_v8sf_destroy(mx_stor_ptr ms)
{
    return mstr_destroy(ms);
} // mstr_v8sf_destroy

inline static void mstr_v8sf_init_zeros(mx_stor_ptr ms)
{
    mstr_init_zeros(ms);
} // mstr_v8sf_init_zero

inline static void mstr_v8sf_init_ones(mx_stor_ptr ms)
{
    return mstr_v8sf_fill(ms, 1);
} // mstr_v8sf_init_ones

#endif // MX_V8SF_H

