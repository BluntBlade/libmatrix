#ifndef MB32_H
#define MB32_H 1

#include <assert.h>
#include <string.h>

#include "src/mx_types.h"
#include "src/mx_common.h"

enum {
    MB32_CHK_LEN    = 8,
    MB32_ALIGNMENT  = 8 * 4,
};

typedef union MB32_CHUNK {
    union {
        int32_t i32[MB32_CHK_LEN][MB32_CHK_LEN];
        float   f32[MB32_CHK_LEN][MB32_CHK_LEN];
    } arr;

    union {
        v8si_t i32[MB32_CHK_LEN];
        v8sf_t f32[MB32_CHK_LEN];
    } vec;
} mb32_chk_t, *mb32_chk_ptr;

typedef struct MB32_STOR {
    int16_t     val_sz;             // The size of one value, in bytes.
    int16_t     vec_len;            // The number of values in one vector.
    int32_t     rnum;               // The number of rows.
    int32_t     cnum;               // The number of columns.

    void *          buf;           // Non-aligned address of allocated memory, only for calling free().
    mb32_chk_ptr    chks;          // Aligned address of allocated memory.
} mb32_stor_t, *mb32_stor_ptr;

extern mb32_stor_ptr mb32_init(mb32_stor_ptr ms, int32_t rnum, int32_t cnum, uint16_t val_sz, uint16_t vec_len);

inline static void mb32_clean(mb32_stor_ptr ms)
{
    free(ms->buf);
} // mb32_clean

inline static int32_t mb32_rnum(mb32_stor_ptr ms)
{
    return ms->rnum;
} // mb32_rnum

inline static int32_t mb32_cnum(mb32_stor_ptr ms)
{
    return ms->cnum;
} // mb32_cnum

inline static int32_t mb32_padded_rnum(mb32_stor_ptr ms)
{
    return mx_ceil_to_multiples_of(ms->rnum, ms->vec_len);
} // mb32_padded_rnum

inline static int32_t mb32_padded_cnum(mb32_stor_ptr ms)
{
    return mx_ceil_to_multiples_of(ms->cnum, ms->vec_len);
} // mb32_padded_cnum

inline static int32_t mb32_chknum_in_height(mb32_stor_ptr ms)
{
    return mb32_padded_rnum(ms) / MB32_CHK_LEN;
} // mb32_chknum_in_height

inline static int32_t mb32_chknum_in_width(mb32_stor_ptr ms)
{
    return mb32_padded_cnum(ms) / MB32_CHK_LEN;
} // mb32_chknum_in_width

inline static int32_t mb32_chknum(mb32_stor_ptr ms)
{
    return mb32_chknum_in_height(ms) * mb32_chknum_in_width(ms);
} // mb32_chknum

inline static size_t mb32_padded_bytes(mb32_stor_ptr ms)
{
    return ms->val_sz * mb32_padded_rnum(ms) * mb32_padded_cnum(ms);
} // mb32_padded_bytes

inline static mb32_chk_ptr mb32_chk_locate_by_number(mb32_stor_ptr ms, int32_t chk_ridx, int32_t chk_cidx)
{
    return ms->chks + chk_ridx * mb32_chknum_in_width(ms) + chk_cidx;
} // mb32_chk_locate_by_number

inline static mb32_chk_ptr mb32_chk_locate_by_index(mb32_stor_ptr ms, int32_t ridx, int32_t cidx)
{
    return mb32_chk_locate_by_number(ms, (ridx / MB32_CHK_LEN), (cidx / MB32_CHK_LEN));
} // mb32_chk_locate_by_index

inline static int32_t mb32_chk_delta(int32_t idx)
{
    return idx & (MB32_CHK_LEN - 1);
} // mb32_chk_delta

// ---- Functions processing int32_t elements ---- //

extern void mb32_i32_init_zeros(mb32_stor_ptr ms);
extern void mb32_i32_init_identity(mb32_stor_ptr ms);

extern void mb32_i32_fill(mb32_stor_ptr ms, int32_t val);
extern void mb32_i32_transpose(mb32_stor_ptr ms, mb32_stor_ptr src);

extern void mb32_i32_add(mb32_stor_ptr ms, mb32_stor_ptr lhs, mb32_stor_ptr rhs);
extern void mb32_i32_sub(mb32_stor_ptr ms, mb32_stor_ptr lhs, mb32_stor_ptr rhs);
extern void mb32_i32_mul(mb32_stor_ptr ms, mb32_stor_ptr lhs, mb32_stor_ptr rhs);
extern void mb32_i32_mul_scalar(mb32_stor_ptr ms, mb32_stor_ptr src, int32_t val);

inline static int32_t mb32_i32_get(mb32_stor_ptr ms, int32_t ridx, int32_t cidx)
{
    mb32_chk_ptr schk = mb32_chk_locate_by_index(ms, ridx, cidx);
    return schk->arr.i32[mb32_chk_delta(ridx)][mb32_chk_delta(cidx)];
} // mb32_i32_get

inline static void mb32_i32_set(mb32_stor_ptr ms, int32_t ridx, int32_t cidx, int32_t val)
{
    mb32_chk_ptr schk = mb32_chk_locate_by_index(ms, ridx, cidx);
    schk->arr.i32[mb32_chk_delta(ridx)][mb32_chk_delta(cidx)] = val;
} // mb32_i32_set

inline static void mb32_i32_init_ones(mb32_stor_ptr ms)
{
    mb32_i32_fill(ms, 1);
} // mb32_i32_init_ones

// ---- Functions processing float elements ---- //

#endif // MB32_H

