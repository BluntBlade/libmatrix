#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "src/v8si_storage.h"
#include "src/mx_expression.h"

void pure_v8si_filter(mx_stor_ptr dst, mx_stor_ptr src)
{
    v8si_t left;
    v8si_t right;
    v8si_t vec;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t row_in_chk = 0;
    uint32_t col_in_chk = 0;
    void * base = NULL;

    for (i = 0; i < mstr_v8si_chunks_in_height(src); i += 1) {
        for (j = 0; j < mstr_v8si_chunks_in_width(src); j += 1) {
            base = mstr_v8si_locate_chunk(src, i, j, &row_in_chk, &col_in_chk);

            for (row = I32_VALS_IN_CACHE_LINE * i; row < I32_VALS_IN_CACHE_LINE * i + row_in_chk; row += 1) {
                for (col = I32_VALS_IN_CACHE_LINE * j; col < I32_VALS_IN_CACHE_LINE * j + col_in_chk; col += 8) {
                    mstr_v8si_load_row_vector(src, row, col, 0, -1, 0, &left);
                    mstr_v8si_load_row_vector(src, row, col, 0, 1, 0, &right);
                    mstr_v8si_load_row_vector(src, row, col, 0, 0, 0, &vec);
                   
                    mx_type_reg(vec) = _mm256_add_epi32(mx_type_reg(vec), mx_type_reg(left));
                    mx_type_reg(vec) = _mm256_add_epi32(mx_type_reg(vec), mx_type_reg(right));

                    mstr_v8si_store_row_vector(dst, row, col, 0, 0, &vec);
                } // for
            } // for
        } // for
    } // for
} // pure_v8si_filter

int main(int argc, const char * argv[])
{
    struct timeval begin = {0};
    struct timeval end = {0};
    long long diff = 0;
    int i = 0;
    int j = 0;
    int cnt = 1000;
    int n = 16;
    int m = 16;
    mx_stor_ptr src = NULL;
    mx_stor_ptr dst = NULL;

    if (argc > 1) {
        cnt = atoi(argv[1]); 
    } // if

    if (argc > 2) {
        n = atoi(argv[2]); 
        m = n;
    } // if

    if (argc > 3) {
        m = atoi(argv[3]); 
    } // if

    src = mstr_v8si_create(n, m);
    dst = mstr_v8si_create(n, m);

    printf("Filter %dx%d matrices, iterate for %d times.\n", n, m, cnt);

    mstr_v8si_init_ones(src);
    mstr_v8si_init_zeros(dst);

    v8si_t vec;
    v8si_t up;
    v8si_t down;
    v8si_t left;
    v8si_t right;
    uint32_t dval = 0;
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t vec_off = 0;
    uint32_t up_off = 0;
    uint32_t down_off = 0;
    uint32_t left_off = -1;
    uint32_t right_off = 1;

    printf("Calculating using simd code.\n");
    gettimeofday(&begin, NULL);
    for (i = 0; i < cnt; i += 1) {
        pure_v8si_filter(dst, src);
    } // for
    gettimeofday(&end, NULL);
    diff = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    printf("Time for mexp_v8si_filter() using SIMD code = %7.6fs\n", (float)diff / 1000000);

    for (i = 0; i < mstr_v8si_rows(dst); i += 1) {
        for (j = 0; j < mstr_v8si_columns(dst); j += 1) {
            printf("%d\t", mstr_v8si_get(dst, i, j));
        } // for
        printf("\n");
    } // for

    mstr_v8si_destroy(dst);
    mstr_v8si_destroy(src);
    return 0;
}
