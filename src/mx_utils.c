#include <assert.h>
#include <math.h>
#include <immintrin.h>

#include "src/mx_utils.h"

void mstr_v8si_bisearch(v8si_t * dst_idx, int32_t * rng, int32_t n, v8si_t * src)
{
#define v8si_bisearch() \
    { \
        mx_type_reg(mid) = _mm256_sub_epi32(mx_type_reg(high), mx_type_reg(low)); \
        mx_type_reg(mid) = _mm256_srli_epi32(mx_type_reg(mid), 1); \
        mx_type_reg(mid) = _mm256_add_epi32(mx_type_reg(mid), mx_type_reg(low)); \
        mx_type_reg(vals) = _mm256_i32gather_epi32(rng, mx_type_reg(mid), I32_SIZE); \
        mx_type_reg(mask) = _mm256_cmpgt_epi32(mx_type_reg(vals), mx_type_reg(*src)); \
        mx_type_reg(high) =  _mm256_or_si256(_mm256_andnot_si256(mx_type_reg(mask), mx_type_reg(high)), _mm256_and_si256(mx_type_reg(mask), mx_type_reg(mid))); \
        mx_type_reg(low) =  _mm256_or_si256(_mm256_and_si256(mx_type_reg(mask), mx_type_reg(low)), _mm256_andnot_si256(mx_type_reg(mask), mx_type_reg(mid))); \
    }

    v8si_t low;
    v8si_t high;
    v8si_t mid;
    v8si_t vals;
    v8si_t mask;
    uint32_t m = 0;
    uint32_t i = 0;

    m = (uint32_t)( ceilf( log2f( (float)n ) ) );
    i = m / 8;

    mx_type_reg(low) = _mm256_setzero_si256();
    mx_type_reg(high) = _mm256_set1_epi32(n);
    switch (m % 8) {
        case 0: while (i-- > 0) {
                    v8si_bisearch();
        case 7:     v8si_bisearch();
        case 6:     v8si_bisearch();
        case 5:     v8si_bisearch();
        case 4:     v8si_bisearch();
        case 3:     v8si_bisearch();
        case 2:     v8si_bisearch();
        case 1:     v8si_bisearch();
                } // while
    } // switch

    *dst_idx = high;

#undef v8si_bisearch
} // mstr_v8si_bisearch

void mstr_v8sf_bisearch(v8si_t * dst_idx, float * rng, int32_t n, v8sf_t * src)
{
#define v8sf_bisearch() \
    { \
        mx_type_reg(mid) = _mm256_sub_epi32(mx_type_reg(high), mx_type_reg(low)); \
        mx_type_reg(mid) = _mm256_srli_epi32(mx_type_reg(mid), 1); \
        mx_type_reg(mid) = _mm256_add_epi32(mx_type_reg(mid), mx_type_reg(low)); \
        mx_type_reg(vals) = _mm256_i32gather_ps(rng, mx_type_reg(mid), F32_SIZE); \
        mx_type_reg(mask.f) = _mm256_cmp_ps(mx_type_reg(vals), mx_type_reg(*src), _CMP_GT_OQ); \
        mx_type_reg(high) =  _mm256_or_si256(_mm256_andnot_si256(mx_type_reg(mask.i), mx_type_reg(high)), _mm256_and_si256(mx_type_reg(mask.i), mx_type_reg(mid))); \
        mx_type_reg(low) =  _mm256_or_si256(_mm256_and_si256(mx_type_reg(mask.i), mx_type_reg(low)), _mm256_andnot_si256(mx_type_reg(mask.i), mx_type_reg(mid))); \
    }

    v8si_t low;
    v8si_t high;
    v8si_t mid;
    union {
        v8sf_t f;
        v8si_t i;
    } mask;
    v8sf_t vals;
    uint32_t m = 0;
    uint32_t i = 0;

    m = (uint32_t)( ceilf( log2f( (float)n ) ) );
    i = m / 8;

    mx_type_reg(low) = _mm256_setzero_si256();
    mx_type_reg(high) = _mm256_set1_epi32(n);
    switch (m % 8) {
        case 0: while (i-- > 0) {
                    v8sf_bisearch();
        case 7:     v8sf_bisearch();
        case 6:     v8sf_bisearch();
        case 5:     v8sf_bisearch();
        case 4:     v8sf_bisearch();
        case 3:     v8sf_bisearch();
        case 2:     v8sf_bisearch();
        case 1:     v8sf_bisearch();
                } // while
    } // switch

    *dst_idx = high;

#undef v8sf_bisearch
} // mstr_v8sf_bisearch
