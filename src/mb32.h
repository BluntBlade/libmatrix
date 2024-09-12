#ifndef MB32_H
#define MB32_H 1

#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "src/mx_types.h"
#include "src/mx_common.h"

// ==== Declaration of MB32 storage ==== //

enum {
    MB32_CHK_LEN    = 8,
    MB32_ALIGNMENT  = 8 * 4,
};

typedef struct MB32_OFFSET {
    int32_t roff;
    int32_t coff;
} mb32_off_t, *mb32_off_ptr;

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

inline static int32_t mb32_chk_delta(int32_t idx)
{
    return idx & (MB32_CHK_LEN - 1);
} // mb32_chk_delta

inline static int32_t mb32_chk_current_boundary(int32_t idx)
{
    return idx & ~(MB32_CHK_LEN - 1);
} // mb32_chk_current_boundary

inline static int32_t mb32_chk_next_boundary(int32_t idx)
{
    return mb32_chk_current_boundary(idx) + MB32_CHK_LEN;
} // mb32_chk_next_boundary

inline static int32_t mb32_chk_inum(int32_t num, int32_t idx)
{
    return (num - idx) < MB32_CHK_LEN ? (num - idx) : MB32_CHK_LEN;
} // mb32_chk_inum

inline static int32_t mb32_chk_irmd(int32_t num, int32_t idx)
{
    return mb32_chk_inum(num, idx) - mb32_chk_delta(idx);
} // mb32_chk_irmd

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

// ==== Declaration of MB32 iterator ==== //

struct MB32_ITER;
typedef struct MB32_ITER * mb32_iter_ptr;

typedef struct MB32_ITER {
    v8si_t          cmask[3];

    mb32_stor_ptr   ms;
    int32_t         ridx;
    int32_t         cidx;
    int32_t         rbegin;
    int32_t         cbegin;
    int32_t         rend;
    int32_t         cend;
} mb32_iter_t __attribute__ ((aligned (32)));

extern void mb32_itr_init_for_iterating_in_range(mb32_iter_ptr it, mb32_stor_ptr ms, int32_t rbegin, int32_t cbegin, int32_t rend, int32_t cend);

inline static void mb32_itr_init_for_iterating(mb32_iter_ptr it, mb32_stor_ptr ms)
{
    return mb32_itr_init_for_iterating_in_range(it, ms, 0, 0, ms->rnum, ms->cnum);
} // mb32_itr_init_for_iterating

inline static void mb32_itr_reset(mb32_iter_ptr it)
{
    it->ridx = it->rbegin;
    it->cidx = it->cbegin;
} // mb32_itr_reset

inline static int32_t mb32_itr_ridx(mb32_iter_ptr it)
{
    return it->ridx;
} // mb32_itr_ridx

inline static int32_t mb32_itr_cidx(mb32_iter_ptr it)
{
    return it->cidx;
} // mb32_itr_cidx

inline static int32_t mb32_itr_vnum_in_row(mb32_iter_ptr it)
{
    return it->rend - it->rbegin;
} // mb32_itr_vnum_in_row

inline static int32_t mb32_itr_vnum_in_column(mb32_iter_ptr it)
{
    return it->cend - it->cbegin;
} // mb32_itr_vnum_in_column

inline static int32_t mb32_itr_vnum_in_chunk(mb32_iter_ptr it)
{
    return MB32_CHK_LEN * MB32_CHK_LEN;
} // mb32_itr_vnum_in_chunk

inline static int32_t mb32_itr_vecnum_in_row(mb32_iter_ptr it)
{
    return (mb32_itr_vnum_in_row(it) + MB32_CHK_LEN - 1) & ~(MB32_CHK_LEN - 1);
} // mb32_itr_vecnum_in_row

inline static int32_t mb32_itr_vecnum_in_column(mb32_iter_ptr it)
{
    return (mb32_itr_vnum_in_column(it) + MB32_CHK_LEN - 1) & ~(MB32_CHK_LEN - 1);
} // mb32_itr_vecnum_in_column

inline static int32_t mb32_itr_vecnum_in_chunk(mb32_iter_ptr it)
{
    return MB32_CHK_LEN;
} // mb32_itr_vecnum_in_chunk

