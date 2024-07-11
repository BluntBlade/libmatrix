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
    I32_VALS_IN_V4SI = sizeof(v4si_t) / sizeof(int32_t),               // 4 values in pack.
    I64_VALS_IN_V2DI = sizeof(v2di_t) / sizeof(int64_t),               // 2 values in pack.
    F32_VALS_IN_V4SF = sizeof(v4sf_t) / sizeof(float),                 // 4 values in pack.
    D64_VALS_IN_V2DF = sizeof(v2df_t) / sizeof(double),                // 2 values in pack.

    I32_VALS_IN_V8SI = sizeof(v8si_t) / sizeof(int32_t),               // 8 values in pack.
    I64_VALS_IN_V4DI = sizeof(v4di_t) / sizeof(int64_t),               // 4 values in pack.
    F32_VALS_IN_V8SF = sizeof(v8sf_t) / sizeof(float),                 // 8 values in pack.
    D64_VALS_IN_V4DF = sizeof(v4df_t) / sizeof(double)                 // 4 values in pack.
};

enum {
    I32_VALS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(int32_t), // 16 values in cache line.
    I64_VALS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(int64_t), // 8 values in cache line.
    F32_VALS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(float),   // 16 values in cache line.
    D64_VALS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(double)   // 8 values in cache line.
};

enum {
    V4SI_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4si_t),      // 4 packs in cache line.
    V2DI_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v2di_t),      // 2 packs in cache line.
    V4SF_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4sf_t),      // 4 packs in cache line.
    V2DF_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v2df_t),      // 2 packs in cache line.

    V8SI_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v8si_t),      // 8 packs in cache line.
    V4DI_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4di_t),      // 4 packs in cache line.
    V8SF_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v8sf_t),      // 8 packs in cache line.
    V4DF_PCKS_IN_CACHE_LINE = MX_BYTES_IN_CACHE_LINE / sizeof(v4df_t)       // 4 packs in cache line.
};

typedef union MX_CHUNK {
    // 128 bits.
    v4si_t v4si_pcks[I32_VALS_IN_CACHE_LINE][V4SI_PCKS_IN_CACHE_LINE];
    v2di_t v2di_pcks[I64_VALS_IN_CACHE_LINE][V2DI_PCKS_IN_CACHE_LINE];
    v4sf_t v4sf_pcks[F32_VALS_IN_CACHE_LINE][V4SF_PCKS_IN_CACHE_LINE];
    v2df_t v2df_pcks[D64_VALS_IN_CACHE_LINE][V2DF_PCKS_IN_CACHE_LINE];

    // 256 bits.
    v8si_t v8si_16x2[16][2];
    v8si_t v8si_16x1[16][1];
    v8sf_t v8sf_16x2[16][2];
    v8sf_t v8sf_16x1[16][1];

    v4di_t v4di_pcks[I64_VALS_IN_CACHE_LINE][V4DI_PCKS_IN_CACHE_LINE];
    v4df_t v4df_pcks[D64_VALS_IN_CACHE_LINE][V4DF_PCKS_IN_CACHE_LINE];

    int32_t i32_16x16[16][16];
    int32_t i32_16x8[16][8];

    float   f32_16x16[16][16];
    float   f32_16x8[16][8];
} mx_chunk_t __attribute__ ((aligned (32)));

typedef mx_chunk_t * mx_chunk_ptr;

#endif // MX_TYPES_H

