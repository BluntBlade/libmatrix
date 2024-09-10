#ifndef MX_TYPES_H
#define MX_TYPES_H 1

#include <stdint.h>
#include <stdbool.h>
#include <immintrin.h>

#include "config.h"

// ==== Vector Types ====

typedef union
{
    __m128i reg;
    int32_t val[4];
} v4si_t __attribute__ ((aligned (16)));

typedef union
{
    __m128i reg;
    int64_t val[2];
} v2di_t __attribute__ ((aligned (16)));

typedef union
{
    __m128 reg;
    float  val[4];
} v4sf_t __attribute__ ((aligned (16)));

typedef union
{
    __m128d reg;
    double  val[2];
} v2df_t __attribute__ ((aligned (16)));

typedef union
{
    __m256i reg;
    int32_t val[8];
} v8si_t __attribute__ ((aligned (32)));

typedef union
{
    __m256i reg;
    int64_t val[4];
} v4di_t __attribute__ ((aligned (32)));

typedef union
{
    __m256 reg;
    float  val[8];
} v8sf_t __attribute__ ((aligned (32)));

typedef union
{
    __m256d reg;
    double  val[4];
} v4df_t __attribute__ ((aligned (32)));

#define mx_type_reg(u) ((u).reg)
#define mx_type_val(u) ((u).val)

enum {
    I32_SIZE     = sizeof(int32_t),
    I64_SIZE     = sizeof(int64_t),
    F32_SIZE     = sizeof(float),
    D64_SIZE     = sizeof(double),
};

enum {
    I32S_IN_V4SI = sizeof(v4si_t) / I32_SIZE,       // 4 values in pack.
    I64S_IN_V2DI = sizeof(v2di_t) / I64_SIZE,       // 2 values in pack.
    F32S_IN_V4SF = sizeof(v4sf_t) / F32_SIZE,       // 4 values in pack.
    D64S_IN_V2DF = sizeof(v2df_t) / D64_SIZE,       // 2 values in pack.

    I32S_IN_V8SI = sizeof(v8si_t) / I32_SIZE,       // 8 values in pack.
    I64S_IN_V4DI = sizeof(v4di_t) / I64_SIZE,       // 4 values in pack.
    F32S_IN_V8SF = sizeof(v8sf_t) / F32_SIZE,       // 8 values in pack.
    D64S_IN_V4DF = sizeof(v4df_t) / D64_SIZE,       // 4 values in pack.
};

enum {
    I32S_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / I32_SIZE, // 16 values in cache line.
    I64S_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / I64_SIZE, // 8 values in cache line.
    F32S_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / F32_SIZE, // 16 values in cache line.
    D64S_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / D64_SIZE, // 8 values in cache line.
};

enum {
    V4SIS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4si_t), // 4 packs in cache line.
    V2DIS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v2di_t), // 2 packs in cache line.
    V4SFS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4sf_t), // 4 packs in cache line.
    V2DFS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v2df_t), // 2 packs in cache line.

    V8SIS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v8si_t), // 8 packs in cache line.
    V4DIS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4di_t), // 4 packs in cache line.
    V8SFS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v8sf_t), // 8 packs in cache line.
    V4DFS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4df_t)  // 4 packs in cache line.
};

typedef union MX_CHUNK {
    // 256 bits.
    v8si_t v8si_16x2[16][2];
    v8si_t v8si_16x1[16][1];
    v8sf_t v8sf_16x2[16][2];
    v8sf_t v8sf_16x1[16][1];

    int32_t i32_16x16[16][16];
    int32_t i32_16x8[16][8];

    float   f32_16x16[16][16];
    float   f32_16x8[16][8];
} mx_chunk_t __attribute__ ((aligned (32)));

typedef mx_chunk_t * mx_chunk_ptr;

extern v8si_t v8si_zero;
extern v8si_t v8si_idx;
extern v8si_t v8si_mask[9];
extern v8sf_t v8sf_zero;
extern v8sf_t v8sf_mask[9];

#endif // MX_TYPES_H

