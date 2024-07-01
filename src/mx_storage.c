#include <stdlib.h>

#include "src/mx_common.h"
#include "src/mx_storage.h"

mx_stor_ptr mstr_init(mx_stor_ptr ms, uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len){
    ms->val_sz = val_sz;
    ms->rows = rows;
    ms->cols = cols;
    ms->cols_padded = mx_round_to_multiples(cols, pck_len);
    ms->bytes = ms->val_sz * ms->rows * ms->cols_padded;
    ms->pck_len = pck_len;
    ms->chk_len = chk_len;
    ms->last_chk_width = cols - (mx_round_to_multiples(cols, chk_len) - chk_len);
    ms->last_chk_height = rows - (mx_round_to_multiples(rows, chk_len) - chk_len);
    ms->chks_in_width = mx_round_to_multiples(cols, chk_len) / chk_len;
    ms->chks_in_height = mx_round_to_multiples(rows, chk_len) / chk_len;

    ms->alignment = ms->val_sz * ms->pck_len;
    ms->buf = malloc(ms->bytes + ms->alignment);
    if (! ms->buf) {
        return NULL;
    } /* if */
    
    /* NOTE: Align to the vector type's size, otherwise it will be segmentation fault when access to packs. */
    ms->data = (void *)( ((size_t)ms->buf + (ms->alignment - 1)) & (~(ms->alignment - 1)));
    return ms;
} // mstr_init

void mstr_clean(mx_stor_ptr ms)
{
    free(ms->buf);
} // mstr_clean

mx_stor_ptr mstr_create(uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len)
{
    uint32_t i = 0;
    mx_stor_ptr ms = NULL;

    ms = malloc(sizeof(mx_stor_t));
    if (! ms) {
        return NULL;
    } // if

    if (! mstr_init(ms, rows, cols, val_sz, pck_len, chk_len)) {
        free(ms);
        return NULL;
    } // if
    return ms;
} // mstr_create

void mstr_destroy(mx_stor_ptr ms)
{
    if (ms) {
        mstr_clean(ms);
    } // if
    free(ms);
} // mstr_destroy
