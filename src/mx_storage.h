#ifndef MX_STORAGE_H
#define MX_STORAGE_H 1

// =============================================================================
// MODULE: Matrix Storage
// DESIGN GOAL:
//   1. Cope with the allocation, alignment, and fast access to memory;
//   2. Optimize memory access by using proper instructions, depends on CPU
//      model and features.
// =============================================================================

#include "mx_types.h"

// ==== Storage Module ==== //

struct MX_STORAGE;
typedef struct MX_STORAGE * mx_stor_ptr;

extern mx_stor_ptr mstr_v8si_create(uint32_t rows, uint32_t cols);
extern void mstr_v8si_destroy(mx_stor_ptr ms);

extern uint32_t mstr_v8si_rows(mx_stor_ptr ms);
extern uint32_t mstr_v8si_columns(mx_stor_ptr ms);
extern uint32_t mstr_v8si_values(mx_stor_ptr ms);
extern uint32_t mstr_v8si_value_size(mx_stor_ptr ms);

extern uint32_t mstr_v8si_chunks_in_height(mx_stor_ptr ms);
extern uint32_t mstr_v8si_chunks_in_width(mx_stor_ptr ms);
extern uint32_t mstr_v8si_chunks(mx_stor_ptr ms);

extern void mstr_v8si_init_zeros(mx_stor_ptr ms);
extern void mstr_v8si_init_identity(mx_stor_ptr ms);

extern int32_t mstr_v8si_get(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx);
extern void mstr_v8si_set(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t src);

extern void mstr_v8si_fill(mx_stor_ptr ms, int32_t val);

extern mx_chunk_ptr mstr_v8si_copy_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk, uint32_t * rows_in_chk, uint32_t * cols_in_chk, bool * full);
extern mx_chunk_ptr mstr_v8si_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk, uint32_t * rows_in_chk, uint32_t * cols_in_chk, bool * full);

extern void mstr_v8si_store_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk);

inline static void mstr_v8si_init_ones(mx_stor_ptr ms)
{
    return mstr_v8si_fill(ms, 1);
} // mstr_v8si_init_ones

#endif // MX_STORAGE_H

