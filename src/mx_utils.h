#ifndef MX_UTILS_H
#define MX_UTILS_H 1

#include "src/mx_types.h"

extern void mx_v8si_bisearch(v8si_t * dst_idx, int32_t * rng, int32_t n, v8si_t * src);
extern void mx_v8sf_bisearch(v8si_t * dst_idx, float * rng, int32_t n, v8sf_t * src);

extern void mx_v8si_interpolate(int32_t * y_out, uint32_t pn, int32_t * xp, int32_t * fp, float * slp, int32_t * left, int32_t * right, uint32_t xn, int32_t * x_in);

#endif // MX_UTILS_H

