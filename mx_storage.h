#ifndef MX_STORAGE_H
#define MX_STORAGE_H 1

// =============================================================================
// MODULE: Matrix Storage
// DESIGN GOAL:
//   1. Cope with the allocation, alignment, and fast access to memory;
//   2. Optimize memory access by using proper instructions, depends on CPU
//      model and features.
// =============================================================================

// ==== Storage Struct ====

struct MX_STORAGE;
typedef struct MX_STORAGE * mx_stor_ptr;

extern mx_stor_ptr mstr_create(uint32_t rows, uint32_t cols, uint32_t val_sz);
extern void mstr_destroy(mx_stor_ptr ms);

extern void mstr_v8si_init_zeros(ms_stor_ptr ms);
extern void mstr_v8si_init_ident(ms_stor_ptr ms);

extern void mstr_v8si_fill(ms_stor_ptr ms, int32_t val);

extern void mstr_v8si_assemble_chunk_from_rows(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk);
extern void mstr_v8si_assemble_chunk_from_columns(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk);

inline static void mstr_v8si_init_ones(ms_stor_ptr ms)
{
    return mstr_v8si_fill(ms, 1);
} // mstr_v8si_init_ones

#endif // MX_STORAGE_H

