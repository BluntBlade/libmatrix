#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "matrix.h"

enum {
    PLAIN = 0x1,
    SIMD = 0x2,
    ALL = 0x3,
};

int main(int argc, const char * argv[])
{
    struct timeval begin = {0};
    struct timeval end = {0};
    long long diff = 0;
    int i = 0;
    int cnt = 1000;
    int n = 256;
    unsigned int code = 0;
    ptr_matrix_t lhs = NULL;
    ptr_matrix_t rhs = NULL;
    ptr_matrix_t m = NULL;

    if (argc > 1) {
        cnt = atoi(argv[1]); 
    } /* if */

    if (argc > 2) {
        n = atoi(argv[2]); 
    } /* if */

    code = ALL;
    if (argc > 3) {
        code = 0;
        if (strcmp(argv[3], "plain") == 0) {
            code |= PLAIN;
        } else if (strcmp(argv[3], "simd") == 0) {
            code |= SIMD;
        } /* if */
    } /* if */

    lhs = mtx_i32_allocate(n, n);
    rhs = mtx_i32_allocate(n, n);
    m = mtx_allocate_for_multiplying(lhs, rhs);

    printf("Multiply %dx%d matrices, iterate for %d times.\n", n, n, cnt);

    mtx_initialize_ones(lhs, MTX_PLAIN_CODE);
    mtx_initialize_ones(rhs, MTX_PLAIN_CODE);

    if (code & PLAIN) {
        printf("Calculating using plain code.\n");
        gettimeofday(&begin, NULL);
        for (i = 0; i < cnt; i += 1) {
            mtx_multiply_and_store(m, lhs, rhs, MTX_PLAIN_CODE);
        } /* for */
        gettimeofday(&end, NULL);
        diff = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
        printf("Time for mtx_multiply_and_store() using PLAIN code = %7.6fs\n", (float)diff / 1000000);
    } /* if */

    if (code & SIMD) {
        printf("Calculating using simd code.\n");
        gettimeofday(&begin, NULL);
        for (i = 0; i < cnt; i += 1) {
            mtx_multiply_and_store(m, lhs, rhs, MTX_SIMD_CODE);
        } /* for */
        gettimeofday(&end, NULL);
        diff = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
        printf("Time for mtx_multiply_and_store() using SIMD code = %7.6fs\n", (float)diff / 1000000);
    } /* if */

    mtx_destroy(m);
    mtx_destroy(rhs);
    mtx_destroy(lhs);
    return 0;
}