inline static bool mb32_itr_to_next_row(mb32_iter_ptr it)
{
    if (it->ridx >= it->rend) {
        return false;
    } // if
    it->ridx += 1;
    it->cidx = it->cbegin;
    return it->ridx < it->rend;
} // mb32_itr_to_next_row

inline static bool mb32_itr_to_next_column(mb32_iter_ptr it)
{
    if (it->cidx >= it->cend) {
        return false;
    } // if
    it->cidx += 1;
    it->ridx = it->rbegin;
    return it->cidx < it->cend;
} // mb32_itr_to_next_column

inline static bool mb32_itr_to_next_chunk(mb32_iter_ptr it)
{
    if (it->ridx >= it->rend || it->cidx >= it->cend) {
        return false;
    } // if
    it->cidx += MB32_CHK_LEN;
    if (it->cidx >= it->cend) {
        it->cidx = it->cbegin;
        it->ridx += MB32_CHK_LEN;
    } // if
    return it->ridx < it->rend;
} // mb32_itr_to_next_chunk

inline static bool mb32_itr_to_next_position_in_row(mb32_iter_ptr it, int32_t step, int32_t boundary)
{
    it->cidx += step;
    it->ridx = (it->cidx < boundary) ? it->ridx : mx_i32_min(it->ridx + 1, it->rend);
    it->cidx = (it->cidx < boundary) ? it->cidx : it->cbegin;
    return it->ridx < it->rend;
} // mb32_itr_to_next_position_in_row

inline static bool mb32_itr_to_next_position_in_column(mb32_iter_ptr it, int32_t step, int32_t boundary)
{
    it->ridx += step;
    it->cidx = (it->ridx < boundary) ? it->cidx : mx_i32_min(it->cidx + 1, it->cend);
    it->ridx = (it->ridx < boundary) ? it->ridx : it->rbegin;
    return it->cidx < it->cend;
} // mb32_itr_to_next_position_in_column

inline static bool mb32_itr_to_next_value_in_row(mb32_iter_ptr it)
{
    return mb32_itr_to_next_position_in_row(it, 1, it->cend);
} // mb32_itr_to_next_value_in_row

inline static bool mb32_itr_to_next_value_in_column(mb32_iter_ptr it)
{
    return mb32_itr_to_next_position_in_column(it, 1, it->rend);
} // mb32_itr_to_next_value_in_column

inline static bool mb32_itr_to_next_value_in_chunk(mb32_iter_ptr it)
{
    return mb32_itr_to_next_position_in_row(it, 1, mb32_chk_next_boundary(it->cidx));
} // mb32_itr_to_next_value_in_chunk

inline static bool mb32_itr_to_next_v8si_in_row(mb32_iter_ptr it)
{
    return mb32_itr_to_next_position_in_row(it, I32S_IN_V8SI, it->cend);
} // mb32_itr_to_next_v8si_in_row

inline static bool mb32_itr_to_next_v8si_in_column(mb32_iter_ptr it)
{
    return mb32_itr_to_next_position_in_column(it, I32S_IN_V8SI, it->rend);
} // mb32_itr_to_next_v8si_in_column

inline static bool mb32_itr_to_next_v8si_in_chunk(mb32_iter_ptr it)
{
    return mb32_itr_to_next_position_in_row(it, I32S_IN_V8SI, mb32_chk_next_boundary(it->cidx));
} // mb32_itr_to_next_v8si_in_chunk

extern bool mb32_itr_get_v8si_in_row(mb32_iter_ptr it, v8si_t * vec, uint32_t vn, mb32_off_t * off, int32_t dval, bool move);
extern bool mb32_itr_get_v8si_in_column(mb32_iter_ptr it, v8si_t * vec, uint32_t vn, mb32_off_t * off, int32_t dval, bool move);
extern bool mb32_itr_set_v8si_in_row(mb32_iter_ptr it, v8si_t * vec, bool move);
extern bool mb32_itr_set_v8si_in_column(mb32_iter_ptr it, v8si_t * vec, bool move);

#endif // MB32_H

