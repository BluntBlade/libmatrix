#include <assert.h>
#include <math.h>
#include <immintrin.h>

#include "src/mx_common.h"
#include "src/mx_utils.h"

#define v8si_bisearch(high, low, mid, mask, rng, src) \
    { \
        mx_type_reg(mid) = _mm256_sub_epi32(mx_type_reg(high), mx_type_reg(low)); \
        mx_type_reg(mid) = _mm256_srli_epi32(mx_type_reg(mid), 1); \
        mx_type_reg(mid) = _mm256_add_epi32(mx_type_reg(mid), mx_type_reg(low)); \
        mx_type_reg(vals) = _mm256_i32gather_epi32(rng, mx_type_reg(mid), I32_SIZE); \
        mx_type_reg(mask) = _mm256_cmpgt_epi32(mx_type_reg(vals), mx_type_reg(src)); \
        mx_type_reg(high) = _mm256_blendv_epi8(mx_type_reg(high), mx_type_reg(mid), mx_type_reg(mask)); \
        mx_type_reg(low) = _mm256_blendv_epi8(mx_type_reg(mid), mx_type_reg(low), mx_type_reg(mask)); \
    }

void mx_v8si_bisearch(v8si_t * dst_idx, int32_t * rng, int32_t n, v8si_t * src)
{
    v8si_t low;
    v8si_t high;
    v8si_t mid;
    v8si_t vals;
    v8si_t mask;
    uint32_t m = 0;
    uint32_t i = 0;

    //m = (uint32_t)( ceilf( log2f( (float)n ) ) );
    m = 32 - _lzcnt_u32(n); // Depends on the LZCNT flag of CPU.
    i = m / 8;

    mx_type_reg(low) = _mm256_setzero_si256();
    mx_type_reg(high) = _mm256_set1_epi32(n);
    switch (m % 8) {
        case 0: do {
                    v8si_bisearch(high, low, mid, mask, rng, *src);
        case 7:     v8si_bisearch(high, low, mid, mask, rng, *src);
        case 6:     v8si_bisearch(high, low, mid, mask, rng, *src);
        case 5:     v8si_bisearch(high, low, mid, mask, rng, *src);
        case 4:     v8si_bisearch(high, low, mid, mask, rng, *src);
        case 3:     v8si_bisearch(high, low, mid, mask, rng, *src);
        case 2:     v8si_bisearch(high, low, mid, mask, rng, *src);
        case 1:     v8si_bisearch(high, low, mid, mask, rng, *src);
                } while (i-- > 0);
    } // switch

    *dst_idx = high;
} // mx_v8si_bisearch

