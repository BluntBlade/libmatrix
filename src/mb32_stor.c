#include <stdlib.h>
#include <immintrin.h>

#include "src/mb32_stor.h"

// ---- Common functions ---- //

mb32_stor_ptr mb32_init(mb32_stor_ptr ms, int32_t rnum, int32_t cnum, uint16_t val_sz, uint16_t vec_len)
{
    ms->rnum = rnum;
    ms->cnum = cnum;
    ms->val_sz = val_sz;
    ms->vec_len = vec_len;

    ms->buf = malloc(mb32_padded_bytes(ms) + MB32_ALIGNMENT);
    if (! ms->buf) {
        return NULL;
    } // if

    ms->chks = (mb32_chk_ptr) ((size_t)(ms->buf + MB32_ALIGNMENT - 1) & ~(MB32_ALIGNMENT - 1));
    return ms;
} // mb32_init

// ---- Functions processing int32_t elements ---- //

inline static void i32_chk_zero(mb32_chk_ptr dchk)
{
    mx_type_reg(dchk->vec.i32[0]) = _mm256_setzero_si256();
    mx_type_reg(dchk->vec.i32[1]) = _mm256_setzero_si256();
    mx_type_reg(dchk->vec.i32[2]) = _mm256_setzero_si256();
    mx_type_reg(dchk->vec.i32[3]) = _mm256_setzero_si256();
    mx_type_reg(dchk->vec.i32[4]) = _mm256_setzero_si256();
    mx_type_reg(dchk->vec.i32[5]) = _mm256_setzero_si256();
    mx_type_reg(dchk->vec.i32[6]) = _mm256_setzero_si256();
    mx_type_reg(dchk->vec.i32[7]) = _mm256_setzero_si256();
} // i32_chk_zero

void mb32_i32_init_zeros(mb32_stor_ptr ms)
{
    mb32_chk_ptr dchk = ms->chks;
    int32_t i = mb32_chknum(ms) / 8;
    switch (mb32_chknum(ms) % 8) {
        case 8: do {
                    i32_chk_zero(dchk++);
        case 7:     i32_chk_zero(dchk++);
        case 6:     i32_chk_zero(dchk++);
        case 5:     i32_chk_zero(dchk++);
        case 4:     i32_chk_zero(dchk++);
        case 3:     i32_chk_zero(dchk++);
        case 2:     i32_chk_zero(dchk++);
        case 1:     i32_chk_zero(dchk++);
                } while (i-- > 0);
    } // switch
} // mb32_i32_init_zeros

void mb32_i32_init_identity(mb32_stor_ptr ms)
{
    assert(ms->rnum == ms->cnum);

    mb32_i32_init_zeros(ms);

    mb32_chk_ptr dchk = ms->chks + mb32_chknum(ms) - 1;
    int32_t dn = mb32_chknum_in_width(ms);
    int32_t i = dn - 1;
    switch (ms->rnum & (MB32_CHK_LEN - 1)) {
        // NOTE: This action *NEVER* touch padding elements.
        case 0: do {
                    dchk->arr.i32[7][7] = 1;
        case 7:     dchk->arr.i32[6][6] = 1;
        case 6:     dchk->arr.i32[5][5] = 1;
        case 5:     dchk->arr.i32[4][4] = 1;
        case 4:     dchk->arr.i32[3][3] = 1;
        case 3:     dchk->arr.i32[2][2] = 1;
        case 2:     dchk->arr.i32[1][1] = 1;
        case 1:     dchk->arr.i32[0][0] = 1;
                    dchk -= (dn + 1);
                } while (i-- > 0);
    } // switch
} // mb32_i32_init_identity

