#ifndef MX_STORAGE_H
#define MX_STORAGE_H 1

#include "src/mx_types.h"

typedef struct MX_STORAGE {
    uint16_t    val_sz;             // The size of one value, in bytes.
    uint16_t    alignment;          // The byte boundary to align with.
    uint16_t    pck_len;            // The number of values in one pack.
    uint16_t    chk_len;            // The number of rows or columns in one chunk.
    uint16_t    last_chk_width;     // The number of columns in the last chunk.
    uint16_t    last_chk_height;    // The number of rows in the last chunk.
    uint32_t    chks_in_width;      // The number of chunks in the width of the whole matrix.
    uint32_t    chks_in_height;     // The number of chunks in the height of the whole matrix.
    uint32_t    rows;               // The number of rows.
    uint32_t    cols;               // The number of columns.
    uint32_t    cols_padded;        // The actual number of columns, including padding ones.
    size_t      bytes;              // Total bytes that store values, including no aligned ones.

    void *      buf;                // Non-aligned address of allocated memory, only for calling free().
    void *      data;               // Aligned address of allocated memory.
} mx_stor_t, *mx_stor_ptr;

extern mx_stor_ptr mstr_init(mx_stor_ptr ms, uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len);
extern void mstr_clean(mx_stor_ptr ms);

extern mx_stor_ptr mstr_create(uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len);
extern void mstr_destroy(mx_stor_ptr ms);

#endif // MX_STORAGE_H