void mx_v8sf_bisearch(v8si_t * dst_idx, float * rng, int32_t n, v8sf_t * src)
{
#define v8sf_bisearch() \
    { \
        mx_type_reg(mid) = _mm256_sub_epi32(mx_type_reg(high), mx_type_reg(low)); \
        mx_type_reg(mid) = _mm256_srli_epi32(mx_type_reg(mid), 1); \
        mx_type_reg(mid) = _mm256_add_epi32(mx_type_reg(mid), mx_type_reg(low)); \
        mx_type_reg(vals) = _mm256_i32gather_ps(rng, mx_type_reg(mid), F32_SIZE); \
        mx_type_reg(mask.f) = _mm256_cmp_ps(mx_type_reg(vals), mx_type_reg(*src), _CMP_GT_OQ); \
        mx_type_reg(high) = _mm256_blendv_epi8(mx_type_reg(high), mx_type_reg(mid), mx_type_reg(mask.i)); \
        mx_type_reg(low) = _mm256_blendv_epi8(mx_type_reg(mid), mx_type_reg(low), mx_type_reg(mask.i)); \
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

    //m = (uint32_t)( ceilf( log2f( (float)n ) ) );
    m = 32 - _lzcnt_u32(n); // Depends on the LZCNT flag of CPU.
    i = m / 8;

    mx_type_reg(low) = _mm256_setzero_si256();
    mx_type_reg(high) = _mm256_set1_epi32(n);
    switch (m % 8) {
        case 0: do {
                    v8sf_bisearch();
        case 7:     v8sf_bisearch();
        case 6:     v8sf_bisearch();
        case 5:     v8sf_bisearch();
        case 4:     v8sf_bisearch();
        case 3:     v8sf_bisearch();
        case 2:     v8sf_bisearch();
        case 1:     v8sf_bisearch();
                } while (i-- > 0);
    } // switch

    *dst_idx = high;

#undef v8sf_bisearch
} // mx_v8sf_bisearch

void mx_v8si_interpolate(int32_t * y_out, uint32_t pn, int32_t * xp, int32_t * fp, float * slp, int32_t * left, int32_t * right, uint32_t xn, int32_t * x_in)
{
    v8si_t x[3];
    v8si_t y[3];
    v8si_t ydiff;
    v8si_t xdiff;
    v8si_t diff;
    v8si_t vmask_left;
    v8si_t vmask_right;
    union {
        v8si_t i;
        v8sf_t f;
    } vmask;
    v8sf_t slope;
    v8sf_t tmp;
    uint32_t i = 0;
    uint32_t k = 0;
    int32_t * src = 0;
    int32_t * dst = 0;
    v8si_t * xmask = NULL;

    v8si_t low;
    v8si_t high;
    v8si_t mid;
    v8si_t vals;
    v8si_t mask;
    uint32_t m = 0;
    uint32_t j = 0;

    assert(pn >= 2);
    assert(xn >= 1);

    if (! left) {
        left = &fp[0];
    } // if
    if (! right) {
        right = &fp[pn - 1];
    } // if

    src = x_in;
    dst = y_out;
    
    //m = (uint32_t)( ceilf( log2f( (float)pn ) ) );
    m = 32 - _lzcnt_u32(pn); // Depends on the LZCNT flag of CPU.

    i = mx_ceil_to_multiples(xn, I32S_IN_V8SI) / I32S_IN_V8SI;
    k = I32S_IN_V8SI - (mx_ceil_to_multiples(xn, I32S_IN_V8SI) - xn);
    xmask = &v8si_mask[k];

    // -------------------------------------------------
    // Linear interpolation equation:
    //   y = y0 + (x - x0) * ((y1 - y0) / (x1 - x0))
    // -------------------------------------------------
    while (i-- > 0) {        
        mx_type_reg(x[2]) = _mm256_maskload_epi32(src, mx_type_reg(*xmask));

        mx_type_reg(vmask_left) = _mm256_cmpgt_epi32(_mm256_set1_epi32(xp[0]), mx_type_reg(x[2]));
        mx_type_reg(vmask_right) = _mm256_cmpgt_epi32(mx_type_reg(x[2]), _mm256_set1_epi32(xp[pn - 1]));
        mx_type_reg(vmask.i) = _mm256_andnot_si256(_mm256_or_si256(mx_type_reg(vmask_left), mx_type_reg(vmask_right)), mx_type_reg(*xmask));

        mx_type_reg(low) = _mm256_setzero_si256();
        mx_type_reg(high) = _mm256_set1_epi32(pn);

        j = m / 8;
        switch (m % 8) {
            case 0: do {
                        v8si_bisearch(high, low, mid, mask, xp, x[2]);
            case 7:     v8si_bisearch(high, low, mid, mask, xp, x[2]);
            case 6:     v8si_bisearch(high, low, mid, mask, xp, x[2]);
            case 5:     v8si_bisearch(high, low, mid, mask, xp, x[2]);
            case 4:     v8si_bisearch(high, low, mid, mask, xp, x[2]);
            case 3:     v8si_bisearch(high, low, mid, mask, xp, x[2]);
            case 2:     v8si_bisearch(high, low, mid, mask, xp, x[2]);
            case 1:     v8si_bisearch(high, low, mid, mask, xp, x[2]);
                    } while (j-- > 0);
        } // switch

        mx_type_reg(x[0]) = _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), xp, mx_type_reg(low), mx_type_reg(vmask.i), I32_SIZE);
        mx_type_reg(y[0]) = _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), fp, mx_type_reg(low), mx_type_reg(vmask.i), I32_SIZE);

        if (! slp) {
            mx_type_reg(y[1]) = _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), fp, mx_type_reg(high), mx_type_reg(vmask.i), I32_SIZE);

            // NOTE: For (x1 - x0) is used as divisor, it MUSTN'T be zero. So the default values of x1 and x0 are set to 1 and 0, respectively.
            mx_type_reg(x[1]) = _mm256_mask_i32gather_epi32(_mm256_set1_epi32(1), xp, mx_type_reg(high), mx_type_reg(vmask.i), I32_SIZE);

            mx_type_reg(ydiff) = _mm256_sub_epi32(mx_type_reg(y[1]), mx_type_reg(y[0]));
            mx_type_reg(xdiff) = _mm256_sub_epi32(mx_type_reg(x[1]), mx_type_reg(x[0]));

            mx_type_reg(slope) = _mm256_div_ps(_mm256_cvtepi32_ps(mx_type_reg(ydiff)), _mm256_cvtepi32_ps(mx_type_reg(xdiff)));
        } else {
            mx_type_reg(slope) = _mm256_mask_i32gather_ps(_mm256_setzero_ps(), slp, mx_type_reg(high), mx_type_reg(vmask.f), I32_SIZE);
        } // if

        mx_type_reg(diff) = _mm256_sub_epi32(mx_type_reg(x[2]), mx_type_reg(x[0]));
        mx_type_reg(tmp) = _mm256_mul_ps(_mm256_cvtepi32_ps(mx_type_reg(diff)), mx_type_reg(slope));
        //mx_type_reg(tmp) = _mm256_round_ps(mx_type_reg(tmp), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

        mx_type_reg(y[2]) = _mm256_add_epi32(mx_type_reg(y[0]), _mm256_cvtps_epi32(mx_type_reg(tmp)));
        mx_type_reg(y[2]) = _mm256_blendv_epi8(mx_type_reg(y[2]), _mm256_set1_epi32(*left), mx_type_reg(vmask_left));
        mx_type_reg(y[2]) = _mm256_blendv_epi8(mx_type_reg(y[2]), _mm256_set1_epi32(*right), mx_type_reg(vmask_right));

        _mm256_maskstore_epi32(dst, mx_type_reg(*xmask), mx_type_reg(y[2]));

        src += k;
        dst += k;
        k = I32S_IN_V8SI;
        xmask = &v8si_mask[k];
    } // for
} // mx_v8si_interpolate