inline static void i32_chk_fill(mb32_chk_ptr dchk, int32_t rnum, int32_t cnum, int32_t val)
{
    v8si_t vals; 
    mx_type_reg(vals) = _mm256_and_si256(_mm256_set1_epi32(val), mx_type_reg(v8si_mask[cnum]));

    int32_t t = 0xFF >> (MB32_CHK_LEN - rnum);
    mx_type_reg(dchk->vec.i32[0]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 0) & 0x1)) + 1));
    mx_type_reg(dchk->vec.i32[1]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 1) & 0x1)) + 1));
    mx_type_reg(dchk->vec.i32[2]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 2) & 0x1)) + 1));
    mx_type_reg(dchk->vec.i32[3]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 3) & 0x1)) + 1));
    mx_type_reg(dchk->vec.i32[4]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 4) & 0x1)) + 1));
    mx_type_reg(dchk->vec.i32[5]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 5) & 0x1)) + 1));
    mx_type_reg(dchk->vec.i32[6]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 6) & 0x1)) + 1));
    mx_type_reg(dchk->vec.i32[7]) = _mm256_and_si256(mx_type_reg(vals), _mm256_set1_epi32((~((t >> 7) & 0x1)) + 1));
} // i32_chk_fill

inline static void i32_chk_assign(mb32_chk_ptr dchk, mb32_chk_ptr schk)
{
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[0]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[0])));
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[1]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[1])));
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[2]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[2])));
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[3]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[3])));
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[4]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[4])));
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[5]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[5])));
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[6]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[6])));
    _mm256_store_si256(&mx_type_reg(dchk->vec.i32[7]), _mm256_load_si256(&mx_type_reg(schk->vec.i32[7])));
} // i32_chk_assign

static void i32_chk_copy(mb32_chk_ptr dchk, int32_t dn, int32_t off, mb32_chk_ptr schk)
{
    v8si_t t[8];
    mx_type_reg(t[0]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[0]));
    mx_type_reg(t[1]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[1]));
    mx_type_reg(t[2]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[2]));
    mx_type_reg(t[3]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[3]));
    mx_type_reg(t[4]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[4]));
    mx_type_reg(t[5]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[5]));
    mx_type_reg(t[6]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[6]));
    mx_type_reg(t[7]) = _mm256_load_si256(&mx_type_reg(schk->vec.i32[7]));

    for (int32_t i = 0; i < dn; i += 1, dchk += off) {
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[0]), mx_type_reg(t[0]));
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[1]), mx_type_reg(t[1]));
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[2]), mx_type_reg(t[2]));
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[3]), mx_type_reg(t[3]));
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[4]), mx_type_reg(t[4]));
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[5]), mx_type_reg(t[5]));
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[6]), mx_type_reg(t[6]));
        _mm256_store_si256(&mx_type_reg(dchk[i].vec.i32[7]), mx_type_reg(t[7]));
    } // for
} // i32_chk_copy

void mb32_i32_fill(mb32_stor_ptr ms, int32_t val)
{
    int32_t chk_rnum = mb32_chknum_in_height(ms);
    int32_t chk_cnum = mb32_chknum_in_width(ms);
    bool end = chk_rnum == 1;

    int32_t rnum = ms->rnum < MB32_CHK_LEN ? ms->rnum : MB32_CHK_LEN; // The number of rows in the top-left chunk.
    int32_t cnum_l = ms->cnum < MB32_CHK_LEN ? ms->cnum : MB32_CHK_LEN; // The number of columns in the top-left chunk.
    int32_t cnum_r = ms->cnum - (ms->cnum & ~(MB32_CHK_LEN - 1));

    mb32_chk_ptr dchk = ms->chks;

FILL_A_LINE_OF_CHUNKS:
    // Fill the first chunk.
    i32_chk_fill(dchk, rnum, cnum_l, val);

    if (chk_cnum > 1) {
        if (chk_cnum > 2) {
            i32_chk_copy(dchk + 1, chk_cnum - 2, 0, dchk);
        } // if
        i32_chk_fill(dchk + chk_cnum - 1, rnum, cnum_r, val);
    } // if

    if (end) { return; } // if

    // Copy the [2, m - 1] lines.
    if (chk_rnum > 2) {
        mb32_chk_ptr ichk = ms->chks; // Used to fill inner chunks.
        mb32_chk_ptr rchk = ms->chks + chk_cnum - 1; // Used to fill right-edge chunks.

        for (int32_t i = 1; i < chk_rnum - 1; i += 1) {
            i32_chk_copy(dchk + i * chk_cnum, chk_cnum - 1, 0, ichk);
        } // for
        i32_chk_copy(dchk + 1 * chk_cnum + chk_cnum - 1, chk_rnum - 2, chk_cnum, rchk);
    } // if

    // Fill the bottom line.
    dchk += (chk_rnum - 1) * chk_cnum;
    rnum = ms->rnum - (ms->rnum & ~(MB32_CHK_LEN - 1)); // The number of rows in the last line.
    end = true;
    goto FILL_A_LINE_OF_CHUNKS;
} // mb32_i32_fill

inline static void i32_chk_transpose(mb32_chk_ptr dchk, mb32_chk_ptr schk)
{
    static v8si_t idx = { .val = {0, 8, 16, 24, 32, 40, 48, 56} };
    mx_type_reg(dchk->vec.i32[0]) = _mm256_i32gather_epi32(&schk->arr.i32[0][0], mx_type_reg(idx), I32_SIZE);
    mx_type_reg(dchk->vec.i32[1]) = _mm256_i32gather_epi32(&schk->arr.i32[0][1], mx_type_reg(idx), I32_SIZE);
    mx_type_reg(dchk->vec.i32[2]) = _mm256_i32gather_epi32(&schk->arr.i32[0][2], mx_type_reg(idx), I32_SIZE);
    mx_type_reg(dchk->vec.i32[3]) = _mm256_i32gather_epi32(&schk->arr.i32[0][3], mx_type_reg(idx), I32_SIZE);
    mx_type_reg(dchk->vec.i32[4]) = _mm256_i32gather_epi32(&schk->arr.i32[0][4], mx_type_reg(idx), I32_SIZE);
    mx_type_reg(dchk->vec.i32[5]) = _mm256_i32gather_epi32(&schk->arr.i32[0][5], mx_type_reg(idx), I32_SIZE);
    mx_type_reg(dchk->vec.i32[6]) = _mm256_i32gather_epi32(&schk->arr.i32[0][6], mx_type_reg(idx), I32_SIZE);
    mx_type_reg(dchk->vec.i32[7]) = _mm256_i32gather_epi32(&schk->arr.i32[0][7], mx_type_reg(idx), I32_SIZE);
} // i32_chk_transpose

void mb32_i32_transpose(mb32_stor_ptr ms, mb32_stor_ptr src)
{
    assert(mb32_chknum_in_height(ms) == mb32_chknum_in_height(src));
    assert(mb32_chknum_in_width(ms) == mb32_chknum_in_width(src));

    for (int32_t i = 0; i < mb32_chknum_in_height(src); i += 1) {
        for (int32_t j = 0; j < mb32_chknum_in_width(src); j += 1) {
            i32_chk_transpose(mb32_chk_locate(ms, j, i), mb32_chk_locate(src, i, j));
        } // for
    } // for

    ms->rnum = src->cnum;
    ms->cnum = src->rnum;
} // mb32_stor_ptr

extern void mb32_i32_add(mb32_stor_ptr ms, mb32_stor_ptr lhs, mb32_stor_ptr rhs);
extern void mb32_i32_sub(mb32_stor_ptr ms, mb32_stor_ptr lhs, mb32_stor_ptr rhs);
extern void mb32_i32_mul(mb32_stor_ptr ms, mb32_stor_ptr lhs, mb32_stor_ptr rhs);
extern void mb32_i32_mul_scalar(mb32_stor_ptr ms, mb32_stor_ptr lhs, int32_t rhs);
